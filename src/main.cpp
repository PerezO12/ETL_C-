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
        map<string, vector<string>> insertQueries = jsonToSqlMapper(data, config);
        


        //DB
        //conectar base de datos
        manager_db db(
            config["database"]["host"],
            config["database"]["dbname"],
            config["database"]["user"],
            config["database"]["password"],
            config["database"]["port"]
        );
        
        // Opcional: Crear las tablas si no existen (aquí podrías llamar a create_tables())
        // db.create_tables(tablasDefinidas); // tablasDefinidas puede ser generado durante la transformación

        // Ejecutar las consultas de inserción para cada tabla
        
        for (const auto& tablePair : insertQueries) {
            cout << "Insertando datos en la tabla: " << tablePair.first << endl;
            
            for (const auto& query : tablePair.second) {
                cout << 1 << "\n";
                cout << query << "\n";
                db.execute_query(query);
            }
        }
        

        //MENSAJE FINAL para que no se cierre
        cout << "\nPresione Enter para finalizar...";
        cin.ignore();
        cin.get();
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        cout << "\nPresione Enter para finalizar...";
        cin.ignore();
        cin.get();
        return 1;
    }
    return 0;
}
