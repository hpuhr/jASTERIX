/*
 * This file is part of jASTERIX.
 *
 * jASTERIX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jASTERIX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with jASTERIX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mapping.h"
#include "files.h"

#include <fstream>

namespace jASTERIX
{

using namespace Files;
using namespace std;
using namespace nlohmann;


Mapping::Mapping(const std::string& name, const nlohmann::json& definition, const std::string& definition_path)
: name_(name)
{
    //    "comment":"conversion to general format 1.0",
    if (definition.find("comment") == definition.end())
        throw runtime_error ("mapping '"+name_+"' has no comment");

    comment_ = definition.at("comment");

    //    "file": "048/cat048_1.15.json"
    if (definition.find("file") == definition.end())
        throw runtime_error ("mapping '"+name_+"' has no file");

    file_ = definition.at("file");

    if (!fileExists(definition_path+"/categories/"+file_))
        throw invalid_argument ("mapping "+name_+" file '"+definition_path+"/categories/"+file_+"' not found");

    definition_ = json::parse(ifstream(definition_path+"/categories/"+file_));

}

std::string Mapping::name() const
{
    return name_;
}

std::string Mapping::comment() const
{
    return comment_;
}

std::string Mapping::file() const
{
    return file_;
}

}
