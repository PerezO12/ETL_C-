#include "../include/transformation.hpp"
#include <sstream>


vector<json> getJsonRecords(const json& data, const string& sourcePath) {
    //cout << "GetJsonRecords: Buscando en sourcePath: " << sourcePath << endl;
    vector<json> records;

    size_t pos = sourcePath.find(".");
    if (pos != string::npos) {
        string firstKey = sourcePath.substr(0, pos);
        string restKey = sourcePath.substr(pos + 1);
        if (data.contains(firstKey)) {
            records = getJsonRecords(data[firstKey], restKey);
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
        return value;
    }
    return "";
}

map<string, vector<string>> jsonToSqlMapper(const json& data, const json& config) {
    //cout << "Transformando data\n";

    map<string, vector<string>> insertQueries;

    for (auto& [tableName, tableConfig] : config["tables"].items()) {

        string sourcePath = tableConfig["sourcePath"];
        //cout << "sourcePath: " << sourcePath <<"\n" ;
        vector<json> records = getJsonRecords(data, sourcePath);
        //cout << "records: " << records <<"\n" ;

        vector<string> queries;
        
        // Iterar sobre cada registro y construir la sentencia INSERT
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
            
            // Construir la parte de columnas
            for (size_t i = 0; i < columns.size(); ++i) {
                query << columns[i];
                if (i < columns.size() - 1)
                    query << ", ";
            }
            query << ") VALUES (";
            
            // Construir la parte de valores
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

    // Aquí podrías agregar lógica para transformar relaciones, si fuera necesario,
    // por ejemplo, generando queries para las tablas de relación definidas en config["relationships"].

    return insertQueries;
}

