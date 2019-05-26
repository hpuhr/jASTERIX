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


#ifndef FRAMEPARSERTASK_H
#define FRAMEPARSERTASK_H

#include "jasterix.h"
#include "frameparser.h"
#include "logger.h"

#include <tuple>

#include <tbb/tbb.h>

#include <exception>

namespace jASTERIX
{

class FrameParserTask : public tbb::task
{
public:
    FrameParserTask (jASTERIX& jasterix, FrameParser& frame_parser, const nlohmann::json& header,
                     const char* data, size_t index, size_t size, bool debug)
        : jasterix_(jasterix), frame_parser_(frame_parser), header_(header), data_(data), index_(index), size_(size),
          debug_(debug)
    {}

    /*override*/ tbb::task* execute()
    {
        //size_t num_parsed_bytes {0};
        //size_t num_parsed_chunks {0};

        while (!force_stop_ && !done_) // || size_-index_ > 0
        {
            nlohmann::json data_chunk;

            if (frame_parser_.hasFileHeaderItems())
                data_chunk = header_; // copy header

            //loginf << "FPT UGA index " << index_ << " size " << size_ << logendl;

            assert (index_ < size_);

            std::tuple<size_t, size_t, bool> ret = frame_parser_.findFrames(data_, index_, size_, data_chunk, debug_);

            index_ += std::get<0>(ret); // parsed bytes
            done_ = std::get<2>(ret); // done flag

//            loginf << "FPT UGA index " << index_ << " done " << done << " chunk '" << data_chunk.dump(4) << "'"
//                   << logendl;

            assert (data_chunk != nullptr);

            if (data_chunk.find("frames") == data_chunk.end())
                throw std::runtime_error ("jASTERIX scoped frames information contains no frames");

            if (!data_chunk.at("frames").is_array())
                throw std::runtime_error ("jASTERIX scoped frames information is not array");

            jasterix_.addDataChunk(data_chunk, done_);
        }

        if (force_stop_)
            done_ = true;

        return nullptr; // or a pointer to a new task to be executed immediately
    }

    void forceStop();
    bool done() const;

private:
    jASTERIX& jasterix_;
    FrameParser& frame_parser_;
    const nlohmann::json& header_;
    const char* data_;
    size_t index_;
    size_t size_;
    bool debug_;
    bool done_ {false};
    volatile bool force_stop_ {false};
};

void FrameParserTask::forceStop()
{
    force_stop_ = true;
}

bool FrameParserTask::done() const
{
return done_;
}


}

#endif // FRAMEPARSERTASK_H
