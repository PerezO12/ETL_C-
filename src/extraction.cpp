#include "../include/extraction.hpp"

#include <fstream>  // Necesario para `std::ifstream`
#include <algorithm> // Necesario para `std::find`

using namespace std;

//funcion auxiliar
string getJsonType(const json& j) {
    if (j.is_number_integer())
        return "INTEGER";
    else if (j.is_number_float())
        return "REAL";
    else if (j.is_boolean())
        return "BOOLEAN";
    else if (j.is_string())
        return "TEXT";
    else if (j.is_array() || j.is_object())
        return "JSONB";
    return "TEXT";
}

json extractData(const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        cerr << "Error al abrir el archivo JSON" << endl;
        return json();
    }

    json j;
    file >> j;
    return j;
}

void mapearDatosDinamicamente(const json& j, map<string, vector<string>>& tablas, const string& currentTable) {
    if (!j.is_object()) return;

    if (!currentTable.empty() && tablas.find(currentTable) == tablas.end()) {
        vector<string> atributos;
        for (const auto& attr : j.items()) {
            // Si el valor es un array de objetos, lo consideramos una relación y no agregamos el tipo aquí.
            if (attr.value().is_array() && !attr.value().empty() && attr.value()[0].is_object()) {
                // Se marca como relación (por ejemplo, "alumnos" se mapea como "alumnos_relation")
                atributos.push_back(attr.key() + "_llll_relation");
            } else {
                atributos.push_back(attr.key() + "_llll_" + getJsonType(attr.value()));
            }
        }
        tablas[currentTable] = atributos;
    }

    for (const auto& element : j.items()) {
        const string& key = element.key();
        const json& value = element.value();

        if (value.is_array()) {
            if (!value.empty() && value[0].is_object()) {
                // Si no existe la tabla en el map, extraemos los atributos del primer objeto.
                if (tablas.find(key) == tablas.end()) {
                    vector<string> atributos;
                    for (const auto& attr : value[0].items()) {
                        atributos.push_back(attr.key()  + "_llll_" + getJsonType(attr.value()));
                    }
                    tablas[key] = atributos;
                }
                // Procesamos cada objeto del array, usando "key" como currentTable.
                for (const auto& item : value) {
                    mapearDatosDinamicamente(item, tablas, key);
                }
            } else if (!currentTable.empty()) {
                // Si el array no tiene objetos, añadir la clave como atributo de la tabla actual
                if (find(tablas[currentTable].begin(), tablas[currentTable].end(), key) == tablas[currentTable].end()) {
                    tablas[currentTable].push_back(key);
                }
            }
        }
        else if (value.is_object()) {
            mapearDatosDinamicamente(value, tablas, key);
        }
    }
}

//todo: solo desarrollo, comentar luego la llamada
void imprimirTablas(const map<string, vector<string>>& tablas) {
    for (const auto& par : tablas) {
        cout << "Tabla: " << par.first << "\nAtributos: ";
        for (const auto& attr : par.second) {
            cout << attr << " ";
        }
        cout << "\n\n";
    }
}


