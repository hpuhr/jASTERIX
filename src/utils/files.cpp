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

#include "files.h"

#include <boost/filesystem.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "jasterix/global.h"

using namespace std;

namespace jASTERIX
{
namespace Files
{
bool fileExists(const std::string& path) { return boost::filesystem::exists(path); }

size_t fileSize(const std::string& path)
{
    assert(fileExists(path));
    return boost::filesystem::file_size(path);
}

bool directoryExists(const std::string& path)
{
    return boost::filesystem::exists(path) && boost::filesystem::is_directory(path);
}

struct path_leaf_string
{
    std::string operator()(const boost::filesystem::directory_entry& entry) const
    {
        return entry.path().leaf().string();
    }
};

std::vector<std::string> getFilesInDirectory(const std::string& path)
{
    std::vector<std::string> tmp;

    boost::filesystem::path p(path);
    boost::filesystem::directory_iterator start(p);
    boost::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(tmp), path_leaf_string());

    return tmp;
}

}  // namespace Files

}  // namespace jASTERIX
