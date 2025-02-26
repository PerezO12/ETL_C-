#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

//devuelve un mapa donde la clave es el nombre de la tabla y el valor es un vector de sentencias INSERT.
map<string, vector<string>> jsonToSqlInsert(const json& data, const json& config);

vector<json> getJsonRecords(const json& data, const string& sourcePath);
string getValueFromJson(const json& record, const string& jsonPath);

vector<string> generateRelationshipInsertQueries(const json& data, const json& config,
    const unordered_map<string, unordered_map<string, int>>& tableIdMapping);

#endif // TRANSFORMATION_HPP
