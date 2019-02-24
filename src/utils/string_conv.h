#ifndef STRING_CONV_H
#define STRING_CONV_H

extern int char2int(char input);
extern size_t hex2bin(const char* src, char* target);
extern char getIcaoChar (unsigned char c);
extern std::string binary2hex(const unsigned char* src, size_t length);

#endif // STRING_CONV_H
