// extraction.cpp

#include "../include/extraction.hpp"
#include <fstream>
#include <iostream>

json extractData(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file) {
        std::cerr << "Error al abrir el archivo JSON" << std::endl;
        return json();
    }

    json j;
    file >> j;
    return j;
}
