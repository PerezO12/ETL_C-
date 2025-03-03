
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "common_types.hpp"

namespace Utility {
    string replaceSpacesWithUnderscore(const string& input);
    int convertIdToInt(const std::string& id);
    string join(const vector<string>& elements, const string& delimiter);
    vector<string> splitStringByDelimiter(const string& input, const string& delimiter);
    map<string, string> createNaturalKeyMap(const vector<string>& keys, const vector<string>& values);
    //todo: implementar luego
    //void log(const string& message, const string& level = "INFO");
    //void handleError(const string& message, bool fatal = false);
}

#endif