#ifndef SPECIALPURPOSEFIELD_H
#define SPECIALPURPOSEFIELD_H

#include <jasterix/itemparserbase.h>

#include <memory>
#include <vector>

namespace jASTERIX
{
// decodes a field specification/availablity field (ending with extend bit), and list of items
class SpecialPurposeField : public ItemParserBase
{
   public:
    SpecialPurposeField(const nlohmann::json& item_definition);
    virtual ~SpecialPurposeField() override;

    virtual size_t parseItem(const char* data, size_t index, size_t size,
                             size_t current_parsed_bytes, nlohmann::json& target,
                             bool debug) override;

   protected:
    std::unique_ptr<ItemParserBase> complex_field_specification_;
    std::vector<std::string> complex_items_names_;
    std::map<std::string, std::unique_ptr<ItemParserBase>> complex_items_;

    std::vector<std::unique_ptr<ItemParserBase>> simple_items_;

    size_t parseSimpleItem(const char* data, size_t index, size_t size, size_t current_parsed_bytes,
                           nlohmann::json& target, bool debug);

    size_t parseComplexItem(const char* data, size_t index, size_t size,
                            size_t current_parsed_bytes, nlohmann::json& target, bool debug);
};

}  // namespace jASTERIX

#endif  // SPECIALPURPOSEFIELD_H
