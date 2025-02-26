#include "../include/transformation.hpp"
#include <sstream>


//Función auxiliar para unir strings con separador
string join(const vector<string>& elements, const string& separator) {
    ostringstream result;
    for (size_t i = 0; i < elements.size(); ++i) {
        result << elements[i];
        if (i < elements.size() - 1) result << separator;
    }
    return result.str();
}

vector<json> getJsonRecords(const json& data, const string& sourcePath) {
    //cout << "GetJsonRecords: Buscando en sourcePath: " << sourcePath << endl;
    vector<json> records;
    size_t pos = sourcePath.find("."); //la idea es dividir la clave en 2 partes ya q viene "universidad.alumno"
    if (pos != string::npos) {
        string firstKey = sourcePath.substr(0, pos);
        string restKey = sourcePath.substr(pos + 1);//para separar el .
        if (data.contains(firstKey)) {
            records = getJsonRecords(data[firstKey], restKey); //llmada recursiva 
        }
    } else {
        if (data.contains(sourcePath)) {
            for (const auto& rec : data[sourcePath]) {
                records.push_back(rec);
            }
        } else {
            cout << "No se encontró la ruta " << sourcePath << endl;
        }
    }
    //cout << "records: : " << records << endl;
    return records;
}

string getValueFromJson(const json& record, const string& jsonPath) {
    //cout <<"getValueFromJson: \n";
    if (record.contains(jsonPath)) {
        //return record[jsonPath].dump();
        string value = record[jsonPath].dump();
        if (value.size() > 1 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        //cout << "value: : " << value << endl;
        return value;
    }
    return "";
}

map<string, vector<string>> jsonToSqlInsert(const json& data, const json& config) {
    map<string, vector<string>> insertQueries;
    
    // orden ejecucion query tablas
    vector<string> loadOrder = config["globalOptions"]["loadOrder"];

    for (const auto& tableName : loadOrder) {
        const json& tableConfig = config["tables"][tableName];
        bool generatedId = tableConfig.value("generatedId", false);
        string sourcePath = tableConfig["sourcePath"];

        vector<json> records = getJsonRecords(data, sourcePath);
        vector<string> queries;

        for (const auto& record : records) {
            ostringstream query;
            vector<string> columns;
            vector<string> values;

            // columnas
            for (const auto& col : tableConfig["columns"]) {
                string colName = col["name"];
                string jsonPath = col["jsonPath"];
                string value = getValueFromJson(record, jsonPath);

                // Saltar columna si es un ID generado
                if (col.value("isGeneratedId", false)) continue;

                columns.push_back("\"" + colName + "\"");
                values.push_back("'" + value + "'");
            }

            query << "INSERT INTO \"" << tableName << "\" ("
                  << join(columns, ", ") << ") VALUES (" 
                  << join(values, ", ") << ")";

            // Añadir RETURNING id si hay IDs generados
            if (generatedId) {
                query << " RETURNING id";
            }

            queries.push_back(query.str() + ";");
        }

        insertQueries[tableName] = queries;
    }

    return insertQueries;
}

//todo: revizar cambiar y arreglar
vector<string> generateRelationshipInsertQueries(const json& data, const json& config,
    const unordered_map<string, unordered_map<string, int>>& tableIdMapping) {

    vector<string> relQueries;
    if (!config.contains("relationships"))
        return relQueries;

    for (const auto& rel : config["relationships"]) {
        if (rel["type"] != "MANY_TO_MANY")
            continue;

        string tableA = rel["tableA"];
        string tableB = rel["tableB"];
        string junctionTable = rel["junctionTable"];
        string fkA = rel["foreignKeys"][tableA];
        string fkB = rel["foreignKeys"][tableB];

        // Se obtienen los registros de la tabla A a partir del JSON
        vector<json> recordsA = getJsonRecords(data, config["tables"][tableA]["sourcePath"]);
        for (const auto& recordA : recordsA) {
            // Se utiliza "uniqueKey" como identificador único para mapear el registro
            string uniqueKeyA = recordA.contains("uniqueKey") ? getValueFromJson(recordA, "uniqueKey") : "";
            if (uniqueKeyA.empty() || tableIdMapping.find(tableA) == tableIdMapping.end() ||
                tableIdMapping.at(tableA).find(uniqueKeyA) == tableIdMapping.at(tableA).end())
                continue;
            int idA = tableIdMapping.at(tableA).at(uniqueKeyA);

            // Se asume que los registros de tableB están anidados en recordA bajo la clave con el nombre de tableB
            vector<json> recordsB = getJsonRecords(recordA, tableB);
            for (const auto& recordB : recordsB) {
                string uniqueKeyB = recordB.contains("uniqueKey") ? getValueFromJson(recordB, "uniqueKey") : "";
                if (uniqueKeyB.empty() || tableIdMapping.find(tableB) == tableIdMapping.end() ||
                    tableIdMapping.at(tableB).find(uniqueKeyB) == tableIdMapping.at(tableB).end())
                    continue;
                int idB = tableIdMapping.at(tableB).at(uniqueKeyB);

                ostringstream query;
                query << "INSERT INTO \"" << junctionTable << "\" (\"" << fkA << "\", \"" << fkB << "\"";
                vector<string> values = { to_string(idA), to_string(idB) };

                if (rel.contains("extraColumns")) {
                    for (const auto& col : rel["extraColumns"]) {
                        string colName = col["name"];
                        string value = getValueFromJson(recordB, col["jsonPath"]);
                        query << ", \"" << colName << "\"";
                        values.push_back(value.empty() ? "NULL" : "'" + value + "'");
                    }
                }
                query << ") VALUES (" << join(values, ", ") << ");";
                relQueries.push_back(query.str());
            }
        }
    }
    return relQueries;
}