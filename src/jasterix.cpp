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


#include "jasterix.h"
#include "files.h"
#include "logger.h"
#include "asterixparser.h"
#include "datablockfindertask.h"
#include "frameparser.h"
#include "frameparsertask.h"
#include "category.h"
#include "edition.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <malloc.h>

using namespace nlohmann;

namespace jASTERIX {

    int print_dump_indent=4;
    int frame_limit=-1;
    int frame_chunk_size=20000;
    int record_chunk_size=20000;
    int data_write_size=100;
    bool single_thread=false;

#if USE_OPENSSL
    bool add_artas_md5_hash=false;
#endif

    bool add_record_data=false;

    using namespace Files;
    using namespace std;

    const std::string FRAMING_SUBDIR = "/framings";
    const std::string DATABLOCK_FILENAME = "/data_block_definition.json";
    const std::string CATEGORY_SUBDIR = "/categories";
    const std::string CATEGORIES_FILENAME = "/categories.json";

    jASTERIX::jASTERIX(const std::string& definition_path, bool print, bool debug, bool debug_exclude_framing)
        : definition_path_(definition_path), print_(print), debug_(debug), debug_exclude_framing_(debug_exclude_framing)
    {
        // check framing definitions
        if (!directoryExists(definition_path_))
            throw invalid_argument ("jASTERIX called with non-existing definition path '"+definition_path_+"'");

        framing_path_ = definition_path_+FRAMING_SUBDIR;

        if (!directoryExists(framing_path_))
            throw invalid_argument ("jASTERIX called with missing framing path '"+framing_path_+"'");

        framings_.push_back(""); // add no framing
        std::string file_ending = ".json";
        for (std::string framing_file : Files::getFilesInDirectory(framing_path_))
        {
            size_t pos = framing_file.find(file_ending);

            if (pos != std::string::npos) // if ends with json
            {
                // If found then erase it from string
                framing_file.erase(pos, file_ending.length());
                framings_.push_back(framing_file);
            }
        }

        data_block_definition_path_ = definition_path_+DATABLOCK_FILENAME;

        if (!fileExists(data_block_definition_path_))
            throw invalid_argument ("jASTERIX called without asterix data block definition");

        // check asterix category definitions

        if (!directoryExists(definition_path_+CATEGORY_SUBDIR))
            throw invalid_argument ("jASTERIX called with missing categories definition folder '"
                                    +definition_path_+CATEGORY_SUBDIR+"'");

        categories_definition_path_ = definition_path_+CATEGORY_SUBDIR+CATEGORIES_FILENAME;
        if (!fileExists(categories_definition_path_))
            throw invalid_argument ("jASTERIX called without missing asterix categories definition path '"
                                    +categories_definition_path_+"'");

        try // asterix record definition
        {
            data_block_definition_ = json::parse(ifstream(definition_path_+DATABLOCK_FILENAME));
        }
        catch (json::exception& e)
        {
            throw runtime_error (string{"jASTERIX parsing error in asterix data block definition: "}+e.what());
        }

        try // asterix categories list definition
        {
            categories_definition_ = json::parse(ifstream(categories_definition_path_));
        }
        catch (json::exception& e)
        {
            throw runtime_error (string{"jASTERIX parsing error in asterix categories definition: "}+e.what());
        }

        if (!categories_definition_.is_object())
            throw invalid_argument ("jASTERIX called with non-object asterix categories list definition");

        try // asterix category definitions
        {
            std::string cat_str;
            unsigned int cat;

            for (auto cat_def_it = categories_definition_.begin(); cat_def_it != categories_definition_.end();
                 ++cat_def_it)
            {
                cat = 256; // impossible number
                cat_str = cat_def_it.key();
                cat = static_cast<unsigned int> (stoul(cat_str));

                if (cat > 255 || category_definitions_.count(cat) != 0)
                    throw invalid_argument (
                            "jASTERIX called with wrong asterix category '"+cat_str+"' in list definition");

                if (debug)
                    loginf << "jASTERIX found asterix category " << cat_str << logendl;

                try
                {
                    category_definitions_[cat] = std::shared_ptr<Category> (
                                new Category(cat_str, cat_def_it.value(), definition_path));

                    assert (category_definitions_.count(cat) == 1);
                }
                catch (json::exception& e)
                {
                    throw runtime_error ("jASTERIX parsing error in asterix category "+cat_str+": "+e.what());
                }
            }
        }
        catch (json::exception& e)
        {
            throw runtime_error (string{"jASTERIX parsing error in asterix category definitions: "}+e.what());
        }
    }

