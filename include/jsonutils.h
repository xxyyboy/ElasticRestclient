#ifndef _JSON_UTILS_H_
#define _JSON_UTILS_H_

#include<string>

std::string escapeJSON(const std::string& input);
std::string unescapeJSON(const std::string& input);

#endif
