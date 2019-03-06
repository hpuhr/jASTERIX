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

#include "files.h"
#include "global.h"

#include <cassert>
#include <stdexcept>
#include <iostream>

#if USE_BOOST
#include <boost/filesystem.hpp>
#else
#include <fstream>
#include <dirent.h>
#include <errno.h>
#endif

using namespace std;

namespace jASTERIX
{

namespace Files
{

#if USE_BOOST
bool fileExists(const std::string& path)
{
    return boost::filesystem::exists(path);
}

size_t fileSize(const std::string& path)
{
    assert (fileExists(path));
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

#else
bool fileExists(const std::string& path)
{
    ifstream f(path.c_str());
    return f.good();
}

size_t fileSize(const std::string& path)
{
    assert (fileExists(path));
    std::ifstream in(path.c_str(), std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

bool directoryExists(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
        return true;
    }
    else if (ENOENT == errno)
    {
        /* Directory does not exist. */
        return false;
    }
    else
    {
        /* opendir() failed for some other reason. */
        return false;
    }
}

std::vector<std::string> getFilesInDirectory(const std::string& path)
{
    std::vector<std::string> tmp;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            tmp.push_back(ent->d_name);
            //printf ("%s\n", ent->d_name);
        }
        closedir (dir);
    }
    else
    {
        /* could not open directory */
        return tmp;
    }
    return tmp;
}
#endif

}

}
