#include "../include/manager_db.hpp"
#include <unordered_set>
#include <stdexcept>


managerDb::managerDb(const string& host, const string& dbname, const string& user, const string& password, const string& port) {
    string conninfo = "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password + " port=" + port;

    conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Conexión a la base de datos fallida: " << PQerrorMessage(conn) << endl;
        throw runtime_error("Error en la conexión a la base de datos");
    } else {
        cout << "Conexión exitosa a la base de datos " << dbname << endl;
    }
}

managerDb::managerDb(const json& dbConfig) {
    try {
        const string host = dbConfig.at("host").get<string>();
        const string dbname = dbConfig.at("dbname").get<string>();
        const string user = dbConfig.at("user").get<string>();
        const string password = dbConfig.at("password").get<string>();
        const string port = dbConfig.at("port").get<string>();

        string conninfo = 
            "host=" + host + " " +
            "dbname=" + dbname + " " +
            "user=" + user + " " +
            "password=" + password + " " +
            "port=" + port;

        conn = PQconnectdb(conninfo.c_str());

        if (PQstatus(conn) != CONNECTION_OK) {
            string errorMsg = "Conexión a la base de datos fallida: " + string(PQerrorMessage(conn));
            PQfinish(conn);
            throw runtime_error(errorMsg);
        } else {
            cout << "Conexión exitosa a la base de datos " << dbname << endl;
        }
    } catch (const json::exception& e) {
        throw runtime_error("Error en la configuración de la base de datos: " + string(e.what()));
    } catch (const exception& e) {
        throw runtime_error("Error: " + string(e.what()));
    }
}

//insert
void managerDb::executeQuery(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Error en la consulta: " << PQerrorMessage(conn) << endl;
    } else {
        cout << "Consulta ejecutada con éxito: " << endl;
        cout << "Quer: " << query << endl;
    }

    PQclear(res);
}
string managerDb::executeQueryReturningId(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        string errorMsg = "Error en la consulta (RETURNING id): " + string(PQerrorMessage(conn));
        PQclear(res);
        throw runtime_error(errorMsg);
    }

    if (PQntuples(res) == 0 || PQnfields(res) == 0) {
        PQclear(res);
        throw runtime_error("La consulta " + query + " no retornó ningún ID.");
    }

    char* rawId = PQgetvalue(res, 0, 0);
    if (!rawId) {
        PQclear(res);
        throw runtime_error("El ID retornado es NULL.");
    }

    string id(rawId);
    PQclear(res);

    return id;
}

//get
PGresult* managerDb::excecuteSelectQuery(const string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
     cerr << "Error en la consulta SELECT: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return nullptr;
    }

    return res;
}


//tablas
void managerDb::createTable(const string tableName, const json& tableData) {
    try {
        vector<string> elements;
            //si en la conf d la tabla se pide que se autogenere la primary key
            bool generatedPK = tableData["generatedPK"].get<bool>();
            if (generatedPK) {
                elements.push_back("id SERIAL PRIMARY KEY");
            }

            for (const auto& column : tableData["columns"]) {
                string colDef = column["name"].get<string>() + " " + column["type"].get<string>();

                //info: add al readme q el json en al tabla puede tener campos de unique
                if (column.contains("unique") && column["unique"].get<bool>()) {
                    colDef += " UNIQUE";
                }

                // FK
                if (column.contains("lookup")) {
                    const string refTable = column["lookup"]["table"].get<string>();
                    colDef += " REFERENCES " + refTable + "(id)";
                }

                elements.push_back(colDef);
            }

            string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (\n    " + Utility::join(elements, ",\n    ") + "\n);";
            
            executeQuery(query);
            cout << "[OK] Tabla creada: " << tableName << endl;
    } catch (const exception& e) {
         throw runtime_error("Error creando tablas: " + string(e.what()));
    }
}

