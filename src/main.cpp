#include <iostream>
#include <string>
#include "../include/repository.hpp"
#include "../include/extraction.hpp"
#include "../include/transformation.hpp"

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

/*     cout << "Datos transformados";
    std::cout << "Transformed Data:\n" << transformedData.dump(4) << std::endl;
    */

    //conexion db
    try {
        Repository repo("localhost", "postgres", "postgres", "123456", 5454);
    } catch (const std::exception& e) {
        std::cerr << "Error al inicializar el repositorio: " << e.what() << std::endl;
        return 1;
    }
    
    string a; 
    cout << "Precione cualquier tecla para finalizar: ";  cin >> a;
    return 0;
}
