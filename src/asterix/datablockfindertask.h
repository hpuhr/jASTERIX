#ifndef DATABLOCKFINDERTASK_H
#define DATABLOCKFINDERTASK_H

#include <tbb/tbb.h>
#include <future>

#include <exception>

#include "asterixparser.h"
#include "jasterix.h"
#include "logger.h"

namespace jASTERIX
{
class DataBlockFinderTask // : public tbb::task
{
  public:
    DataBlockFinderTask(jASTERIX& jasterix, ASTERIXParser& asterix_parser, const char* data,
                        size_t index, size_t size, bool debug)
        : jasterix_(jasterix),
          asterix_parser_(asterix_parser),
          data_(data),
          index_(index),
          size_(size),
          debug_(debug)
    {
    }

    void start()
    {
        //tbb::task_group g;

        //tbb::task_group g_;

        pending_future_ = std::async(std::launch::async, [&] {
            size_t parsed_bytes{0};
            size_t num_data_blocks{0};

            while (!force_stop_ && !done_)  // || size_-index_ > 0
            {
                std::unique_ptr<nlohmann::json> jdata{new nlohmann::json()};

                try
                {
                    std::tuple<size_t, size_t, bool, bool> ret = asterix_parser_.findDataBlocks(
                                data_, index_ + parsed_bytes, size_ - parsed_bytes, jdata.get(), debug_);

                    parsed_bytes += std::get<0>(ret);
                    num_data_blocks += std::get<1>(ret);
                    error_ += std::get<2>(ret);
                    done_ = std::get<3>(ret);

                    loginf << "DataBlockFinderTask: ex pb " << parsed_bytes << " num db "
                           << num_data_blocks << " done " << done_ << logendl;

                    jasterix_.addDataBlockChunk(std::move(jdata), error_, done_);
                }
                catch (std::exception& e)
                {
                    break;

                    done_ = true;
                    throw (e);
                }
            }

            if (force_stop_)
                done_ = true;

            // loginf << "data block finder task done " << done << logendl;

        });

//        g_.run([&] {
//            size_t parsed_bytes{0};
//            size_t num_data_blocks{0};

//            while (!force_stop_ && !done_)  // || size_-index_ > 0
//            {
//                std::unique_ptr<nlohmann::json> jdata{new nlohmann::json()};

//                std::tuple<size_t, size_t, bool, bool> ret = asterix_parser_.findDataBlocks(
//                    data_, index_ + parsed_bytes, size_ - parsed_bytes, jdata.get(), debug_);

//                parsed_bytes += std::get<0>(ret);
//                num_data_blocks += std::get<1>(ret);
//                error_ += std::get<2>(ret);
//                done_ = std::get<3>(ret);

//                //            loginf << "DataBlockFinderTask: ex pb " << parsed_bytes << " num db " <<
//                //            num_data_blocks << " done "
//                //                   << done << logendl;

//                jasterix_.addDataBlockChunk(std::move(jdata), error_, done_);
//            }

//            if (force_stop_)
//                done_ = true;

//            // loginf << "data block finder task done " << done << logendl;

//        });
    }

//    /*override*/ tbb::task* execute()
//    {
//        // loginf << "data block finder task start" << logendl;

//        size_t parsed_bytes{0};
//        size_t num_data_blocks{0};
//        while (!force_stop_ && !done_)  // || size_-index_ > 0
//        {
//            std::unique_ptr<nlohmann::json> jdata{new nlohmann::json()};

//            std::tuple<size_t, size_t, bool, bool> ret = asterix_parser_.findDataBlocks(
//                data_, index_ + parsed_bytes, size_ - parsed_bytes, jdata.get(), debug_);

//            parsed_bytes += std::get<0>(ret);
//            num_data_blocks += std::get<1>(ret);
//            error_ += std::get<2>(ret);
//            done_ = std::get<3>(ret);

//            //            loginf << "DataBlockFinderTask: ex pb " << parsed_bytes << " num db " <<
//            //            num_data_blocks << " done "
//            //                   << done << logendl;

//            jasterix_.addDataBlockChunk(std::move(jdata), error_, done_);
//        }

//        if (force_stop_)
//            done_ = true;

//        // loginf << "data block finder task done " << done << logendl;

//        return nullptr;  // or a pointer to a new task to be executed immediately
//    }

    void forceStop();

    bool done() const;

    bool error() const;

  private:
    jASTERIX& jasterix_;
    ASTERIXParser& asterix_parser_;
    const char* data_;
    size_t index_;
    size_t size_;
    bool debug_;
    bool error_{false};
    bool done_{false};
    volatile bool force_stop_{false};

    std::future<void> pending_future_;
};

void DataBlockFinderTask::forceStop() { force_stop_ = true; }

bool DataBlockFinderTask::done() const { return done_; }

bool DataBlockFinderTask::error() const { return error_; }

}  // namespace jASTERIX

#endif  // DATABLOCKFINDERTASK_H
