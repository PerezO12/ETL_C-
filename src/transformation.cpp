#include "../include/transformation.hpp"
#include <iostream>

json Transformation::transformData(const json& extractedData) {
    json transformedData = extractedData;

    /* if (transformedData.contains("age")) {
        transformedData["age"] = transformedData["age"].get<int>() + 1;  
    }
    if (transformedData.contains("languages") && transformedData["languages"].is_array()) {
        for (auto& language : transformedData["languages"]) {
            if (language == "German") {
                language = "Lenguaje add";
            }
        }
    } */

    return transformedData;
}
