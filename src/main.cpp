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

        managerDb db (config["database"]);
        //creamos tablas y relaciones si no existen
        db.createTables(config["tables"], config["globalOptions"]["loadOrder"]);
        db.createRelationships(config["relationships"]);

        //3. truncar tablas
        /* if (config["globalOptions"]["truncateBeforeLoad"]) {
            db.truncateTables(config["globalOptions"]["loadOrder"]);
        } */

        // 4. procesar tablas en el orden definido en el json
        for (const auto& tableName : config["globalOptions"]["loadOrder"]) {
            // aplicat transformacioens definidas
            //transformation::applyTransformations(data[tableName], config["transformations"]);
            //cout << endl << "tablename: "<<tableName <<endl ; //borrar luego
            // 4b. Insertar datos en batch  
            const vector<json> records = getJsonRecords(data, tableName, config["dataSource"]["rootPath"]);
            //borrar luego
            /* for(auto& record : records) {
                cout << endl << "records: " << record.dump(4) << endl;
            } */
            db.batchInsert(tableName, records, config["tables"][tableName]);
        }
          // Procesar relaciones Many-to-Many
        db.processRelationships(
            config, 
            config["relationships"], 
            data, 
            config["dataSource"]["rootPath"].get<string>()
        );

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        waitForUserInput(); //para q no se cierre
        return 1;
    } 
    waitForUserInput(); //para q no se cierre
    return 0;
}
