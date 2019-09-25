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

#if USE_LOG4CPP
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/SimpleLayout.hh"
#endif

#if USE_BOOST
#include <boost/program_options.hpp>

namespace po = boost::program_options;
#endif

using namespace std;
//using namespace jASTERIX;

void test_cat001_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat001 (jASTERIX::jASTERIX& jasterix);

void test_cat002_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat002 (jASTERIX::jASTERIX& jasterix);

void test_cat019_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat019 (jASTERIX::jASTERIX& jasterix);

void test_cat020_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat020 (jASTERIX::jASTERIX& jasterix);

void test_cat034_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat034 (jASTERIX::jASTERIX& jasterix);

void test_cat048_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat048 (jASTERIX::jASTERIX& jasterix);

void test_cat021_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat021 (jASTERIX::jASTERIX& jasterix);

void test_cat062_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat062_112 (jASTERIX::jASTERIX& jasterix);

void test_cat062_116_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat062_116 (jASTERIX::jASTERIX& jasterix);

void test_cat063_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat063 (jASTERIX::jASTERIX& jasterix);

void test_cat065_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);
void test_cat065 (jASTERIX::jASTERIX& jasterix);

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

    std::string definition_path;

#if USE_BOOST
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("definition_path", po::value<std::string>(&definition_path), "path to jASTERIX definition files")
            ;

    try
    {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            loginf << desc << logendl;
            return 1;
        }
    }
    catch (exception& e)
    {
        logerr << "jASTERIX test: unable to parse command line parameters: \n" << e.what() << logendl;
        return -1;
    }
#else
    std::vector<std::string> arguments;
    arguments.assign(argv, argv+argc);

    if (arguments.size() == 1 || find(arguments.begin(), arguments.end(), "--help") != arguments.end())
    {
        loginf << "help:" << logendl;
        loginf << "definition_path (value): path to jASTERIX definition files." << logendl;

        return 0;
    }

    if (find(arguments.begin(), arguments.end(), "--definition_path") != arguments.end())
        definition_path = *(find(arguments.begin(), arguments.end(), "--definition_path")+1);

#endif

    try
    {
        loginf << "jASTERIX test: startup with definition_path '" << definition_path << "'" << logendl;

        //(const std::string& definition_path, bool print, bool debug, bool debug_exclude_framing)
        jASTERIX::jASTERIX asterix (definition_path, true, true, false);

        loginf << "jASTERIX test: CAT 001 test" << logendl;
        test_cat001(asterix);
        loginf << "jASTERIX test: CAT 002 test" << logendl;
        test_cat002(asterix);
        loginf << "jASTERIX test: CAT 019 test" << logendl;
        test_cat019(asterix);
        loginf << "jASTERIX test: CAT 020 test" << logendl;
        test_cat020(asterix);
        loginf << "jASTERIX test: CAT 021 test" << logendl;
        test_cat021(asterix);
        loginf << "jASTERIX test: CAT 034 test" << logendl;
        test_cat034(asterix);
        loginf << "jASTERIX test: CAT 048 test" << logendl;
        test_cat048(asterix);
        loginf << "jASTERIX test: CAT 062 test" << logendl;
        test_cat062_112(asterix);
        test_cat062_116(asterix);
        loginf << "jASTERIX test: CAT 063 test" << logendl;
        test_cat063(asterix);
        loginf << "jASTERIX test: CAT 065 test" << logendl;
        test_cat065(asterix);

    }
    catch (exception &ex)
    {
        logerr << "jASTERIX test: caught exception: " << ex.what() << logendl;

        return -1;
    }
    catch(...)
    {
        logerr << "jASTERIX test: caught exception" << logendl;

        return -1;
    }

    loginf << "jASTERIX test: shutdown" << logendl;

    return 0;
}
