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

#ifndef OPTIONALITEMPARSER_H
#define OPTIONALITEMPARSER_H

#include "itemparserbase.h"

namespace jASTERIX
{
class OptionalItemParser : public ItemParserBase
{
  public:
    OptionalItemParser(const nlohmann::json& item_definition, const std::string& long_name_prefix);
    virtual ~OptionalItemParser() {}

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, nlohmann::json& target,
                             bool debug) override;

    virtual void addInfo (const std::string& edition, CategoryItemInfo& info) const override;

  protected:
    std::string bitfield_name_;
    unsigned int bitfield_index_{0};
    std::vector<std::unique_ptr<ItemParserBase>> data_fields_;
};

}  // namespace jASTERIX

#endif  // OPTIONALITEMPARSER_H
