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
#include "test_jasterix.h"
#include "files.h"
#include "test_jasterix.h"
#include "system.h"

#if USE_LOG4CPP
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/SimpleLayout.hh"
#endif

#if USE_BOOST
#include "boost/date_time/posix_time/posix_time.hpp"
#endif

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

using namespace std;

std::string definition_path;
std::string filename;
float start_free_ram;

void test_ram_callback (std::unique_ptr<nlohmann::json> json_data, size_t num_frames, size_t num_records,
                        size_t num_errors)
{
    float free_ram = Utils::System::getFreeRAMinGB();

    loginf << "ram test: using " << std::fixed << std::setprecision(2) << (start_free_ram-free_ram) * 1024.0 << " mb"
           << logendl;

    json_data = nullptr;
}

TEST_CASE( "jASTERIX RAM", "[jASTERIX RAM]" )
{
    loginf << "ram test: start" << logendl;

    jASTERIX::jASTERIX jasterix (definition_path, false, false, false);

    REQUIRE(jASTERIX::Files::fileExists(filename));

#if USE_BOOST
    boost::posix_time::ptime start_time = boost::posix_time::microsec_clock::local_time();
#else
    auto start_time = chrono::steady_clock::now();
#endif

    jasterix.decodeFile(filename, "ioss", test_ram_callback);

    size_t num_frames = jasterix.numFrames();
    size_t num_records = jasterix.numRecords();

#if USE_BOOST
    boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time() - start_time;

    string time_str = to_string(diff.hours())+"h "+to_string(diff.minutes())+"m "+to_string(diff.seconds())+"s "+
            to_string(diff.total_milliseconds()%1000)+"ms";

    double seconds = diff.total_milliseconds()/1000.0;

    loginf << "ram test: decoded " << num_frames << " frames, "
           << num_records << " records in " << time_str << ": "
           << num_frames/seconds << " fr/s, " << num_records/seconds << " rec/s" << logendl;
#else
    auto end_time = chrono::steady_clock::now();
    double full_seconds = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()/1000.0;

    unsigned int hours = full_seconds/3600;
    unsigned int minutes = (full_seconds-hours*3600)/60;
    double seconds = full_seconds-hours*3600-minutes*60;

    string time_str = to_string(hours)+"h "+to_string(minutes)+"m "+to_string(seconds)+"s";

    loginf << "ram test: decoded " << num_frames << " frames, "
           << num_records << " records in " << time_str << ": "
           << num_frames/full_seconds << " fr/s, " << num_records/full_seconds << " rec/s" << logendl;
#endif

    loginf << "ram test: end" << logendl;
}


int main (int argc, char **argv)
{
    static_assert (sizeof(size_t) >= 8, "code requires size_t with at least 8 bytes");

    start_free_ram = Utils::System::getFreeRAMinGB();

    // setup logging
#if USE_LOG4CPP
    log4cpp::Appender *console_appender_ = new log4cpp::OstreamAppender("console", &std::cout);
    console_appender_->setLayout(new log4cpp::SimpleLayout());

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::INFO);
    root.addAppender(console_appender_);
#endif

    Catch::Session session;

    // Build a new parser on top of Catch's
    using namespace Catch::clara;
    auto cli = session.cli() // Get Catch's composite command line parser
            | Opt( definition_path, "definition_path" ) // bind variable to a new option, with a hint string
            ["--definition_path"]    // the option names it will respond to
            ("path for definition files")
            | Opt( filename, "filename" ) // bind variable to a new option, with a hint string
            ["--filename"]    // the option names it will respond to
            ("path for file to decode");        // description string for the help output

    // Now pass the new composite back to Catch so it uses that
    session.cli(cli);

    // Let Catch (using Clara) parse the command line
    int returnCode = session.applyCommandLine (argc, argv);
    if( returnCode != 0 ) // Indicates a command line error
        return returnCode;

    if(definition_path.size())
        loginf << "definition_path: '" << definition_path << "'" << logendl;
    else
    {
        loginf << "definition_path variable missing" << logendl;
        return -1;
    }

    if(filename.size())
        loginf << "filename: '" << filename << "'" << logendl;
    else
    {
        loginf << "filename variable missing" << logendl;
        return -1;
    }

    return session.run();
}
