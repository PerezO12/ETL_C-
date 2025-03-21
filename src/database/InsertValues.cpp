
#include "../../include/database/InsertValues.hpp"

InsertValues::InsertValues(QueryExecutor& executor): executor(executor)
{

}

string InsertValues::lookupId(const string& table, const map<string, string>& naturalKey) {
    try {
        if (naturalKey.empty()) {
            //throw runtime_error("Clave natural inválida: debe contener pares clave-valor.");
            cerr << "Clave natural inválida: debe contener pares clave-valor." << endl;
            //add un log
            return "";
        }
        //cout << endl << "Desde lokupId table: " << table << endl;
        string query = "SELECT id FROM " + table + " WHERE ";

        for (auto it = naturalKey.begin(); it != naturalKey.end(); ) {
            const string& column = it->first;
            const string& value = it->second;

            query += column + " = '" + value + "'";

            if (++it != naturalKey.end()) {
                query += " AND ";
            }
        }

       // cout << endl << "Desde lokupId Query: " << query << endl;
        string idStr = executor.executeQueryReturningId(query);
        //cout << endl << "Desde lokupId ID retornado: " << idStr << endl;
        return idStr;
    }  catch (const exception& e) {
        //throw runtime_error("Error en lookupId: " + string(e.what()));
        cerr << "Advertencia: No se encontró ID para tabla " << table << " con claves: " << endl;
        for (const auto& [column, value] : naturalKey) {
            cout << "columna: " << column << ", value: " << value << endl;
        }
        cout << endl;
        return "";
    }
}



void InsertValues::batchInsert(const string& table, const vector<json>& records, const json& tableConfig) {
    try {
        executor.executeQuery("BEGIN");
        //cout << endl << "Insertando datos en tabla: " << table << endl;

        size_t batchSize = tableConfig.contains("batchSize") ? tableConfig["batchSize"].get<size_t>() : 1000;
        
        string baseQuery = "INSERT INTO " + table + " (";

        for (const auto& column : tableConfig["columns"]) {
            baseQuery += column["name"].get<string>() + ", ";
        }
        baseQuery = baseQuery.substr(0, baseQuery.size() - 2) + ") VALUES ";

        string query = baseQuery;
        size_t count = 0;

        for (const auto& record : records) {
            bool validRecord = true; // Bandera para verificar si el registro es válido
            string currentRecord = "(";
        
            for (const auto& column : tableConfig["columns"]) {
                string value = "";
                if (column.contains("lookup")) {
                    string naturalKeyValue = getValueFromJson(record, column["jsonPath"]);
                    vector<string> naturalKeyValueVector = Utility::splitStringByDelimiter(naturalKeyValue, " ");
                    map<string, string> naturalKeykeyValue = Utility::createNaturalKeyMap(column["lookup"]["naturalKey"], naturalKeyValueVector);
                    value = lookupId(column["lookup"]["table"], naturalKeykeyValue);
                    if(value.empty()) {
                        validRecord = false;
                        break;
                    }
/*                     try {
                    } catch (const std::exception& e) {
                        validRecord = false; 
                        break;
                    } */
                } else {
                    value = getValueFromJson(record, column["jsonPath"]);
                    string type = column["type"].get<string>();
                    //todo: cambiar luego
                    if (type == "jsonb") {
                        value = "'" + value + "'::jsonb"; 
                    } else if (type == "int") {
                        value = value; 
                    } else {
                        value = "'" + value + "'";
                    }
                }
                currentRecord += value + ", ";
            }
            //cambiar esto para insertar en logs
            if (!validRecord) {
                //cout << "Registro omitido (fallo en clave foránea) ************************" << endl;
                continue;
            }
            currentRecord = currentRecord.substr(0, currentRecord.size() - 2) + "), ";
            query += currentRecord;
            cout << endl << "query Antes de ejecutar" << query << endl;
            count++;

            if (count % batchSize == 0) {
                query = query.substr(0, query.size() - 2) + ";"; 
                executor.executeQuery(query);
                query = baseQuery;
            }
        }

        if (count % batchSize != 0) {
            query = query.substr(0, query.size() - 2) + ";";
            cout<< "MDB: Query: " <<endl << query << endl;
            executor.executeQuery(query);
        }

        executor.executeQuery("COMMIT");
    } catch (const exception& e) {
        executor.executeQuery("ROLLBACK");
        throw runtime_error("Error en batchInsert: " + string(e.what()));
    }
}

void InsertValues::processRelationships(const json& config, const json& relationshipsConfig, const json& data, const string& rootPath) {
    try {  
        //cout << endl << "estoy aqui" << endl;
        //todo: cambiar, de momento solo se estan teniendo en cuenta las relaciones many to many
        for (const auto& rel : relationshipsConfig) {
            if (rel["type"] != "MANY_TO_MANY") continue;

            const string junctionTable = rel["junctionTable"].get<string>();
            const string fromTable = rel["fromTable"].get<string>();
            const string toTable = rel["toTable"].get<string>();
            const json& columnsConfig = rel["columns"];
            const string toDataPath = rel["dataPath"].get<string>();

            const json& fromRecords = data[rootPath][fromTable];
            vector<json> junctionRecords;
            for (const auto& fromRecord : fromRecords) {
                map<string, string> fromNaturalKey;

                //cout << endl << "fromRecord: "<< fromRecord << endl;

                for (const auto& key : config["tables"][fromTable]["naturalKey"]) {
                    fromNaturalKey[key.get<string>()] = getValueFromJson(fromRecord, key.get<string>());
                }
                string fromId = lookupId(fromTable, fromNaturalKey);
                if (fromId.empty()) {
                    cerr << "Advertencia: Omitiendo registro de " << fromTable << endl;//" (clave: " << fromNaturalKey << ")" << endl;
                    continue;
                }

                //cout << endl << "fromId: "<< fromId << endl;

                for (const auto& toData : fromRecord[toDataPath]) { 
                    map<string, string> toNaturalKey;
                    for (const auto& key : config["tables"][toTable]["naturalKey"]) {
                        toNaturalKey[key.get<string>()] = getValueFromJson(toData, key.get<string>());
                    }
                    string toId = lookupId(toTable, toNaturalKey);
                    if (toId.empty()) {
                        cerr << "Advertencia: Omitiendo relación hacia " << toTable << endl; //" (clave: " << toNaturalKey << ")" << endl;
                        continue;
                    }
                    //cout << endl << "toId: "<< toId << endl;
                    // Construir registro para la tabla de unión
                    json junctionRecord;
                    for (const auto& colConfig : columnsConfig) {
                        const string colName = colConfig["name"].get<string>();
                        
                        if (colName.find("_id") != string::npos) {
                            const string sourceTable = colConfig["source"]["table"].get<string>();
                            if (sourceTable == fromTable) {
                                junctionRecord[colName] = fromId;
                            } else {
                                junctionRecord[colName] = toId;
                            }
                            //junctionRecord[colName] = (colName == fromTable + "_id") ? fromId : toId; 
                        } else {
                            string jsonPath = colConfig["jsonPath"].get<string>();
                            junctionRecord[colName] = getValueFromJson(toData, jsonPath); 
                            //junctionRecord[colName] = toData[colConfig["jsonPath"]];
                        }
                    }
                    junctionRecords.push_back(junctionRecord);
                }
            }
            

            batchInsert(junctionTable, junctionRecords, rel);
            
        }
    } catch (const exception& e) {
        throw runtime_error("Error procesando relaciones: " + string(e.what()));
    }
}
