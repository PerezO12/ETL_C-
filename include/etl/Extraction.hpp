#ifndef EXTRACTION_HPP
#define EXTRACTION_HPP

#include "../common/common_types.hpp"
#include <simdjson.h>

json extractData(const std::string& filePath);
std::vector<json> getJsonRecords(const json& data, const std::string& sourcePath, const std::string& rootPath = "");
std::string getValueFromJson(const json& record, const std::string& jsonPath);

//simdjson
simdjson::dom::element extractDataSimDjson(const std::string& filePath);

#endif