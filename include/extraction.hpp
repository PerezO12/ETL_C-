#ifndef EXTRACTION_HPP
#define EXTRACTION_HPP

#include "common_types.hpp"

json extractData(const string& filePath);
vector<json> getJsonRecords(const json& data, const string& sourcePath, const string& rootPath = "");  
string getValueFromJson(const json& record, const string& jsonPath);

#endif // EXTRACTION_HPP
