/*
 * This file is part of jASTERIX.
 *
 * jASTERIX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jASTERIX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with jASTERIX.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <vector>

#include "json.hpp"

extern int char2int(char input);
extern size_t hex2bin(const char* src, char* target);
extern char getIcaoChar(unsigned char c);
extern unsigned char getIcaoCode(char c);
extern std::string binary2hex(const unsigned char* src, unsigned int length);
// hexdump of [index, index+length) clamped to the buffer end total_size;
// appends "..." if clamped. For dumping lengths taken from (possibly corrupt
// or truncated) input data, where reading the full declared length could run
// past the end of the mapped file buffer.
extern std::string binary2hex_bounded(const unsigned char* data, size_t index, size_t length,
                                      size_t total_size);
extern std::string bin2hex(const char* src, size_t length);

extern std::vector<std::string>& split(const std::string& s, char delim,
                                       std::vector<std::string>& elems);
// extern std::vector<std::string> split(const std::string &s, char delim);

extern std::string toString(const nlohmann::json& j);

extern bool isASCII(const std::string& s);


