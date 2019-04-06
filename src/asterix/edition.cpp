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

#include "edition.h"
#include "files.h"
#include "record.h"

#include <fstream>

namespace jASTERIX
{

using namespace Files;
using namespace std;
using namespace nlohmann;

Edition::Edition(const std::string& number, const nlohmann::json& definition, const std::string& definition_path)
  : number_(number)
{
    //    "document": "SUR.ET1.ST05.2000-STD-04-01",
    if (definition.find("document") == definition.end())
        throw runtime_error ("edition '"+number_+"' has no document reference");

    document_ = definition.at("document");

    //    "date": "April 2007",
    if (definition.find("date") == definition.end())
        throw runtime_error ("edition '"+number_+"' has no date");

    date_ = definition.at("date");

    //    "file": "048/cat048_1.15.json"
    if (definition.find("file") == definition.end())
        throw runtime_error ("edition '"+number_+"' has no file");

    file_ = definition.at("file");

    if (!fileExists(definition_path+"/categories/"+file_))
        throw invalid_argument ("edition "+number_+" file '"+definition_path+"/categories/"+file_+"' not found");

    definition_ = json::parse(ifstream(definition_path+"/categories/"+file_));

    record_.reset(new Record(definition_));
}

std::string Edition::document() const
{
    return document_;
}

std::string Edition::date() const
{
    return date_;
}

std::string Edition::file() const
{
    return file_;
}

std::shared_ptr<Record> Edition::record() const
{
    return record_;
}

std::string Edition::number() const
{
    return number_;
}

}
