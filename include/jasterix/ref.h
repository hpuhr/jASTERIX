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

#ifndef RESERVEDEXPSIONFIELD_H
#define RESERVEDEXPSIONFIELD_H

#include <jasterix/itemparserbase.h>

#include <memory>
#include <vector>

namespace jASTERIX
{
// decodes a field specification/availablity field (ending with extend bit), and list of items
class ReservedExpansionField : public ItemParserBase
{
  public:
    ReservedExpansionField(const nlohmann::json& item_definition);
    virtual ~ReservedExpansionField() override;

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, size_t total_size, nlohmann::json& target,
                             bool debug) override;

    virtual void addInfo (const std::string& edition, CategoryItemInfo& info) const override;

  protected:
    std::unique_ptr<ItemParserBase> field_specification_;
    std::vector<std::string> items_names_;
    std::map<std::string, std::unique_ptr<ItemParserBase>> items_;
};

}  // namespace jASTERIX
#endif  // RESERVEDEXPSIONFIELD_H
