#include "../../include/etl/transformation.hpp" 


//applica las transformaciones defidindas en conf
//todo:cambiar esto
string applyTransformation(const string& value, const string& transformName, const json& transformationsConfig) {
    if (!transformationsConfig.contains(transformName)) {
        throw runtime_error("Transformación no definida: " + transformName);
    }

    const json& transform = transformationsConfig[transformName];
    string logic = transform["logic"];

    if (logic == "split_by_space") {
        size_t spacePos = value.find(' ');
        if (spacePos == string::npos) {
            throw runtime_error("No se puede dividir el nombre completo: " + value);
        }
        return value.substr(0, spacePos) + "_" + value.substr(spacePos + 1);
    } else if (logic == "trim_uppercase") {
        string trimmed = value;
        trimmed.erase(trimmed.begin(), find_if(trimmed.begin(), trimmed.end(), [](int ch) { return !isspace(ch); }));
        trimmed.erase(find_if(trimmed.rbegin(), trimmed.rend(), [](int ch) { return !isspace(ch); }).base(), trimmed.end());
        std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
        return trimmed;
    }
    return value;
}

/* string join(const vector<string>& elements, const string& separator) {
    ostringstream result;
    for (size_t i = 0; i < elements.size(); ++i) {
        result << elements[i];
        if (i < elements.size() - 1) result << separator;
    }
    return result.str();
}
 */