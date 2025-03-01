
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "common_types.hpp"

namespace Utility {
    string replaceSpacesWithUnderscore(const string& input);
    int convertIdToInt(const std::string& id);
    string join(const vector<string>& elements, const string& delimiter);
    //todo: implementar luego
    //void log(const string& message, const string& level = "INFO");
    //void handleError(const string& message, bool fatal = false);
}

#endif