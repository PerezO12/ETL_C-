#ifndef EXTRACTION_HPP
#define EXTRACTION_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <vector>
#include <iostream>  // Necesario para `std::cout`

using json = nlohmann::json;

// Función para cargar datos JSON desde un archivo
json extractData(const std::string& filePath);

// Mapea dinámicamente los datos del JSON a tablas relacionales simuladas
void mapearDatosDinamicamente(const json& j, std::map<std::string, std::vector<std::string>>& tablas, const std::string& currentTable = "");

// Imprime las tablas extraídas
void imprimirTablas(const std::map<std::string, std::vector<std::string>>& tablas);

#endif // EXTRACTION_HPP
