#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "../include/extraction.hpp"
#include "../include/transformation.hpp"
#include "../include/manager_db.hpp"

using namespace std;


json leerConfiguracion(const string& configFilePath) {
    ifstream configFile(configFilePath);
    if (!configFile) {
        throw runtime_error("No se pudo abrir el archivo de configuración");
    }
    json config;
    configFile >> config;
    return config;
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
        map<string, vector<string>> insertQueries = jsonToSqlInsert(data, config);
        

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

        vector<string> ids;
        //ejecucion de las consultas
        for (const auto& tablePair : insertQueries) {
            cout << "Insertando datos en la tabla: " << tablePair.first << endl;
            
            for (const auto& query : tablePair.second) {
                cout << query << "\n";
                ;
                ids.push_back(db.executeQueryReturningId(query));
            }
        }
        for (const auto& id : ids) {
            cout << "id: " << id <<  endl;
        }

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
