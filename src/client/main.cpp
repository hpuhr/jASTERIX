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
#include "logger.h"
#include "jsonwriter.h"
#include "jasterix/global.h"

#if USE_BOOST
#include <boost/program_options.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

namespace po = boost::program_options;
#else
#include <vector>
#include <algorithm>
#include <chrono>
#endif

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

#if USE_LOG4CPP
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/SimpleLayout.hh"
#endif

using namespace std;
//using namespace jASTERIX;

jASTERIX::JSONWriter* json_writer {nullptr};

void print_callback (std::unique_ptr<nlohmann::json> data_chunk, size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << data_chunk->dump(4);
}

void write_callback (std::unique_ptr<nlohmann::json> data_chunk, size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << "jASTERIX: write_callback " << num_frames << " frames, " << num_records << " records, "
           << num_errors << " errors";
    assert (json_writer);

    json_writer->write(std::move(data_chunk));
}

void test_callback (std::unique_ptr<nlohmann::json> data_chunk, size_t num_frames, size_t num_records, size_t num_errors)
{
    assert (data_chunk);
}

int main (int argc, char **argv)
{
    static_assert (sizeof(size_t) >= 8, "code requires size_t with at least 8 bytes");

    // setup logging

#if USE_LOG4CPP
    log4cpp::Appender *console_appender_ = new log4cpp::OstreamAppender("console", &std::cout);
    console_appender_->setLayout(new log4cpp::SimpleLayout());

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::INFO);
    root.addAppender(console_appender_);
#endif

    std::string filename;
    std::string framing {""};
    std::string definition_path;
    bool debug {false};
    bool debug_include_framing {false};
    bool print {false};
    std::string write_type;
    std::string write_filename;

#if USE_BOOST
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("filename", po::value<std::string>(&filename), "input file name")
        ("definition_path", po::value<std::string>(&definition_path), "path to jASTERIX definition files")
        ("framing", po::value<std::string>(&framing), "input framine format, as specified in the framing definitions."
                                                      " raw/netto is default")
        ("frame_limit", po::value<int>(&jASTERIX::frame_limit),
         "number of frames to process, default -1, use -1 to disable.")
        ("frame_chunk_size", po::value<int>(&jASTERIX::frame_chunk_size),
         "number of frames to process in one chunk, default 1000, use -1 to disable.")
        ("data_write_size", po::value<int>(&jASTERIX::data_write_size),
         "number of frame chunks to write in one file write, default 100, use -1 to disable.")
        ("debug", po::bool_switch(&debug), "print debug output")
        ("debug_include_framing", po::bool_switch(&debug_include_framing),
         "print debug output including framing, debug still has to be set, disable per default")
        ("print", po::bool_switch(&print), "print JSON output")
        ("print_indent", po::value<int>(&jASTERIX::print_dump_indent), "intendation of json print, use -1 to disable.")
        ("write_type", po::value<std::string>(&write_type),
         "optional write type, e.g. text,zip. needs write_filename.")
        ("write_filename", po::value<std::string>(&write_filename), "optional write filename, e.g. test.zip.")
    ;

    try
    {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            loginf << desc;
            return 1;
        }
    }
    catch (exception& e)
    {
        logerr << "jASTERIX: unable to parse command line parameters: \n" << e.what() << logendl;
        return -1;
    }
#else
    std::vector<std::string> arguments;
    arguments.assign(argv, argv+argc);

    if (arguments.size() == 1 || find(arguments.begin(), arguments.end(), "--help") != arguments.end())
    {
        loginf << "help:" << logendl;
        loginf << "filename (value): input filename." << logendl;
        loginf << "definition_path (value): path to jASTERIX definition files." << logendl;
        loginf << "framing (value): input framine format, as specified in the framing definitions."
                  " raw/netto is default" << logendl;
        loginf << "frame_limit: number of frames to process, default -1, use -1 to disable." << logendl;
        loginf << "frame_chunk_size: number of frames to process in one chunk, default 1000, use -1 to disable." << logendl;
        loginf << "data_write_size: number of frame chunks to write in one file write, default 100, use -1 to disable." << logendl;
        loginf << "debug: print debug output" << logendl;
        loginf << "debug_include_framing: print debug excluding framing, debug still has to be set, disabled by default" << logendl;
        loginf << "print: print JSON output" << logendl;
        loginf << "print_indent: intendation of json print, use -1 to disable." << logendl;
        loginf << "write_type (value): optional write type, e.g. text,zip. needs write_filename." << logendl;
        loginf << "write_filename (value): optional write filename, e.g. test.zip." << logendl;

        return 0;
    }

    if (find(arguments.begin(), arguments.end(), "--filename") != arguments.end())
        filename = *(find(arguments.begin(), arguments.end(), "--filename")+1);

    if (find(arguments.begin(), arguments.end(), "--definition_path") != arguments.end())
        definition_path = *(find(arguments.begin(), arguments.end(), "--definition_path")+1);

    if (find(arguments.begin(), arguments.end(), "--framing") != arguments.end())
        framing = *(find(arguments.begin(), arguments.end(), "--framing")+1);

    if (find(arguments.begin(), arguments.end(), "--frame_limit") != arguments.end())
        frame_limit = std::atoi ((find(arguments.begin(), arguments.end(), "--frame_limit")+1)->c_str());

    if (find(arguments.begin(), arguments.end(), "--frame_chunk_size") != arguments.end())
        frame_chunk_size = std::atoi ((find(arguments.begin(), arguments.end(), "--frame_chunk_size")+1)->c_str());

    if (find(arguments.begin(), arguments.end(), "--data_write_size") != arguments.end())
        data_write_size = std::atoi ((find(arguments.begin(), arguments.end(), "--data_write_size")+1)->c_str());

    if (find(arguments.begin(), arguments.end(), "--debug") != arguments.end())
        debug = true;

    if (find(arguments.begin(), arguments.end(), "--debug_include_framing") != arguments.end())
        debug_include_framing = true;

    if (find(arguments.begin(), arguments.end(), "--print") != arguments.end())
        print = true;

    if (find(arguments.begin(), arguments.end(), "--print_indent") != arguments.end())
        print_dump_indent = std::atoi ((find(arguments.begin(), arguments.end(), "--print_indent")+1)->c_str());

    if (find(arguments.begin(), arguments.end(), "--write_type") != arguments.end())
        write_type = *(find(arguments.begin(), arguments.end(), "--write_type")+1);

    if (find(arguments.begin(), arguments.end(), "--write_filename") != arguments.end())
        write_filename = *(find(arguments.begin(), arguments.end(), "--write_filename")+1);

