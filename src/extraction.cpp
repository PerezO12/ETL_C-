#include "../include/extraction.hpp"

#include <fstream>
#include <algorithm>

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


