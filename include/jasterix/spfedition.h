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

#ifndef SPFEDITION_H
#define SPFEDITION_H

#include <jasterix/editionbase.h>
#include <jasterix/spf.h>

#include <string>

#include "json.hpp"

namespace jASTERIX
{
class Record;

class SPFEdition : public EditionBase
{
  public:
    SPFEdition(const std::string& number, const nlohmann::json& definition,
               const std::string& definition_path);
    virtual ~SPFEdition();

    std::shared_ptr<SpecialPurposeField> specialPurposeField() const;

    virtual void addInfo (CategoryItemInfo& info, const std::string& prefix="") override;

  protected:
    std::shared_ptr<SpecialPurposeField> spf_;
};

}  // namespace jASTERIX
#endif  // SPFEDITION_H
