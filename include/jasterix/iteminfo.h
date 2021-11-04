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
    std::set<std::string> editions_; // item edition numbers, includes REF + SPF
};

using CategoryItemInfo = std::map<std::string, ItemInfo>; // flattended item name, e.g. 010.SAC -> info

}

#endif // ITEMINFO_H
