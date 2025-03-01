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
}