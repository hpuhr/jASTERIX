/*
 * This file is part of ATSDB.
 *
 * ATSDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ATSDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with ATSDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ASTERIXPARSER_H
#define ASTERIXPARSER_H

#include "json.hpp"
#include "category.h"
#include "edition.h"

#include "mapping.h"

#include <tuple>

namespace jASTERIX {

class ItemParserBase;

class ASTERIXParser
{
public:
    ASTERIXParser(const nlohmann::json& data_block_definition,
                  std::map<unsigned int, Category>& category_definitions, bool debug);

    std::tuple<size_t, size_t, bool> findDataBlocks (const char* data, size_t index, size_t length,
                                                     nlohmann::json& target,bool debug);
    // parsed bytes, num data blocks, done flag

    size_t decodeDataBlocks (const char* data, nlohmann::json& data_blocks, bool debug);
    size_t decodeDataBlock (const char* data, nlohmann::json& data_block, bool debug);
//    size_t decodeDataBlock (const char* data, unsigned int cat, size_t index, size_t size, nlohmann::json& target,
//                            bool debug);

private:
    std::string data_block_name_;
    std::vector<std::unique_ptr<ItemParserBase>> data_block_items_;
    std::map<unsigned int, std::shared_ptr<Record>> records_;
    std::map<unsigned int, std::shared_ptr<ReservedExpansionField>> refs_;
    std::map<unsigned int, std::shared_ptr<Mapping>> mappings_;
};

}
#endif // ASTERIXPARSER_H
