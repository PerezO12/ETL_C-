#ifndef EXTRACTION_HPP
#define EXTRACTION_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <vector>
#include <iostream> 

using namespace std;
using json = nlohmann::json;

json extractData(const string& filePath);

//void mapearDatosDinamicamente(const json& j, map<string, vector<string>>& tablas, RelationsMap& relations, const string& currentTable = "");
//void imprimirTablas(const map<string, vector<string>>& tablas);
//void imprimirRelaciones(const RelationsMap& relations);


#endif // EXTRACTION_HPP
