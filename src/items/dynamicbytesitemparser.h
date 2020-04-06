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

#ifndef DYNAMICBYTESITEMPARSER_H
#define DYNAMICBYTESITEMPARSER_H

#include "itemparserbase.h"

namespace jASTERIX
{
// calculates the index and length based on other decoded variable
class DynamicBytesItemParser : public ItemParserBase
{
  public:
    DynamicBytesItemParser(const nlohmann::json& item_definition);
    virtual ~DynamicBytesItemParser() {}

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, nlohmann::json& target,
                             bool debug) override;

  protected:
    std::string length_variable_name_;
    bool substract_previous_{false};

    bool has_additative_factor_{false};
    int additative_factor_;
};

}  // namespace jASTERIX

#endif  // DYNAMICBYTESITEMPARSER_H
