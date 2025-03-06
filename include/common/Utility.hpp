#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "../common/common_types.hpp"

namespace Utility {
    std::string replaceSpacesWithUnderscore(const std::string& input);
    std::string join(const std::vector<std::string>& elements, const std::string& delimiter);
    std::vector<std::string> splitStringByDelimiter(const std::string& input, const std::string& delimiter);
    std::map<std::string, std::string> createNaturalKeyMap(const std::vector<std::string>& keys, const std::vector<std::string>& values);
}

#endif