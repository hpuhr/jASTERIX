#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <string>
#include <map>
#include <set>

namespace jASTERIX
{

class ItemInfo
{
    std::string description_; // item description
    std::set<std::string> editions_; // item edition numbers
    std::set<std::string> ref_editions_; // REF item edition numbers
    std::set<std::string> spf_editions_; // SPF item edition numbers
};

using CategoryItemInfo = std::map<std::string, ItemInfo>; // flattended item name, e.g. 010.SAC -> info

}

#endif // ITEMINFO_H
