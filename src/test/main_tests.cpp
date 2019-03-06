#include "jasterix.h"
#include "logger.h"
#include "string_conv.h"

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
using namespace jASTERIX;

void test_cat048_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records);

void test_cat048 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat048 test: start" << logendl;

    //    ; ASTERIX data block at pos 0: cat=48; len=65
    //      0x 30 00 41 ff  f7 02 00 01  41 6d eb a8  49 ec 3f c4
    //      0x 21 38 05 c8  20 c1 ab 4c  bd 49 94 b5  61 78 20 03
    //      0x 8b d9 eb 2f  bf e4 00 60  80 91 9f 39  a0 04 dd 50
    //      0x c8 48 00 30  a8 00 00 40  03 97 08 3c  17 30 40 20
    //      0x fd

    // echo -n 300041fff7020001416deba849ec3fc4213805c820c1ab4cbd4994b5617820038bd9eb2fbfe400608091
    //9f39a004dd50c8480030a80000400397083c17304020fd | xxd -r -p > cat048ed1.15.bin

    const char *cat048_ed115 = "300041fff7020001416deba849ec3fc4213805c820c1ab4cbd4994b5617820038bd9eb2fbfe400608091"
                               "9f39a004dd50c8480030a80000400397083c17304020fd";
    char* target = new char[strlen(cat048_ed115)/2];

    size_t size = hex2bin (cat048_ed115, target);

    loginf << "cat048 test: src len " << strlen(cat048_ed115) << " bin len " << size << logendl;

    assert (size == 65);

    assert (jasterix.hasCategory("048"));
    assert (jasterix.hasEdition("048", "1.15"));
    jasterix.setEdition("048", "1.15");
    jasterix.setMapping("048", "");

    jasterix.decodeASTERIX(target, size, test_cat048_callback);

    loginf << "cat048 test: end" << logendl;
}

void test_cat048_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat048 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4)
               << logendl;

    loginf << "cat048 test: data block" << logendl;

    assert (json_data["data_block"]["category"] == 48);
    assert (json_data["data_block"]["length"] == 65);

//    {
//        "data_block": {
//            "category": 48,
//            "content": {
//                "index": 3,
//                "length": 62,
//                "records": [
//                    {
//                        "010": {
//                            "sac": 0,
//                            "sic": 1
//                        },
//                        "020": {
//                            "fx": 0,
//                            "rab": 0,
//                            "rdp": 1,
//                            "sim": 0,
//                            "spi": 0,
//                            "typ": 5
//                        },
//                        "040": {
//                            "rho": 18924,
//                            "theta": 16324
//                        },
//                        "070": {
//                            "g": 0,
//                            "l": 1,
//                            "mode-3/a": 470,
//                            "v": 0
//                        },
//                        "090": {
//                            "flight_level": 1480,
//                            "g": 0,
//                            "v": 0
//                        },
//                        "130": {
//                            "available": [
//                                false,
//                                false,
//                                false,
//                                false,
//                                false,
//                                true,
//                                false,
//                                false
//                            ],
//                            "sam": {
//                                "value": -63
//                            }
//                        },
//                        "140": {
//                            "time-of-day": 4287979
//                        },
//                        "161": {
//                            "track_number": 919
//                        },
//                        "170": {
//                            "cdm": 0,
//                            "cnf": 0,
//                            "dou": 0,
//                            "fx": 0,
//                            "mah": 0,
//                            "rad": 2
//                        },
//                        "200": {
//                            "calculated_groundspeed": 2108,
//                            "calculated_heading": 5936
//                        },
//                        "220": {
//                            "aircraft_address": 11226301
//                        },
//                        "230": {
//                            "aic": 1,
//                            "arc": 1,
//                            "b1a": 1,
//                            "b1b": 13,
//                            "com": 1,
//                            "mssc": 1,
//                            "stat": 0
//                        },
//                        "240": {
//                            "aircraft_identification": "RYR5XW  "
//                        },
//                        "250": {
//                            "mode_s_mb_data": [
//                                {
//                                    "bds1": 6,
//                                    "bds2": 0,
//                                    "mb_data": "8bd9eb2fbfe400"
//                                },
//                                {
//                                    "bds1": 5,
//                                    "bds2": 0,
//                                    "mb_data": "80919f39a004dd"
//                                },
//                                {
//                                    "bds1": 4,
//                                    "bds2": 0,
//                                    "mb_data": "c8480030a80000"
//                                }
//                            ],
//                            "rep": 3
//                        },
//                        "fspec": [
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            false,
//                            true,
//                            true,
//                            true,
//                            false,
//                            false,
//                            false,
//                            false,
//                            false,
//                            false,
//                            true,
//                            false
//                        ]
//                    }
//                ]
//            },
//            "length": 65
//        }
//    }
//   }

    loginf << "cat048 test: num records" << logendl;
    assert (json_data.at("data_block").at("content").at("records").size() == 1);

    //    ; FSPEC: 0x ff f7 02
    // 111111111111011100000010

    loginf << "cat048 test: fspec" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("fspec").size() == 3*8);