void managerDb::createTables(const json& tablesConfig, const vector<string> loadOrder) {
    try {
        for (const auto& table : loadOrder) {
            createTable(table, tablesConfig[table]);
        }
    } catch (const exception& e) {
        throw runtime_error("Error creando tablas: " + string(e.what()));
    }
}

void managerDb::createRelationships(const json& relationshipsConfig) {
    try {
        for (const auto& rel : relationshipsConfig) {
            if (rel["type"] == "MANY_TO_MANY") {
                vector<string> columns;
                
                for (const auto& col : rel["columns"]) {
                    string colDef = col["name"].get<string>() + " " + col["type"].get<string>();
                    
                    if (col.contains("source")) {
                        const string refTable = col["source"]["table"].get<string>();
                        colDef += " REFERENCES " + refTable + "(id)";
                    }
                    
                    columns.push_back(colDef);
                }
                
                string query = "CREATE TABLE IF NOT EXISTS " 
                                  + rel["junctionTable"].get<string>() + " (\n    " 
                                  + Utility::join(columns, ",\n    ") + "\n);";
                
                executeQuery(query);
                cout << "[OK] Tabla de relación creada: " << rel["junctionTable"] << endl;
            }
        }
    } catch (const exception& e) {
        throw runtime_error("Error creando relaciones: " + string(e.what()));
    }
}

void managerDb::truncateTables(const vector<string>& tables) {
    try {
        if (tables.empty()) {
            throw runtime_error("Lista de tablas vacía. No hay nada que truncar.");
        }

        string query = "TRUNCATE ";
        for (size_t i = 0; i < tables.size(); ++i) {
            query += tables[i];
            if (i != tables.size() - 1) {
                query += ", ";
            }
        }
        query += " RESTART IDENTITY CASCADE;";

        executeQuery(query);

        cout << "Tablas truncadas exitosamente: ";
        for (const auto& table : tables) {
            cout << table << " ";
        }
        cout << endl;

    } catch (const exception& e) {
        throw runtime_error("Error al truncar tablas: " + string(e.what()));
    }
}

