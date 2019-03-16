#ifndef STRING_CONV_H
#define STRING_CONV_H

#include <vector>
#include <string>

#include "json.hpp"

extern int char2int(char input);
extern size_t hex2bin(const char* src, char* target);
extern char getIcaoChar (unsigned char c);
extern std::string binary2hex(const unsigned char* src, size_t length);

extern std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
//extern std::vector<std::string> split(const std::string &s, char delim);

extern std::string toString(const nlohmann::json& j);


#endif // STRING_CONV_H
