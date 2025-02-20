#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class Transformation {
public:
    static json transformData(const json& extractedData);
};

#endif 
