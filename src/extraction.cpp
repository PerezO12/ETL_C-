#include "../include/extraction.hpp"  

#include <fstream>
#include <algorithm>

json extractData(const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        cerr << "Error al abrir el archivo JSON" << endl;
        return json();
    }

    json j;
    file >> j;
    return j;
}

vector<json> getJsonRecords(const json& data, const string& sourcePath, const string& rootPath) {  
    vector<json> records;
    json currentData = rootPath.empty() ? data : data[rootPath];

    size_t pos = sourcePath.find('.');
    if (pos != string::npos) {
        string firstKey = sourcePath.substr(0, pos);
        string restKey = sourcePath.substr(pos + 1);

        if (currentData.contains(firstKey)) {
            records = getJsonRecords(currentData[firstKey], restKey, "");
        }
    } else {
        if (currentData.contains(sourcePath)) {
            for (const auto& rec : currentData[sourcePath]) {
                records.push_back(rec);
            }
        } else {
            cerr << "Error: Ruta no encontrada - " << sourcePath << endl;
        }
    }

    return records;
}

string getValueFromJson(const json& record, const string& jsonPath) {
    if (record.contains(jsonPath)) {
        string value = record[jsonPath].dump();
        if (value.size() > 1 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        return value;
    }
    return "";
}
