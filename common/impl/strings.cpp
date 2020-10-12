#include "strings.hpp"

namespace common {

/// SO: https://stackoverflow.com/a/3418285/5799035
std::string replaceAll(std::string str, const std::string& from,
                       const std::string& to) {
    if (from.empty())
        return str;

    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like
                                  // replacing 'x' with 'yx'
    }

    return str;
}

} // namespace common
