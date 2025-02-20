#ifndef EXTRACTION_HPP
#define EXTRACTION_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

// func cargar datos json
json extractData(const std::string& filePath);

#endif
