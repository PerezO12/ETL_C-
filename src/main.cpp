#include <iostream>
#include <string>
#include "../include/extraction.hpp"
#include "../include/transformation.hpp"
#include "../include/manager_db.hpp"

using namespace std;

int main() {
    // Ruta completa del archivo JSON
    string filePath = "";
    cout << "La ruta: "; getline(cin, filePath);
    (filePath == "") ? filePath = "../DataInput/data.json" : filePath;

    //EXTACCION
    json data = extractData(filePath);

    if (data.is_null()) {
        cerr << "No se pudieron obtener datos del archivo" << endl;
        return 1;
    }

    //TRANSFORMACION
    json transformedData = Transformation::transformData(data);

    // Acceder a los valores del JSON
    string name = data["name"];
    int age = data["age"];
    string city = data["city"];

    cout << "Name: " << name << "\n";
    cout << "Age: " << age << "\n";
    cout << "City: " << city << "\n";


    //conexion db
    //todo: sacar de aqui
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

/*        db.execute_query("INSERT INTO personas (nombre, apellido, edad) VALUES ('Carlos', 'Gómez', 21)");
        PGresult* res = db.execute_select_query("SELECT * FROM personas");
         if (res != nullptr) {
            // Procesar los resultados de la consulta
            int rows = PQntuples(res);
            for (int i = 0; i < rows; ++i) {
                cout << "Nombre: " << PQgetvalue(res, i, 0) << ", Apellido: " << PQgetvalue(res, i, 1) << endl;
            }
            PQclear(res);
        } */
    }
    catch(const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
    
    
    //const char* conninfo = "host=localhost dbname=postgres user=postgres password=123456 port=5454";
/*     PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection to database failed: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        string a; 
        cout << "Fallo: ";  cin >> a;
        return 1;
    };
    cout << "Connectado a la DB" << endl;
    PQfinish(conn); */


    //mensaje de finalidad, borrar luego
    string a; 
    cout << "Precione cualquier tecla para finalizar: ";  cin >> a;
    return 0;
}
