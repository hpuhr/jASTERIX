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

#ifndef JASTERIX_H
#define JASTERIX_H

#include <string>
#include <map>
#include <boost/iostreams/device/mapped_file.hpp>
#include "tbb/concurrent_queue.h"

#include "json.hpp"
#include "frameparser.h"

namespace jASTERIX {

class jASTERIX
{
public:
    jASTERIX(const std::string& filename, const std::string& definition_path, const std::string& framing_str,
             bool print, bool debug);
    virtual ~jASTERIX();

    // returns number of decoded records
    void decode ();

    size_t numFrames() const;
    size_t numRecords() const;

    void addDataChunk (nlohmann::json& data_chunk);

private:
    std::string filename_;
    std::string definition_path_;
    std::string framing_;
    bool print_ {false};
    bool debug_ {false};

    nlohmann::json framing_definition_;
    nlohmann::json data_block_definition_;
    nlohmann::json asterix_list_definition_;
    std::map<unsigned int, nlohmann::json> asterix_category_definitions_;
    std::unique_ptr<FrameParser> frame_parser_;

    size_t file_size_{0};
    boost::iostreams::mapped_file_source file_;

    tbb::concurrent_queue<nlohmann::json> data_chunks_;


    size_t num_frames_{0};
    size_t num_records_{0};
};
}

#endif // JASTERIX_H
