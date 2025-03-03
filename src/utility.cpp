#include "../include/utility.hpp"


namespace Utility {
    string replaceSpacesWithUnderscore(const string& input) {
        string result = input;
        for (auto& c : result) { 
            if (c == ' ') {
                c = '_'; 
            }
        }
        return result;
    }
    int convertIdToInt(const std::string& id) {
        try {
            return std::stoi(id);
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("El ID '" + id + "' no es un número válido.");
        } catch (const std::out_of_range& e) {
            throw std::runtime_error("El ID '" + id + "' está fuera del rango de int.");
        }
    }
    string join(const vector<string>& elements, const string& delimiter) {
        string result;
        for (size_t i = 0; i < elements.size(); ++i) {
            result += elements[i];
            if (i < elements.size() - 1) result += delimiter;
        }
        return result;
    }
    vector<string> splitStringByDelimiter(const string& input, const string& delimiter) {
        if (delimiter.empty()) {
            throw invalid_argument("El delimitador no puede estar vacío.");
        }
    
        vector<string> result;
        size_t start = 0;
        size_t end = input.find(delimiter);
    
        if (end == string::npos) {
            return {input};
        }
    
        while (end != string::npos) {
            string token = input.substr(start, end - start);
            if (!token.empty()) { 
                result.push_back(token);
            }
            start = end + delimiter.length();
            end = input.find(delimiter, start);
        }
    
        string lastToken = input.substr(start);
        if (!lastToken.empty()) {
            result.push_back(lastToken);
        }
    
        return result;
    }
    
    //todo: cambiar esto apra que no sea tan estricto, se puedan ajustar desde el json de configuracion;
    json transformJson(const string& valor, const vector<string>& claves) {
        json claveValor;
        for (const auto& clave : claves) {
            claveValor[clave] = valor;
        }
        return claveValor;
    }
    

    map<string, string> createNaturalKeyMap(const vector<string>& keys, const vector<string>& values) {
        if (keys.size() != values.size()) {
            throw invalid_argument("El número de claves no coincide con el número de valores.");
        }
        
        map<string, string> naturalKey;
        for (size_t i = 0; i < keys.size(); i++) {
            naturalKey[keys[i]] = values[i];
        }
        
        return naturalKey;
    }
}