    jASTERIX::~jASTERIX()
    {
#if USE_BOOST
        if(file_.is_open())
            file_.close();
#else
        if (file_buffer_)
        {
            delete file_buffer_;
            file_buffer_ = nullptr;
        }
#endif
    }

    bool jASTERIX::hasCategory(unsigned int cat)
    {
        return category_definitions_.count(cat) == 1;
    }

    bool jASTERIX::decodeCategory(unsigned int cat)
    {
        assert (hasCategory(cat));
        return category_definitions_.at(cat)->decode();
    }

    void jASTERIX::setDecodeCategory (unsigned int cat, bool decode)
    {
        assert (hasCategory(cat));
        category_definitions_.at(cat)->decode(decode);
    }

    void jASTERIX::decodeNoCategories()
    {
        for (auto& cat_it : category_definitions_)
            cat_it.second->decode(false);
    }

    std::shared_ptr<Category> jASTERIX::category (unsigned int cat)
    {
        assert (hasCategory(cat));
        return category_definitions_.at(cat);
    }

    void jASTERIX::decodeFile (const std::string& filename, const std::string& framing,
                               std::function<void(std::unique_ptr<nlohmann::json>, size_t, size_t, size_t)> data_callback)
    {
        // check and open file
        if (!fileExists(filename))
            throw invalid_argument ("jASTERIX called with non-existing file '"+filename+"'");

        size_t file_size = fileSize (filename);

        if (!file_size)
            throw invalid_argument ("jASTERIX called with empty file '"+filename+"'");

        if (debug_)
            loginf << "jASTERIX: file " << filename << " size " << file_size << logendl;

#if USE_BOOST
        assert (!file_.is_open());

        file_.open(filename, file_size);

        if(!file_.is_open())
            throw runtime_error ("jASTERIX unable to map file '"+filename+"'");

        const char* data = file_.data();
#else
        ifstream file (filename, ios::in | ios::binary);
        file_buffer_ = new char[file_size];
        file.read (file_buffer_, file_size);
        if (!file)
        {
            // An error occurred!
            throw runtime_error ("jASTERIX unable to read file '"+filename+"'");
        }
        file.close();

        char* data = file_buffer_;
#endif

        // check framing
        if (!fileExists(definition_path_+"/framings/"+framing+".json"))
            throw invalid_argument ("jASTERIX called with unknown framing '"+framing+"'");

        nlohmann::json framing_definition;

        try // create framing definition
        {
            framing_definition = json::parse(ifstream(definition_path_+"/framings/"+framing+".json"));
        }
        catch (json::exception& e)
        {
            throw runtime_error ("jASTERIX parsing error in framing definition '"+framing+"': "+e.what());
        }

        // create ASTERIX parser
        ASTERIXParser asterix_parser (data_block_definition_, category_definitions_, debug_);

        // create frame parser
        bool debug_framing = debug_ && !debug_exclude_framing_;
        FrameParser frame_parser (framing_definition, asterix_parser, debug_framing);

        nlohmann::json json_header;

        size_t index {0};

        // parsing header

        if (frame_parser.hasFileHeaderItems())
            index = frame_parser.parseHeader(data, 0, file_size, json_header, debug_framing);

        if (debug_)
            loginf << "jasterix: creating frame parser task" << logendl;

        FrameParserTask* task = new (tbb::task::allocate_root()) FrameParserTask (
                    *this, frame_parser, json_header, data, index, file_size, debug_framing);
        tbb::task::enqueue(*task);

        if (debug_)
            while (!task->done())
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::unique_ptr<nlohmann::json> data_chunk;

        size_t num_callback_frames;
        std::pair<size_t, size_t> dec_ret {0, 0};

        while (1)
        {
            if (data_processing_done_ && data_chunks_.empty())
                break;

            if (data_chunks_.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            assert (!data_chunk);

            data_chunks_mutex_.lock();

            data_chunk = std::move (data_chunks_.front());
            data_chunks_.pop_front();

            data_chunks_mutex_.unlock();

            if (debug_)
                loginf << "jASTERIX: decoding frames" << logendl;

            num_callback_frames = data_chunk->at("frames").size();
            num_frames_ += num_callback_frames;

            try
            {
                dec_ret = frame_parser.decodeFrames(data, data_chunk.get(), debug_);
                num_records_ += dec_ret.first;
                num_errors_ += dec_ret.second;

                if (debug_)
                    loginf << "jASTERIX processing " << num_frames_ << " frames, " << num_records_ << " records "
                           << num_errors_ << " errors " << logendl;

                if (print_)
                    loginf << data_chunk->dump(print_dump_indent) << logendl;

                if (data_callback)
                    data_callback(std::move(data_chunk), num_callback_frames, dec_ret.first, dec_ret.second);
                else
                    data_chunk = nullptr;

                if (frame_limit > 0 && num_frames_ >= static_cast<unsigned>(frame_limit))
                {
                    if (debug_)
                        loginf << "jASTERIX processing hit framelimit" << logendl;

                    break;
                }
            }
            catch (std::exception& e)
            {
                loginf << "jASTERIX caught exception '" << e.what() << "', breaking" << logendl;
                task->forceStop();

                while (!task->done())
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));

                throw; // rethrow
            }
        }

