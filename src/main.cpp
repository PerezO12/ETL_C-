#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <variant>
#include "../include/extraction.hpp"
#include "../include/transformation.hpp"
#include "../include/manager_db.hpp"

using namespace std;


using IdType = std::variant<int, string>;

json leerConfiguracion(const string& configFilePath) {
    ifstream configFile(configFilePath);
    if (!configFile) {
        throw runtime_error("No se pudo abrir el archivo de configuración");
    }
    json config;
    configFile >> config;
    return config;
}
void insertarMapearEjecutarQuerys(managerDb& db, const json& data, const json& config, map<string, map<string, int>>& idCache) {
    const string rootPath = config["dataSource"].value("rootPath", "");
    const json& transformations = config["transformations"];
    cout<<"entre: " << endl;
    for (const string& tableName : config["globalOptions"]["loadOrder"]) {
        const json& tableConfig = config["tables"][tableName];
        vector<json> records = getJsonRecords(data, tableConfig["sourcePath"], rootPath);
        vector<string> queries = jsonToSqlInsert(tableName, records, data, config, idCache);

        for (size_t i = 0; i < queries.size(); ++i) {
            cout << queries[i] << "\n";
            int id = stoi(db.executeQueryReturningId(queries[i]));
            string naturalKey;
            if (tableConfig.contains("naturalKey")) {
                vector<string> keyParts;
                for (const auto& field : tableConfig["naturalKey"]) {
                    string rawValue = records[i][field].get<string>();

                    /* if (tableConfig["transforms"].contains(field)) {
                        string transformName = tableConfig["transforms"][field];
                        rawValue = applyTransformation(rawValue, transformName, transformations);
                    } */

                    keyParts.push_back(rawValue);
                }
                naturalKey = join(keyParts, "_"); 
                //cout << "\nnaturalKey" << naturalKey << endl;
            } else {
                throw runtime_error("Falta 'naturalKey' en la configuración de la tabla: " + tableName);
            }

            idCache[tableName][naturalKey] = id;
        }
    }
}



int main() {
    try {
        //CONFIGURACION

        //cargar configuracion
        json config = leerConfiguracion("../config/config.json");
        //ruta archivo de datos 
        string dataFilePath = config["dataSource"]["filePath"];
        /*************/
        //EXTRACCIÓN
        json data = extractData(dataFilePath);
        if (data.is_null())  throw runtime_error("El archivo de datos JSON no se pudo cargar.");
        cout << "*************************************************" << endl;

        //TRANSFORMACIÓN Y MAPEADO
        map<string, map<string, int>> idCache;
        
        //map<string, vector<string>> insertQueries = jsonToSqlInsertt2(data, config, idCache);

        //DB
        //conectar base de datos
        managerDb db(
            config["database"]["host"],
            config["database"]["dbname"],
            config["database"]["user"],
            config["database"]["password"],
            config["database"]["port"]
        );
        
        // Opcional: Crear las tablas si no existen (aquí podrías llamar a create_tables())
        db.createTables(config);
        db.createRelationships(config);
        
        
        //ejecucion de las consultas
        insertarMapearEjecutarQuerys(db, data, config, idCache);

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        //MENSAJE FINAL para que no se cierre
        cout << "\nPresione Enter para finalizar..."; cin.ignore(); cin.get();
        return 1;
    } 
    //MENSAJE FINAL para que no se cierre
    cout << "\nPresione Enter para finalizar..."; cin.ignore(); cin.get();
    return 0;
}


