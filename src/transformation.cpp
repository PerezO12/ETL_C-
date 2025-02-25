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

    for (auto& [tableName, tableConfig] : config["tables"].items()) {

        string sourcePath = tableConfig["sourcePath"];
        //cout << "sourcePath: " << sourcePath <<"\n" ;
        vector<json> records = getJsonRecords(data, sourcePath);
        //cout << "records: " << records <<"\n" ;
        vector<string> queries;

        for (const auto& record : records) {
            ostringstream query;
            query << "INSERT INTO \"" << tableName << "\" (";
            
            vector<string> columns;
            vector<string> values;
            
            for (const auto& col : tableConfig["columns"]) {
                string colName = col["name"];
                string jsonPath = col["jsonPath"];
                string value = getValueFromJson(record, jsonPath);

                columns.push_back("\"" + colName + "\"");
                values.push_back( "'" + value + "'");
            }
            
            for (size_t i = 0; i < columns.size(); ++i) {
                query << columns[i];
                if (i < columns.size() - 1)
                    query << ", ";
            }
            query << ") VALUES (";
            
            for (size_t i = 0; i < values.size(); ++i) {
                query << values[i];
                if (i < values.size() - 1)
                    query << ", ";
            }
            query << ");";
            
            queries.push_back(query.str());
        }
        insertQueries[tableName] = queries;
    }
    //procesar relaciones 
    //todo: de momento solo manejara Many-To-Many
    if (config.contains("relationships")) {
        for (const auto& rel : config["relationships"]) {
            if (rel["type"] != "MANY_TO_MANY") continue;
            
            string tableA = rel["tableA"];
            string tableB = rel["tableB"];
            string junctionTable = rel["junctionTable"];
            string fkA = rel["foreignKeys"][tableA];
            string fkB = rel["foreignKeys"][tableB];

            vector<string> relQueries;

            vector<json> recordsA = getJsonRecords(data, config["tables"][tableA]["sourcePath"]);
            for (const auto& recordA : recordsA) {
                string idA = getValueFromJson(recordA, "id");

                vector<json> recordsB = getJsonRecords(recordA, tableB);
                for (const auto& recordB : recordsB) {
                    string idB = getValueFromJson(recordB, "id");

                    if (idA.empty() || idB.empty()) continue;

                    ostringstream relQuery;
                    relQuery << "INSERT INTO \"" << junctionTable << "\" (\"" << fkA << "\", \"" << fkB << "\"";

                    vector<string> relValues = {"'" + idA + "'", "'" + idB + "'"};

                    if (rel.contains("extraColumns")) {
                        for (const auto& col : rel["extraColumns"]) {
                            string colName = col["name"];
                            string value = getValueFromJson(recordB, col["jsonPath"]);
                            relQuery << ", \"" << colName << "\"";
                            relValues.push_back(value.empty() ? "NULL" : "'" + value + "'");
                        }
                    }
                    
                    relQuery << ") VALUES (" << join(relValues, ", ") << ");";
                    relQueries.push_back(relQuery.str());
                }
            }
            insertQueries[junctionTable] = relQueries;
        }
    }

    return insertQueries;
}

