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

#ifndef FIXEDBITFIELDITEMPARSER_H
#define FIXEDBITFIELDITEMPARSER_H

#include <memory>
#include <vector>

#include "itemparserbase.h"
#include "json.hpp"

namespace jASTERIX
{
class FixedBitFieldItemParser : public ItemParserBase
{
  public:
    FixedBitFieldItemParser(const nlohmann::json& item_definition);
    virtual ~FixedBitFieldItemParser() {}

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, nlohmann::json& target,
                             bool debug) override;

  protected:
    bool optional_{false};
    std::string optional_variable_name_;
    std::vector<std::string> optional_variable_name_parts_;
    nlohmann::json optional_variable_value_;
    size_t length_;  // byte length
    std::vector<std::unique_ptr<ItemParserBase>> items_;
};

}  // namespace jASTERIX

#endif  // FIXEDBITFIELDITEMPARSER_H
