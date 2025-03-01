#include "../include/utils.hpp"


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
}