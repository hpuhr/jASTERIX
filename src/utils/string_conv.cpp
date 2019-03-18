
#include <string>
#include <sstream>
#include <cstring>
#include <cassert>
#include <iomanip>

#include "string_conv.h"

int char2int(char input)
{
    if(input >= '0' && input <= '9')
        return input - '0';
    if(input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if(input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
// Returns size
size_t hex2bin(const char* src, char* target)
{
    size_t src_len = strlen(src);
    assert (src_len % 2 == 0);


    while(*src && src[1])
    {
        *(target++) = char2int(*src)*16 + char2int(src[1]);
        src += 2;
    }

    return src_len/2;
}

char getIcaoChar (unsigned char c)
{
   char ch;

   ch = '?';
   if (1 <= c && c <= 26)
   {
       ch = 'A' + (c - 1);
   }
   else if (c == 32)
   {
       ch = ' ';
   }
   else if (48 <= c && c <= 57)
   {
       ch = '0' + (c - 48);
   }

   return ch;
}

std::string binary2hex(const unsigned char* src, size_t length)
{
    std::stringstream ss;
    for(size_t i=0; i < length; ++i)
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)src[i];
    return ss.str();
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::string toString(const nlohmann::json& j)
{
    if (j.type() == nlohmann::json::value_t::string) {
        return j.get<std::string>();
    }

    return j.dump();
}


//std::vector<std::string> split(const std::string &s, char delim)
//{
//    std::vector<std::string> elems;
//    return split(s, delim, elems);
//}
