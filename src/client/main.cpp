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

#include "jasterix.h"
#include "logger.h"
#include "jsonwriter.h"
#include "global.h"

#if USE_BOOST
#include <boost/program_options.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

namespace po = boost::program_options;
#else
#include <vector>
#include <algorithm>
#endif

#include <iostream>
#include <cstdlib>

#if USE_LOG4CPP
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/SimpleLayout.hh"
#endif


using namespace std;
using namespace jASTERIX;

JSONWriter* json_writer {nullptr};

void callback (nlohmann::json& data_chunk, size_t num_frames, size_t num_records)
{
    loginf << "jASTERIX: decoded " << num_frames << " frames, " << num_records << " records: "
           << data_chunk.dump(print_dump_indent);
}

void write_callback (nlohmann::json& data_chunk, size_t num_frames, size_t num_records)
{
    //loginf << "jASTERIX: decoded " << num_frames << " frames, " << num_records << " records: " << data_chunk.dump(4);
    assert (json_writer);

    json_writer->write(data_chunk);
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
    std::string framing {"netto"};
    std::string definition_path;
    bool debug {false};
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
                                                      " netto is default")
        ("debug", po::bool_switch(&debug), "print debug output")
        ("print", po::bool_switch(&print), "print JSON output")
        ("print_indent", po::value<int>(&print_dump_indent), "intendation of json print, use -1 to disable.")
        ("write_type", po::value<std::string>(&write_type), "optional write type, e.g. text,zip. needs write_filename.")
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
                  " netto is default" << logendl;
        loginf << "debug: print debug output" << logendl;
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

    if (find(arguments.begin(), arguments.end(), "--debug") != arguments.end())
        debug = true;

    if (find(arguments.begin(), arguments.end(), "--print") != arguments.end())
        print = true;

    if (find(arguments.begin(), arguments.end(), "--print_indent") != arguments.end())
        print_dump_indent = std::atoi (*(find(arguments.begin(), arguments.end(), "--print_indent")+1));

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
            json_writer = new JSONWriter(JSON_TEXT, write_filename, 100);
        else if (write_type == "zip")
            json_writer = new JSONWriter(JSON_ZIP_TEXT, write_filename, 100);
    }

    // check if basic configuration works
    try
    {
        if (debug)
            loginf << "jASTERIX client: startup with filename '" << filename << "' framing '" << framing
                   << "' definition_path '" << definition_path << "' debug " << debug << logendl;

        jASTERIX::jASTERIX asterix (definition_path, print, debug);

#if USE_BOOST
        boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
#endif

        if (json_writer)
            asterix.decodeFile (filename, framing, write_callback);
        else
            asterix.decodeFile (filename, framing, callback);

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

    return 0;
}