        if (debug_)
            loginf << "jASTERIX decode file done" << logendl;

#if USE_BOOST
        file_.close();
#endif
    }

    void jASTERIX::decodeFile (
            const std::string& filename,
            std::function<void(std::unique_ptr<nlohmann::json>, size_t, size_t, size_t)> data_callback)
    {
        // check and open file
        if (!fileExists(filename))
            throw invalid_argument ("jASTERIX called with non-existing file '"+filename+"'");

        size_t file_size = fileSize (filename);

        if (!file_size)
            throw invalid_argument ("jASTERIX called with empty file '"+filename+"'");

        if (debug_)
            loginf << "jASTERIX: file " << filename << " size " << file_size << logendl;

#if USE_BOOST
        assert (!file_.is_open());
        file_.open(filename, file_size);

        if(!file_.is_open())
            throw runtime_error ("jASTERIX unable to map file '"+filename+"'");

        const char* data = file_.data();
#else
        ifstream file (filename, ios::in | ios::binary);
        file_buffer_ = new char[file_size];
        file.read (file_buffer_, file_size);
        if (!file)
        {
            // An error occurred!
            throw runtime_error ("jASTERIX unable to read file '"+filename+"'");
        }
        file.close();

        char* data = file_buffer_;
#endif

        // create ASTERIX parser
        ASTERIXParser asterix_parser (data_block_definition_, category_definitions_, debug_);

        if (debug_)
            loginf << "jASTERIX: finding data blocks" << logendl;

        size_t index {0};

        DataBlockFinderTask* task = new (tbb::task::allocate_root()) DataBlockFinderTask (
                    *this, asterix_parser, data, index, file_size, debug_);
        tbb::task::enqueue(*task);

        if (debug_)
            while (!task->done())
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::unique_ptr<nlohmann::json> data_block_chunk;

        std::pair<size_t, size_t> dec_ret {0,0};

        while (1)
        {
            if (data_block_processing_done_ && data_block_chunks_.empty())
                break;

            if (data_block_chunks_.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            //loginf << "jasterix: task done " << data_block_processing_done_ << " empty " << data_block_chunks_.empty()
            // << logendl;

            assert (!data_block_chunk);

            data_block_chunks_mutex_.lock();

            data_block_chunk = std::move(data_block_chunks_.front());
            data_block_chunks_.pop_front();

            data_block_chunks_mutex_.unlock();


            if (debug_)
                loginf << "jasterix: decoding data block" << logendl;

            try
            {
                if (!data_block_chunk->contains ("data_blocks"))
                    throw runtime_error("jasterix data blocks not found");

                if (!data_block_chunk->at("data_blocks").is_array())
                    throw runtime_error("jasterix data blocks is not an array");

                dec_ret = asterix_parser.decodeDataBlocks(data, data_block_chunk->at("data_blocks"), debug_);
                num_records_ += dec_ret.first;
                num_errors_ += dec_ret.second;

                if (print_)
                    loginf << data_block_chunk->dump(print_dump_indent) << logendl;

                if (data_callback)
                    data_callback(std::move(data_block_chunk), 0, dec_ret.first, dec_ret.second);
                else
                    data_block_chunk = nullptr;
            }
            catch (std::exception& e)
            {
                loginf << "jASTERIX caught exception'" << e.what() << "', breaking" << logendl;
                task->forceStop();

                while (!task->done())
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                throw;
            }
        }

        loginf << "jASTERIX decode file done" << logendl;

#if USE_BOOST
        file_.close();
#endif
    }

    size_t jASTERIX::numFrames() const
    {
        return num_frames_;
    }

    size_t jASTERIX::numRecords() const
    {
        return num_records_;
    }

    void jASTERIX::addDataBlockChunk (std::unique_ptr<nlohmann::json> data_block_chunk, bool error, bool done)
    {
        if (debug_)
        {
            loginf << "jASTERIX adding data block chunk, error " << error << " done " << done << logendl;

            if (!data_block_chunk->contains("data_blocks"))
                throw std::runtime_error ("jASTERIX scoped data block information contains no data blocks");

            if (!data_block_chunk->at("data_blocks").is_array())
                throw std::runtime_error ("jASTERIX scoped scoped data block information is not array");
        }

        if (error)
            num_errors_ += 1;

        data_block_chunks_mutex_.lock();

        data_block_chunks_.push_back(std::move(data_block_chunk));
        data_block_processing_done_ = done;

        data_block_chunks_mutex_.unlock();

        while (data_block_chunks_.size() > 5)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    void jASTERIX::addDataChunk (std::unique_ptr<nlohmann::json> data_chunk, bool done)
    {
        if (debug_)
        {
            loginf << "jASTERIX adding data chunk, done " << done << logendl;

            if (!data_chunk->contains("frames"))
                throw std::runtime_error ("jASTERIX scoped frames information contains no frames");

            if (!data_chunk->at("frames").is_array())
                throw std::runtime_error ("jASTERIX scoped frames information is not array");
        }

        data_chunks_mutex_.lock();
        data_chunks_.push_back(std::move(data_chunk));
        data_processing_done_ = done;
        data_chunks_mutex_.unlock();

        while (data_chunks_.size() > 5)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    const std::string& jASTERIX::dataBlockDefinitionPath() const
    {
        return data_block_definition_path_;
    }

    const std::string& jASTERIX::categoriesDefinitionPath() const
    {
        return categories_definition_path_;
    }

    const std::string& jASTERIX::framingsFolderPath() const
    {
        return framing_path_;
    }

    void jASTERIX::setDebug(bool debug)
    {
        debug_ = debug;
    }

    size_t jASTERIX::numErrors() const
    {
        return num_errors_;
    }

}
