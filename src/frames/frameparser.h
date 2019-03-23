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

#ifndef FRAMEPARSER_H
#define FRAMEPARSER_H

#include "json.hpp"
//#include "itemparserbase.h"

namespace jASTERIX {

class ASTERIXParser;
class ItemParserBase;

class FrameParser
{
public:
    FrameParser(const nlohmann::json& framing_definition, ASTERIXParser& asterix_parser, bool debug);

    // return number of parsed bytes
    size_t parseHeader (const char* data, size_t index, size_t size, nlohmann::json& target, bool debug);

    size_t parseFrames (const char* data, size_t index, size_t size, nlohmann::json& target, bool debug);

    size_t decodeFrames (const char* data, nlohmann::json& target, bool debug);

    bool done() const;

private:
    ASTERIXParser& asterix_parser_;

    std::vector<std::unique_ptr<ItemParserBase>> header_items_;
    std::vector<std::unique_ptr<ItemParserBase>> frame_items_;

    size_t sum_frames_cnt_ {0};
    bool done_ {false};

    // returns number of records
    size_t decodeFrame (const char* data, nlohmann::json& json_frame, bool debug);
};

}

#endif // FRAMEPARSER_H