//todo: si no retorna un id se rompe, corregir luego !!!!!!!!!!!!!
string managerDb::lookupId(const string& table, const map<string, string>& naturalKey) {
    try {
        if (naturalKey.empty()) {
            throw runtime_error("Clave natural inválida: debe contener pares clave-valor.");
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

        cout << endl << "Desde lokupId Query: " << query << endl;
        string idStr = executeQueryReturningId(query);
        cout << endl << "Desde lokupId ID retornado: " << idStr << endl;
        return idStr;
    }  catch (const exception& e) {
        throw runtime_error("Error en lookupId: " + string(e.what()));
    }
}


void managerDb::batchInsert(const string& table, const vector<json>& records, const json& tableConfig) {
    try {
        executeQuery("BEGIN");
        cout << endl << "Insertando datos en tabla: " << table << endl;

        size_t batchSize = tableConfig.contains("batchSize") ? tableConfig["batchSize"].get<size_t>() : 1000;
        
        string baseQuery = "INSERT INTO " + table + " (";

        for (const auto& column : tableConfig["columns"]) {
            baseQuery += column["name"].get<string>() + ", ";
        }
        baseQuery = baseQuery.substr(0, baseQuery.size() - 2) + ") VALUES ";

        string query = baseQuery;
        size_t count = 0;

        for (const auto& record : records) {
            cout << endl << "record: " << record.dump(4) << endl;
            query += "(";
            for (const auto& column : tableConfig["columns"]) {
                cout << endl << "Insertando datos en columna: " << column << endl;
                string value = "";
                if(column.contains("lookup"))
                {   
                    string naturalKeyValue = getValueFromJson(record, column["jsonPath"]);
                    vector<string> naturalKeyValueVector = Utility::splitStringByDelimiter(naturalKeyValue, " ");
                    map<string, string> naturalKeykeyValue = Utility::createNaturalKeyMap(column["lookup"]["naturalKey"], naturalKeyValueVector);
                    value = lookupId(column["lookup"]["table"], naturalKeykeyValue);
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
                //query += "'" + value + "', ";
                query += value + ", "; 

            }
            query = query.substr(0, query.size() - 2) + "), ";
            cout << endl << "query Antes de ejecutar" << query << endl;
            count++;

            if (count % batchSize == 0) {
                query = query.substr(0, query.size() - 2) + ";"; 
                executeQuery(query);
                query = baseQuery;
            }
        }

        if (count % batchSize != 0) {
            query = query.substr(0, query.size() - 2) + ";";
            //cout<< "MDB: Query: " <<endl << query << endl;
            executeQuery(query);
        }

        executeQuery("COMMIT");
    } catch (const exception& e) {
        executeQuery("ROLLBACK");
        throw runtime_error("Error en batchInsert: " + string(e.what()));
    }
}

void managerDb::processRelationships(const json& config, const json& relationshipsConfig, const json& data, const string& rootPath) {
    try {  
        cout << endl << "estoy aqui" << endl;
        for (const auto& rel : relationshipsConfig) {
            if (rel["type"] != "MANY_TO_MANY") continue;

            const string junctionTable = rel["junctionTable"].get<string>();
            const string fromTable = rel["fromTable"].get<string>();
            const string toTable = rel["toTable"].get<string>();
            const json& columnsConfig = rel["columns"];
            const string toDataPath = rel["dataPath"].get<string>();

            const json& fromRecords = data[rootPath][fromTable];
            vector<json> junctionRecords;
            //cout << endl << "junctionTable: "<< junctionTable << endl;
            //cout << endl << "fromTable: "<< fromTable << endl;
            //cout << endl << "toTable: "<< toTable << endl;
            //cout << endl << "toDataPath: "<< toDataPath << endl;
            //cout << endl << "columnsConfig: "<< columnsConfig.dump(4) << endl;
            for (const auto& fromRecord : fromRecords) {
                map<string, string> fromNaturalKey;

                //cout << endl << "fromRecord: "<< fromRecord << endl;

                for (const auto& key : config["tables"][fromTable]["naturalKey"]) {
                    fromNaturalKey[key.get<string>()] = getValueFromJson(fromRecord, key.get<string>());
                }
                string fromId = lookupId(fromTable, fromNaturalKey);

                cout << endl << "fromId: "<< fromId << endl;

                for (const auto& toData : fromRecord[toDataPath]) { 
                    map<string, string> toNaturalKey;
                    for (const auto& key : config["tables"][toTable]["naturalKey"]) {
                        toNaturalKey[key.get<string>()] = getValueFromJson(toData, key.get<string>());
                    }
                    string toId = lookupId(toTable, toNaturalKey);
                    cout << endl << "toId: "<< toId << endl;
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
                            cout << endl << "desde el bucle, colName:" << colName << endl; 
                            cout << endl << "desde el bucle, fromId:" << fromId << endl; 
                            cout << endl << "desde el bucle, fromTable:" << fromTable << endl; 
                            cout << endl << "desde el bucle, toId:" << toId << endl; 
                        } else {
                            string jsonPath = colConfig["jsonPath"].get<string>();
                            junctionRecord[colName] = getValueFromJson(toData, jsonPath); 
                            //junctionRecord[colName] = toData[colConfig["jsonPath"]];
                        }
                    }
                    junctionRecords.push_back(junctionRecord);
                }
            }
            cout <<endl <<"********************************************************" << endl<< endl;
            for(auto& juntionRecord: junctionRecords) {
                cout <<endl <<"juntionRecord" << juntionRecord.dump(4) << endl;
            }
            cout <<endl <<"rel" << rel.dump(4) << endl;
            batchInsert(junctionTable, junctionRecords, rel);
            
        }
    } catch (const exception& e) {
        throw runtime_error("Error procesando relaciones: " + string(e.what()));
    }
}

//destructor
managerDb::~managerDb() {
    if (conn) {
        PQfinish(conn);
     cout << "Conexión cerrada" << endl;
    }
}