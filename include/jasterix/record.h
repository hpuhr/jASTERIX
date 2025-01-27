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

#ifndef RECORD_H
#define RECORD_H

#include <jasterix/itemparserbase.h>
#include <jasterix/ref.h>
#include <jasterix/spf.h>

#include <memory>
#include <vector>

namespace jASTERIX
{
// decodes a field specification/availablity field (ending with extend bit), and list of items
class Record : public ItemParserBase
{
  public:
    Record(const nlohmann::json& item_definition);
    virtual ~Record() override {}

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, size_t total_size,
                             nlohmann::json& target, bool debug) override;
    bool decodeREF() const;
    void decodeREF(bool decodeREF);

    std::shared_ptr<ReservedExpansionField> ref() const;
    void setRef(const std::shared_ptr<ReservedExpansionField>& ref);

    std::shared_ptr<SpecialPurposeField> spf() const;
    void setSpf(const std::shared_ptr<SpecialPurposeField>& spf);

    virtual void addInfo (const std::string& edition, CategoryItemInfo& info) const override;

  protected:
    std::unique_ptr<ItemParserBase> field_specification_;
    std::vector<std::string> uap_names_;

    bool has_conditional_uap_{false};
    std::string conditional_uaps_key_;
    std::vector<std::string> conditional_uaps_sub_keys_;
    std::map<std::string, std::vector<std::string>> conditional_uap_names_;
    std::map<std::string, std::unique_ptr<ItemParserBase>> items_;

    bool decode_ref_{false};
    std::shared_ptr<ReservedExpansionField> ref_;
    std::shared_ptr<SpecialPurposeField> spf_;

    bool compareKey(const nlohmann::json& container, const std::string& value, bool debug);
    std::string getValue(const nlohmann::json& container, bool debug);
};

}  // namespace jASTERIX
#endif  // RECORD_H
