#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP


#include "extraction.hpp"
#include "common_types.hpp"
#include "utility.hpp"


//devuelve un mapa donde la clave es el nombre de la tabla y el valor es un vector de sentencias INSERT.
//map<string, vector<string>> jsonToSqlInsertt2(const json& data, const json& config, map<string, map<string, IdType>>& idCache);


//string applyTransformation(const string& value, const string& transformName, const json& transformationsConfig);
//vector<string> jsonToSqlInsert(const string& tableName, const vector<json>& recordsTable, const json& data, const json& config, map<string, map<string, int>>& idCache);
//string join(const vector<string>& elements, const string& separator);
void applyTransformations(json& record, const json& transformations);
#endif // TRANSFORMATION_HPP
