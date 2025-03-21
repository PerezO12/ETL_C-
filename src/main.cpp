#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <limits>
#include <chrono> //pra medir el tiempo
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

        auto inicio = chrono::high_resolution_clock::now();//para medir el tiempo que se demora leyendo los json
        //**************************************************************************************/
        json data = extractData(config["dataSource"]["filePath"]);
        //repitiendo para simular mas cantidad de volumen d datos
        int iteraciones = 100000;
        /* for(int i = 0; i < iteraciones; i++) {
            json data1 = extractData(config["dataSource"]["filePath"]);
            if(i % (iteraciones/100) == 0){
                cout<<" "<<i/(iteraciones/100) <<"%";
            }
        } */
        cout<<endl;
        if (data.is_null())  throw runtime_error("El archivo de datos JSON no se pudo cargar.");

        //**************************************************************************************/
        auto fin = chrono::high_resolution_clock::now(); 
        auto duracion1 = chrono::duration_cast<chrono::microseconds>(fin - inicio);
        
        //tiempo con la otra libreria
        inicio = chrono::high_resolution_clock::now();
        std::string path = config["dataSource"]["filePath"];
        auto data2 = extractDataSimDjson(path);


        //repitiendo para simular mas cantidad de volumen d datos
        /* for(int i = 0; i < iteraciones; i++) {
            auto data1 = extractDataSimDjson(config["dataSource"]["filePath"]);
            if(i % (iteraciones/100) == 0){
                cout<<" "<<i/(iteraciones/100) <<"%";
            }
        } */
        
        fin = chrono::high_resolution_clock::now(); 
        auto duracion2 = chrono::duration_cast<chrono::microseconds>(fin - inicio);
        cout << endl <<endl << endl << endl;
        cout << "\nTiempo que tardo en leer los json("<< iteraciones <<"iteraciones) con nlohmann_json: " << duracion1.count() << " microsegundos... " << duracion1.count()/1000000 <<"segundos"<< endl;
        cout << "\nTiempo que tardo en leer los json("<< iteraciones <<"iteraciones) con simdjson: " << duracion2.count() << " microsegundos... "<< duracion2.count()/1000000 <<"segundos" << endl;
        cout << endl <<endl << endl << endl;
        //**************************************************************************************/








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
        /* if (config["globalOptions"]["truncateBeforeLoad"]) {
            schemaManager.truncateTables(config["globalOptions"]["loadOrder"]);
        } */

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
