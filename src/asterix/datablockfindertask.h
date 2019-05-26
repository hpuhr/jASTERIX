#ifndef DATABLOCKFINDERTASK_H
#define DATABLOCKFINDERTASK_H

#include "jasterix.h"
#include "asterixparser.h"
#include "logger.h"

#include <tbb/tbb.h>

#include <exception>

namespace jASTERIX
{

class DataBlockFinderTask : public tbb::task
{
public:
    DataBlockFinderTask (jASTERIX& jasterix, ASTERIXParser& asterix_parser, const char* data,
                         size_t index, size_t size, bool debug)
        : jasterix_(jasterix), asterix_parser_(asterix_parser), data_(data), index_(index), size_(size),
          debug_(debug)
    {}

    /*override*/ tbb::task* execute()
    {
        //loginf << "data block finder task start" << logendl;

        size_t parsed_bytes {0};
        size_t num_data_blocks {0};
        while (!force_stop_ && !done_) // || size_-index_ > 0
        {
            nlohmann::json jdata;

            std::tuple<size_t, size_t, bool> ret = asterix_parser_.findDataBlocks(data_, index_+parsed_bytes,
                                                                                  size_-parsed_bytes,
                                                                                  jdata, debug_);

            parsed_bytes += std::get<0>(ret);
            num_data_blocks += std::get<1>(ret);
            done_ = std::get<2>(ret);

//            loginf << "DataBlockFinderTask: ex pb " << parsed_bytes << " num db " << num_data_blocks << " done "
//                   << done << logendl;

            jasterix_.addDataBlockChunk(jdata, done_);
        }

        if (force_stop_)
            done_ = true;

        //loginf << "data block finder task done " << done << logendl;

        return nullptr; // or a pointer to a new task to be executed immediately
    }

    void forceStop();

    bool done() const;

private:
    jASTERIX& jasterix_;
    ASTERIXParser& asterix_parser_;
    const char* data_;
    size_t index_;
    size_t size_;
    bool debug_;
    bool done_ {false};
    volatile bool force_stop_ {false};
};

void DataBlockFinderTask::forceStop()
{
    force_stop_ = true;
}

bool DataBlockFinderTask::done() const
{
return done_;
}

}

#endif // DATABLOCKFINDERTASK_H
