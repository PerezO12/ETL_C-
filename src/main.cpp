#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <limits>
//config
#include "../include/services/ConfigLoader.hpp"
//etl
#include "../include/etl/extraction.hpp"
#include "../include/etl/transformation.hpp"
//database
#include "../include/database/Connection.hpp"
#include "../include/database/QueryExecutor.hpp"
#include "../include/database/SchemaManager.hpp"
#include "../include/database/InsertValues.hpp"



using namespace std;


void waitForUserInput() {
    cout << "\nPresione Enter para finalizar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main() {
    try {
        //cargar conf,datos y variables a utilizar .
        json config = leerConfiguracion("../config/config.json");
        json data = extractData(config["dataSource"]["filePath"]);
        if (data.is_null())  throw runtime_error("El archivo de datos JSON no se pudo cargar.");

        //Coneccion a la base de datos
        const json& dbConfig = config["database"];

        //creacion de los componentes a utilzar
        Connection conn(dbConfig);
        QueryExecutor queryExecutor(conn.getRawConnection());
        SchemaManager schemaManager(queryExecutor);
        InsertValues insertValues(queryExecutor);

        //gestiónn del esquema d las tablas y relaciones

        schemaManager.createTables(config["tables"], config["globalOptions"]["loadOrder"]);
        schemaManager.createRelationships(config["relationships"]);
        
        // Truncar tablas si est aactivo
        //todo:falta probar
        if (config["globalOptions"]["truncateBeforeLoad"]) {
            schemaManager.truncateTables(config["globalOptions"]["loadOrder"]);
        }

        // procesar datos
        for (const auto& tableName : config["globalOptions"]["loadOrder"]) {

            const vector<json> records = getJsonRecords(data, tableName, config["dataSource"]["rootPath"]);
            // Aplicar transformaciones
            //todo: falta
            /* for (auto& record : records) {
                applyTransformations(record, config["transformations"]);
            } */
            insertValues.batchInsert(tableName, records, config["tables"][tableName]);
        }
        // Procesar relaciones Many-to-Many
        insertValues.processRelationships(
            config,
            config["relationships"],
            data,
            config["dataSource"]["rootPath"].get<string>()
        );
        cout << "\nProceso ETL completado con éxito\n";

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        waitForUserInput();
        return 1;
    } 
    waitForUserInput();
    return 0;
}
