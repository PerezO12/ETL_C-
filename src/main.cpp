#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "../include/extraction.hpp"
#include "../include/transformation.hpp"
#include "../include/manager_db.hpp"

using namespace std;

int main() {
    string filePath;
    cout << "Ingrese la ruta del archivo JSON (deje en blanco para usar la predeterminada): ";
    getline(cin, filePath);
    
    if (filePath.empty()) {
        filePath = "../DataInput/data.json";
    }

    //EXTRACCIÓN
    json data = extractData(filePath);

    if (data.is_null()) {
        cerr << "No se pudieron obtener datos del archivo. Verifique la ruta.\n";
        return 1;
    }

    //TRANSFORMACIÓN Y MAPEADO
    map<string, vector<string>> tablas;
    mapearDatosDinamicamente(data, tablas);

    cout << "\n **Datos mapeados:**\n";
    imprimirTablas(tablas);

    cout << "\n **Datos sin mapear:**\n" << data.dump(4) << "\n";

    //MENSAJE FINAL
    cout << "\nPresione Enter para finalizar...";
    cin.ignore();
    cin.get();

    return 0;
}


//conexion db
    //todo: sacar de aqui
    /*
    try
    {
        manager_db db("localhost", "postgres", "postgres", "123456", "5454");

        db.create_tables(R"(
                CREATE TABLE IF NOT EXISTS personas (
                    id SERIAL PRIMARY KEY,
                    nombre VARCHAR(100),
                    apellido VARCHAR(100),
                    edad INT,
                    ciudad VARCHAR(100)
                );
            )"
        );

        db.execute_query("INSERT INTO personas (nombre, apellido, edad) VALUES ('Carlos', 'Gómez', 21)");
        PGresult* res = db.execute_select_query("SELECT * FROM personas");
         if (res != nullptr) {
            // Procesar los resultados de la consulta
            int rows = PQntuples(res);
            for (int i = 0; i < rows; ++i) {
                cout << "Nombre: " << PQgetvalue(res, i, 0) << ", Apellido: " << PQgetvalue(res, i, 1) << endl;
            }
            PQclear(res);
        } 
    }
    catch(const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }*/