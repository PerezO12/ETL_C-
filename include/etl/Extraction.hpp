#ifndef EXTRACTION_HPP
#define EXTRACTION_HPP

#include "../common/common_types.hpp"

json extractData(const std::string& filePath);
std::vector<json> getJsonRecords(const json& data, const std::string& sourcePath, const std::string& rootPath = "");
std::string getValueFromJson(const json& record, const std::string& jsonPath);

#endif