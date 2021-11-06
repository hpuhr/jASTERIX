#ifndef JASTERIX_EDITIONBASE_H
#define JASTERIX_EDITIONBASE_H

#include <string>

#include <jasterix/iteminfo.h>

#include "json.hpp"

namespace jASTERIX {


class EditionBase
{
public:
    EditionBase(const std::string& number, const nlohmann::json& definition,
                const std::string& definition_path);
    virtual ~EditionBase();

    std::string number() const;
    std::string document() const;
    std::string date() const;
    std::string file() const;
    std::string definitionPath() const;

    virtual void addInfo (CategoryItemInfo& info)=0;

protected:
    std::string number_;
    std::string document_;
    std::string date_;
    std::string file_;

    std::string edition_definition_path_;
    nlohmann::json definition_;  // from file
};

} // namespace jASTERIX

#endif // JASTERIX_EDITIONBASE_H
