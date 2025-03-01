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
#include <iostream>
#include <limits>

void waitForUserInput() {
    cout << "\nPresione Enter para finalizar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    try {
        //cargar conf y datos.
        json config = leerConfiguracion("../config/config.json");
        json data = extractData(config["dataSource"]["filePath"]);
        if (data.is_null())  throw runtime_error("El archivo de datos JSON no se pudo cargar.");
        cout << "*************************************************" << endl;

        //TRANSFORMACIÓN

        /* managerDb db(
            config["database"]["host"],
            config["database"]["dbname"],
            config["database"]["user"],
            config["database"]["password"],
            config["database"]["port"]
        ); */
        managerDb db (config["database"]);
        //creamos tablas y relaciones si no existen
        db.createTables(config["tables"], config["globalOptions"]["loadOrder"]);
        db.createRelationships(config["relationships"]);

        //3. truncar tablas
        /* if (config["globalOptions"]["truncateBeforeLoad"]) {
            db.truncateTables(config["globalOptions"]["loadOrder"]);
        } */

        // 4. procesar tablas en el orden definido en el json
        /* for (const auto& tableName : config["globalOptions"]["loadOrder"]) {
            // aplicat transformacioens definidas
            //transformation::applyTransformations(data[tableName], config["transformations"]);

            // 4b. Insertar datos en batch
            db.batchInsert(tableName, data, config["tables"][tableName]);
        } */

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        waitForUserInput(); //para q no se cierre
        return 1;
    } 
    waitForUserInput(); //para q no se cierre
    return 0;
}


/* void insertarMapearEjecutarQuerys(managerDb& db, const json& data, const json& config) {
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
                    keyParts.push_back(rawValue);
                }
                naturalKey = join(keyParts, "_"); 
                cout << "\nnaturalKey" << naturalKey << endl;
                cout << endl << "main:" << endl;
                cout << endl << "\nvalorFK naturalKey: " << naturalKey << endl;
                cout << endl << "\nrefTable tableName: " << tableName << endl;
            } else {
                throw runtime_error("Falta 'naturalKey' en la configuración de la tabla: " + tableName);
            }

            idCache[tableName][naturalKey] = id;
        }
    }
} */