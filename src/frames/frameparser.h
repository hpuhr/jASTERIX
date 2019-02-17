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
#include "itemparser.h"

namespace jASTERIX {

class FrameParser
{
public:
    FrameParser(const nlohmann::json& framing_definition, const nlohmann::json& data_block_definition,
                const std::map<unsigned int, nlohmann::json>& asterix_category_definitions, bool debug);

    // return number of parsed bytes
    size_t parseHeader (const char* data, size_t index, size_t size, nlohmann::json& target, bool debug);

    size_t parseFrames (const char* data, size_t index, size_t size, nlohmann::json& target, size_t num_frames,
                     bool debug);

    size_t decodeFrames (const char* data, nlohmann::json& target, bool debug);

    bool done() const;

private:
    //const nlohmann::json framing_definition_;
    //std::unique_ptr<ItemParser> framing_definition_;
    //const nlohmann::json data_block_definition_;
    //std::unique_ptr<ItemParser> data_block_definition_;
    //const std::map<unsigned int, nlohmann::json> asterix_category_definitions_;
    std::map<unsigned int, std::unique_ptr<ItemParser>> asterix_category_definitions_;

    //nlohmann::json header_items_;
    std::vector<std::unique_ptr<ItemParser>> header_items_;
    //nlohmann::json frame_items_;
    std::vector<std::unique_ptr<ItemParser>> frame_items_;
    std::string data_block_name_;
    std::vector<std::unique_ptr<ItemParser>> data_block_items_;

    bool done_ {false};

    // returns number of records
    size_t decodeFrame (const char* data, nlohmann::json& json_frame, bool debug);
};

}

#endif // FRAMEPARSER_H