//    std::vector<bool> ref ({1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,0});
//    std::vector<bool> tmp = json_data["data_block"]["content"]["record"]["fspec"];

//    for (size_t cnt=0; cnt < 3*8; ++cnt)
//        loginf << "cnt " << cnt << " tmp " << tmp.at(cnt) << " ref " << ref.at(cnt);

    assert (json_data.at("data_block").at("content").at("records")[0].at("fspec")
            == std::vector<bool>({1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,0}));

    //    ; Data Record:
    //    ;  I048/010: =0x 00 01
    //    ;  Data Source Identifier: 0x0001 (SAC=0; SIC=1)

    loginf << "cat048 test: 010" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("010").at("sac") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("010").at("sic") == 1);

    //    ;  I048/140: =0x 41 6d eb
    //    ;  Time of Day: 0x416deb (4287979; 33499.835938 secs; 09:18:19.836 UTC)

    loginf << "cat048 test: 140" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("140").at("time-of-day") == 4287979);

    //    ;  I048/020: =0x a8
    //    ;  Target Report Descriptor: TYP='Single ModeS Roll-Call' ACT RDP-2

    loginf << "cat048 test: 020" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("typ") == 5);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("sim") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("rdp") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("spi") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("rab") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("fx") == 0);

    //    ;  I048/040: =0x 49 ec 3f c4
    //    ;  Measured Position: srg=18924 (73.922 nmi); azm=16324 (89.670 deg)

    loginf << "cat048 test: 040" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("040").at("rho") == 18924);
    assert (json_data.at("data_block").at("content").at("records")[0].at("040").at("theta") == 16324);


    //    ;  I048/070: =0x 21 38
    //    ;  Mode 3/A Code: v=0; g=0; l=1; code=00470

    loginf << "cat048 test: 070" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("v") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("g") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("l") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("mode-3/a") == 470);

    //    ;  I048/090: =0x 05 c8
    //    ;  Flight Level: v=0; g=0; code=1480 (370.00 FL)

    loginf << "cat048 test: 090" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("v") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("g") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("flight_level") == 1480);

    //    ;  I048/130: 0x 20 c1
    //    ;  Radar Plot Characteristics:
    //    ;   Amplitude of (M)SSR reply: -63 dBm

    loginf << "cat048 test: 130" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("130").at("available")
            == std::vector<bool>({0,0,0,0,0,1,0,0}));
    assert (json_data.at("data_block").at("content").at("records")[0].at("130").at("sam").at("value") == -63);

    //    ;  I048/220: =0x ab 4c bd
    //    ;  Aircraft Address: 0xab4cbd (11226301)

    loginf << "cat048 test: 220" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("220").at("aircraft_address") == 11226301);

    //    ;  I048/240: =0x 49 94 b5 61  78 20
    //    ;  Aircraft Identification: "RYR5XW  "

    loginf << "cat048 test: 240" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("240").at("aircraft_identification") == "RYR5XW  ");

    //    ;  I048/250: =0x 03 8b d9 eb  2f bf e4 00  60 80 91 9f  39 a0 04 dd
    //    ;            +0x 50 c8 48 00  30 a8 00 00  40
    //    ;  Mode S MB Data:
    //    ;   BDS 6,0 data=0x 8b d9 eb 2f bf e4 00
    //    ;   BDS 5,0 data=0x 80 91 9f 39 a0 04 dd
    //    ;   BDS 4,0 data=0x c8 48 00 30 a8 00 00
    //    ;           mcp_fcu_selected_altitude=37008[ft]

    loginf << "cat048 test: 250" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data").size() == 3);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].at("bds1") == 6);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].at("bds2") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].at("mb_data")
            == "8bd9eb2fbfe400");

    //    ;  I048/161: =0x 03 97
    //    ;  Track Number: num=919

    loginf << "cat048 test: 161" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("161").at("track_number") == 919);

    //    ;  I048/200: =0x 08 3c 17 30
    //    ;  Calculated Track Velocity: spd=2108 (463.184 kts); hdg=5936 (32.607 deg)

    loginf << "cat048 test: 200" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("200").at("calculated_groundspeed") == 2108);
    assert (json_data.at("data_block").at("content").at("records")[0].at("200").at("calculated_heading") == 5936);

    //    ;  I048/170: =0x 40
    //    ;  Track Status: CNF SRT LVL

    loginf << "cat048 test: 170" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("cnf") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("rad") == 2);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("dou") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("mah") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("cdm") == 0);

    //    ;  I048/230: =0x 20 fd
    //    ;  Communications Capability: CC=1; FS=0 (airborne); MSSC; ARC=25ft; AIC; B1A=1; B1B=13

    loginf << "cat048 test: 230" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("com") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("stat") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("mssc") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("arc") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("aic") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("b1a") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("b1b") == 13);
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

        jASTERIX::jASTERIX asterix (definition_path, true, true);

        test_cat048(asterix);

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
