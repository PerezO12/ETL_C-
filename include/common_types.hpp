#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <variant>
#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream> 

using json = nlohmann::json;
using IdType = std::variant<int, std::string>;
using namespace std;

#endif