#endif


    if (write_type.size())
    {
        if (write_type != "text" && write_type != "zip")
        {
            logerr << "jASTERIX: unknown write_type '" << write_type << "'" << logendl;
            return -1;
        }

        if (!write_filename.size())
        {
            logerr << "jASTERIX: write_type '" << write_type << "' requires write_filename to be set" << logendl;
            return -1;
        }

        if (write_type == "text")
            json_writer = new jASTERIX::JSONWriter(jASTERIX::JSON_TEXT, write_filename);
        else if (write_type == "zip")
            json_writer = new jASTERIX::JSONWriter(jASTERIX::JSON_ZIP_TEXT, write_filename);
    }

    // check if basic configuration works
    try
    {
        if (debug)
            loginf << "jASTERIX client: startup with filename '" << filename << "' framing '" << framing
                   << "' definition_path '" << definition_path << "' debug " << debug << logendl;

        jASTERIX::jASTERIX asterix (definition_path, print, debug, !debug_include_framing);

#if USE_BOOST
        boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
#else
        auto start_time = chrono::steady_clock::now();
#endif

        if (framing == "netto" || framing == "")
        {
            if (json_writer)
                asterix.decodeFile (filename, write_callback);
            else if (print)
                asterix.decodeFile (filename, print_callback);
            else
                asterix.decodeFile (filename, test_callback);
        }
        else
        {
            if (json_writer)
                asterix.decodeFile (filename, framing, write_callback);
            else if (print)
                asterix.decodeFile (filename, framing, print_callback);
            else
                asterix.decodeFile (filename, framing, test_callback);
        }

        size_t num_frames = asterix.numFrames();
        size_t num_records = asterix.numRecords();

#if USE_BOOST
        boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time() - start_time;

        string time_str = to_string(diff.hours())+"h "+to_string(diff.minutes())+"m "+to_string(diff.seconds())+"s "+
                to_string(diff.total_milliseconds()%1000)+"ms";

        double seconds = diff.total_milliseconds()/1000.0;

        //if (debug)
            loginf << "jASTERIX client: decoded " << num_frames << " frames, "
                   << num_records << " records in " << time_str << ": "
                   << num_frames/seconds << " fr/s, " << num_records/seconds << " rec/s" << logendl;
#else
        auto end_time = chrono::steady_clock::now();
        double full_seconds = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()/1000.0;

        unsigned int hours = full_seconds/3600;
        unsigned int minutes = (full_seconds-hours*3600)/60;
        double seconds = full_seconds-hours*3600-minutes*60;

        string time_str = to_string(hours)+"h "+to_string(minutes)+"m "+to_string(seconds)+"s";

        loginf << "jASTERIX client: decoded " << num_frames << " frames, "
               << num_records << " records in " << time_str << ": "
               << num_frames/full_seconds << " fr/s, " << num_records/full_seconds << " rec/s" << logendl;
#endif
    }
    catch (exception &ex)
    {
        logerr << "jASTERIX: caught exception: " << ex.what() << logendl;

        //assert (false);

        return -1;
    }
    catch(...)
    {
        logerr << "jASTERIX: caught exception" << logendl;

        //assert (false);

        return -1;
    }

    if (json_writer)
        delete json_writer;

    loginf << "jASTERIX: shutdown" << logendl;

//#if USE_BOOST
//    std::this_thread::sleep_for(std::chrono::seconds(15));
//#endif
    return 0;
}
