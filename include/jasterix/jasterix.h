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

#ifndef JASTERIX_H
#define JASTERIX_H

#include <string>
#include <map>
#include <jasterix/global.h>

#if USE_BOOST
#include <boost/iostreams/device/mapped_file.hpp>
#endif

//#include "tbb/task_scheduler_init.h"
#include "tbb/concurrent_queue.h"

#include "json.hpp"
#include <jasterix/frameparser.h>
#include <jasterix/category.h>
#include <jasterix/edition.h>
#include <jasterix/mapping.h>

namespace jASTERIX {

extern int print_dump_indent;
extern int frame_limit;
extern int frame_chunk_size;
extern int record_chunk_size;
extern int data_write_size;

extern bool single_thread;

#if USE_OPENSSL
extern bool add_artas_md5_hash;
#endif

extern bool add_record_data;

class jASTERIX
{
public:
    jASTERIX(const std::string& definition_path, bool print, bool debug, bool debug_exclude_framing);
    virtual ~jASTERIX();

    bool hasCategory(unsigned int cat);
    bool decodeCategory(unsigned int cat);
    void setDecodeCategory (unsigned int cat, bool decode);
    void decodeNoCategories();

    std::shared_ptr<Category> category (unsigned int cat);
    const std::map<unsigned int, std::shared_ptr<Category>>& categories() { return category_definitions_; }

    void decodeFile (const std::string& filename, const std::string& framing_str,
                     std::function<void(std::unique_ptr<nlohmann::json>, size_t, size_t, size_t)> data_callback=nullptr);
    // callback gets moved chunk, accumulated number of frames, number of records, number of errors
    void decodeFile (const std::string& filename,
                     std::function<void(std::unique_ptr<nlohmann::json>, size_t, size_t, size_t)> data_callback=nullptr);

    void decodeASTERIX (const char* data, size_t size,
                 std::function<void(std::unique_ptr<nlohmann::json>, size_t, size_t, size_t)> data_callback=nullptr);

    size_t numFrames() const;
    size_t numRecords() const;
    size_t numErrors() const;

    void addDataBlockChunk (std::unique_ptr<nlohmann::json> data_block_chunk, bool error, bool done);
    void addDataChunk (std::unique_ptr<nlohmann::json> data_chunk, bool done);

    const std::vector<std::string>& framings() { return framings_; }

    const std::string& dataBlockDefinitionPath() const;
    const std::string& categoriesDefinitionPath() const;
    const std::string& framingsFolderPath() const;

    void setDebug(bool debug);


private:
    //tbb::task_scheduler_init init_;

    std::string definition_path_;
    bool print_ {false};
    bool debug_ {false};
    bool debug_exclude_framing_ {false};

    std::string framing_path_;
    std::vector<std::string> framings_;

    std::string data_block_definition_path_;
    nlohmann::json data_block_definition_;

    std::string categories_definition_path_;
    nlohmann::json categories_definition_;

    std::map<unsigned int, std::shared_ptr<Category>> category_definitions_;

#if USE_BOOST
    boost::iostreams::mapped_file_source file_;
#else
    char* file_buffer_{nullptr};
#endif

    tbb::concurrent_queue<std::unique_ptr<nlohmann::json>> data_block_chunks_;
    bool data_block_processing_done_ {false};

    tbb::concurrent_queue<std::unique_ptr<nlohmann::json>> data_chunks_;
    bool data_processing_done_ {false};

    size_t num_frames_{0};
    size_t num_records_{0};
    size_t num_errors_{0};
};
}

#endif // JASTERIX_H
