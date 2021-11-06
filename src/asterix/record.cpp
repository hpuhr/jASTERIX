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

#include "record.h"

#include "logger.h"
#include "string_conv.h"

using namespace std;
using namespace nlohmann;

namespace jASTERIX
{
Record::Record(const nlohmann::json& item_definition) : ItemParserBase(item_definition)
{
    assert(type_ == "record");

    // fspec

    if (!item_definition.contains("field_specification"))
        throw runtime_error("record item '" + name_ + "' parsing without field specification");

    const json& field_specification = item_definition.at("field_specification");

    if (!field_specification.is_object())
        throw runtime_error("record item '" + name_ + "' field specification is not an object");

    field_specification_.reset(ItemParserBase::createItemParser(field_specification, ""));
    assert(field_specification_);

    // uap

    if (!item_definition.contains("uap"))
        throw runtime_error("record item '" + name_ + "' without uap");

    const json& uap = item_definition.at("uap");

    if (!uap.is_array())
        throw runtime_error("record item '" + name_ + "' uap is not an array");

    for (const auto& uap_item : uap)
        uap_names_.push_back(uap_item);

    // conditional uaps

    if (item_definition.contains("conditional_uaps"))
    {
        has_conditional_uap_ = true;
        const json& conditional_uaps = item_definition.at("conditional_uaps");

        if (!conditional_uaps.is_object())
            throw runtime_error("record item '" + name_ + "' conditional uaps is not an object");

        if (!conditional_uaps.contains("key"))
            throw runtime_error("record item '" + name_ + "' conditional uap without key");

        conditional_uaps_key_ = conditional_uaps.at("key");

        conditional_uaps_sub_keys_ = split(conditional_uaps_key_, '.');

        if (!conditional_uaps.contains("values"))
            throw runtime_error("record item '" + name_ + "' conditional uap without values");

        const json& conditional_uaps_values = conditional_uaps.at("values");

        if (!conditional_uaps_values.is_object())
            throw runtime_error("record item '" + name_ +
                                "' conditional uap values is not an object");

        for (auto uap_conditional_value = conditional_uaps_values.begin();
             uap_conditional_value != conditional_uaps_values.end(); ++uap_conditional_value)
        {
            std::string value_key = uap_conditional_value.key();

            if (conditional_uap_names_.count(value_key) != 0)
                throw std::runtime_error("record item '" + name_ +
                                         "' conditional uap values has multiple uaps with"
                                         " same key");

            if (!uap_conditional_value.value().is_array())
                throw std::runtime_error("record item '" + name_ +
                                         "' conditional uap values has non-array values");

            std::vector<std::string> value_uap = uap_conditional_value.value();

            conditional_uap_names_[value_key] = value_uap;
        }
    }

    // items

    if (!item_definition.contains("items"))
        throw runtime_error("record item '" + name_ + "' without items");

    const json& items = item_definition.at("items");

    if (!items.is_array())
        throw runtime_error("record item '" + name_ + "' field specification is not an array");

    std::string item_number;
    ItemParserBase* item{nullptr};

    for (const json& data_item_it : items)
    {
        if (!data_item_it.contains("number"))
            throw runtime_error("record item '" + data_item_it.dump(4) + "' without number");

        item_number = data_item_it.at("number");
        item = ItemParserBase::createItemParser(data_item_it, "");
        assert(item);

        if (items_.count(item_number) != 0)
            throw runtime_error("record item '" + name_ + "' item number '" + item_number +
                                "' used multiple times");

        items_[item_number] = std::unique_ptr<ItemParserBase>{item};
    }
}

size_t Record::parseItem(const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                         nlohmann::json& target, bool debug)
{
    if (debug)
        loginf << "parsing record item '" << name_ << "' with index " << index << " size " << size
               << " current parsed bytes " << current_parsed_bytes << logendl;

    size_t parsed_bytes{0};

    if (debug)
        loginf << "parsing record item '" + name_ + "' field specification" << logendl;

    parsed_bytes = field_specification_->parseItem(data, index + parsed_bytes, size, parsed_bytes,
                                                   target, debug);

    if (!target.contains("FSPEC"))
        throw runtime_error("record item '" + name_ + "' FSPEC not found");

    std::vector<bool> fspec_bits = target.at("FSPEC");

    if (!has_conditional_uap_ && fspec_bits.size() > uap_names_.size())
        throw runtime_error("record item '" + name_ +
                            "' has more FSPEC bits than defined uap items");

    if (debug)
        loginf << "parsing record item '" + name_ + "' data items" << logendl;

    size_t uap_cnt{0};
    size_t num_fspec_bits = fspec_bits.size();

    bool special_purpose_field_present{false};
    bool reserved_expansion_field_present{false};

    for (const auto& item_name : uap_names_)  // parse static uap items
    {
        if (uap_cnt >= num_fspec_bits)
            break;

        if (fspec_bits.at(uap_cnt))
        {
            uap_cnt++;

            if (item_name == "FX")  // extension into next byte
                continue;

            if (item_name == "-")  // bit not used
                continue;

            if (item_name == "SP")  // special purpose field
            {
                // loginf << "WARN: record item '"+name_+"' has special purpose field, not
                // implemented yet"
                // << logendl;
                special_purpose_field_present = true;
                continue;
            }

            if (item_name == "RE")  // reserved expansion field
            {
                // loginf << "WARN: record item '"+name_+"' has reserved expansion field, not
                // implemented yet" << logendl;
                reserved_expansion_field_present = true;
                continue;
            }

            if (debug)
                loginf << "parsing record item '" << name_ << "' data item '" << item_name
                       << "' index " << index + parsed_bytes << logendl;

            if (items_.count(item_name) != 1)
                throw runtime_error("record item '" + name_ + "' references undefined item '" +
                                    item_name + "'");

            parsed_bytes += items_.at(item_name)->parseItem(data, index + parsed_bytes, size,
                                                            parsed_bytes, target, debug);
        }
        else
            uap_cnt++;
    }

    if (has_conditional_uap_)
    {
        for (const auto& conditional_uap_value : conditional_uap_names_)
        {
            if (compareKey(target, conditional_uap_value.first))
            {
                if (debug)
                    loginf << "record item '" << name_ << "' with conditinal "
                           << conditional_uaps_key_ << " value " << conditional_uap_value.first
                           << " found" << logendl;

                for (const auto& item_name :
                     conditional_uap_value.second)  // parse dynamic uap items
                {
                    if (uap_cnt >= num_fspec_bits)
                        break;

                    if (fspec_bits.at(uap_cnt))
                    {
                        uap_cnt++;

                        if (item_name == "FX")  // extension into next byte
                            continue;

                        if (item_name == "-")  // bit not used
                            continue;

                        if (item_name == "SP")  // special purpose field
                        {
                            // loginf << "WARN: record item '"+name_+"' has special purpose field,
                            // not implemented yet" << logendl;
                            special_purpose_field_present = true;
                            continue;
                        }

                        if (item_name == "RE")  // reserved expansion field
                        {
                            // loginf << "WARN: record item '"+name_+"' has reserved expansion
                            // field, not implemented yet" << logendl;
                            reserved_expansion_field_present = true;
                            continue;
                        }

                        if (debug)
                            loginf << "parsing record item '" << name_ << "' data item '"
                                   << item_name << "' index " << index + parsed_bytes << logendl;

                        if (items_.count(item_name) != 1)
                            throw runtime_error("record item '" + name_ +
                                                "' references undefined item '" + item_name + "'");

                        parsed_bytes += items_.at(item_name)->parseItem(
                            data, index + parsed_bytes, size, parsed_bytes, target, debug);
                    }
                    else
                        uap_cnt++;
                }
            }
        }
    }

    if (reserved_expansion_field_present)
    {
        size_t re_bytes = static_cast<unsigned char>(data[index + parsed_bytes]);
        parsed_bytes += 1;  // read 1 len byte
        re_bytes -= 1;      // includes 1 len byte

        if (ref_)  // decode ref
        {
            if (debug)
                loginf << "record '" + name_ + "' has reserved expansion field, reading "
                       << re_bytes << " bytes " << logendl;

            assert(re_bytes > 1);

            size_t ref_bytes =
                ref_->parseItem(data, index + parsed_bytes, re_bytes, 0, target["REF"], debug);

            if (debug)
                loginf << "record '" + name_ + "' parsed reserved expansion field, read "
                       << ref_bytes << " ref in " << re_bytes << " bytes " << logendl;

            if (ref_bytes != re_bytes)
                throw runtime_error(
                    "record item '" + name_ + "' reserved expansion field definition only read " +
                    to_string(ref_bytes) + " bytes of specified " + to_string(re_bytes));

            // loginf << "UGA REF '" << target["REF"].dump(4) << "'" << logendl;
        }
        else
        {
            if (debug)
                loginf << "record '" + name_ + "' has reserved expansion field, reading "
                       << re_bytes << " bytes " << logendl;

            target["REF"] = binary2hex((const unsigned char*)&data[index + parsed_bytes], re_bytes);
        }

        parsed_bytes += re_bytes;
    }

    if (special_purpose_field_present)
    {
        size_t re_bytes = static_cast<unsigned char>(data[index + parsed_bytes]);

        parsed_bytes += 1;  // read 1 len byte
        re_bytes -= 1;      // includes 1 len byte

        if (spf_)  // decode ref
        {
            if (debug)
                loginf << "record '" + name_ + "' has special purpose field, reading " << re_bytes
                       << " bytes " << logendl;

            assert(re_bytes > 1);

            size_t ref_bytes =
                spf_->parseItem(data, index + parsed_bytes, re_bytes, 0, target["SPF"], debug);

            if (debug)
                loginf << "record '" + name_ + "' parsed special purpose field, read " << ref_bytes
                       << " ref in " << re_bytes << " bytes " << logendl;

            if (ref_bytes != re_bytes)
                throw runtime_error(
                    "record item '" + name_ + "' special purpose field definition only read " +
                    to_string(ref_bytes) + " bytes of specified " + to_string(re_bytes));

            // loginf << "UGA SPF '" << target["SPF"].dump(4) << "'" << logendl;
        }
        else
        {
            if (debug)
                loginf << "record '" + name_ + "' has special purpose field, reading " << re_bytes
                       << " bytes " << logendl;

            target["SPF"] = binary2hex((const unsigned char*)&data[index + parsed_bytes], re_bytes);
        }

        parsed_bytes += re_bytes;
    }

    return parsed_bytes;
}

bool Record::decodeREF() const { return decode_ref_; }

void Record::decodeREF(bool decode_ref) { decode_ref_ = decode_ref; }

std::shared_ptr<ReservedExpansionField> Record::ref() const { return ref_; }

void Record::setRef(const std::shared_ptr<ReservedExpansionField>& ref) { ref_ = ref; }

std::shared_ptr<SpecialPurposeField> Record::spf() const { return spf_; }

void Record::setSpf(const std::shared_ptr<SpecialPurposeField>& spf) { spf_ = spf; }

void Record::addInfo (const std::string& edition, CategoryItemInfo& info) const
{
    for (auto& item_it : items_)
        item_it.second->addInfo(edition, info);
}

// bool Record::compareKey (const nlohmann::json& container, const std::string& value)
//{
//    //loginf << "mapping key '" << key_definition << "' src value '" << src_value << "'";

//    const nlohmann::json* val_ptr = &container;

//    for (const std::string& sub_key : conditional_uaps_sub_keys_)
//    {
//        //loginf << "UGA '" << sub_key << "' json '" << val_ptr->dump(4) << "'"<< logendl;
//        val_ptr = &(*val_ptr)[sub_key];
//    }

//    return (toString(*val_ptr) == value);

//    //loginf << "mapping key '" << key_definition << "' dest '" << dest.dump(4) << "'";
//}

}  // namespace jASTERIX
