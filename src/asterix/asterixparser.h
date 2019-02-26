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

#ifndef ASTERIXPARSER_H
#define ASTERIXPARSER_H

#include "json.hpp"
//#include "itemparser.h"
#include "edition.h"

namespace jASTERIX {

class ItemParserBase;

class ASTERIXParser
{
public:
    ASTERIXParser(const nlohmann::json& data_block_definition,
                  const std::map<unsigned int, std::shared_ptr<Edition>>& asterix_category_definitions, bool debug);

    size_t decodeDataBlock (const char* data, size_t index, size_t length, nlohmann::json& target, bool debug);

private:
    //const std::map<unsigned int, std::shared_ptr<Edition>>& asterix_category_definitions_;

    std::string data_block_name_;
    std::vector<std::unique_ptr<ItemParserBase>> data_block_items_;
    std::map<unsigned int, std::shared_ptr<Record>> records_;
};

}
#endif // ASTERIXPARSER_H
