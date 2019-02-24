#include "jasterix.h"
#include "logger.h"
#include "string_conv.h"

#include <boost/program_options.hpp>

#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/SimpleLayout.hh"

namespace po = boost::program_options;

using namespace std;

void test_cat048_callback (nlohmann::json&& json_data, size_t num_frames, size_t num_records);

void test_cat048 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat048 test: start";

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

    loginf << "cat048 test: src len " << strlen(cat048_ed115) << " bin len " << size;

    assert (size == 65);

    jasterix.decodeASTERIX(target, size, test_cat048_callback);


    loginf << "cat048 test: end";
}

void test_cat048_callback (nlohmann::json&& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat048 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4);

    assert (json_data["data_block"]["category"] == 48);
    assert (json_data["data_block"]["length"] == 65);

//    {
//        "data_block": {
//            "category": 48,
//            "content": {
//                "index": 3,
//                "length": 62,
//                "record": {
//                    "010": {
//                        "sac": 0,
//                        "sic": 1
//                    },
//                    "020": {
//                        "extent_present": 0,
//                        "rab": 0,
//                        "rdb": 1,
//                        "sim": 0,
//                        "spi": 0,
//                        "typ": 5
//                    },
//                    "040": {
//                        "rho": 18924,
//                        "theta": 16324
//                    },
//                    "070": {
//                        "code": 6248,
//                        "g": 0,
//                        "l": 1,
//                        "v": 0
//                    },
//                    "090": {
//                        "flight_level": 2053,
//                        "g": 1,
//                        "v": 1
//                    },
//                    "130": {
//                        "available": [
//                            false,
//                            false,
//                            false,
//                            false,
//                            false,
//                            true,
//                            false,
//                            false
//                        ],
//                        "sam": {
//                            "value": -63
//                        }
//                    },
//                    "140": {
//                        "time_of_day": 4287979
//                    },
//                    "161": {
//                        "track_number": 919
//                    },
//                    "170": {
//                        "cdm": 0,
//                        "cnf": 0,
//                        "dou": 0,
//                        "extent_present": 0,
//                        "mah": 0,
//                        "rad": 2
//                    },
//                    "200": {
//                        "calculated_groundspeed": 2108,
//                        "calculated_heading": 5936
//                    },
//                    "220": {
//                        "aircraft_address": 11226301
//                    },
//                    "240": {
//                        "c1": 8,
//                        "c2": 7,
//                        "c3": 33,
//                        "c4": 33,
//                        "c5": 45,
//                        "c6": 25,
//                        "c7": 17,
//                        "c8": 9
//                    },
//                    "250": {
//                        "mode_s_mb_data": [
//                            {
//                                "bds1": 6,
//                                "bds2": 0,
//                                "mb_data": "i9nrL7/kAA=="
//                            },
//                            {
//                                "bds1": 5,
//                                "bds2": 0,
//                                "mb_data": "gJGfOaAE3Q=="
//                            },
//                            {
//                                "bds1": 4,
//                                "bds2": 0,
//                                "mb_data": "yEgAMKgAAA=="
//                            }
//                        ],
//                        "rep": 3
//                    },
//                    "fspec": [
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        true,
//                        false,
//                        true,
//                        true,
//                        true,
//                        false,
//                        false,
//                        false,
//                        false,
//                        false,
//                        false,
//                        true,
//                        false
//                    ]
//                }
//            },
//            "length": 65
//        }
    //    ; FSPEC: 0x ff f7 02

    //111111111111011100000010

    assert (json_data["data_block"]["content"]["record"]["fspec"].size() == 3*8);

//    std::vector<bool> ref ({1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,0});
//    std::vector<bool> tmp = json_data["data_block"]["content"]["record"]["fspec"];

//    for (size_t cnt=0; cnt < 3*8; ++cnt)
//        loginf << "cnt " << cnt << " tmp " << tmp.at(cnt) << " ref " << ref.at(cnt);

    assert (json_data["data_block"]["content"]["record"]["fspec"]
            == std::vector<bool>({1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,0}));

    //    ; Data Record:
    //    ;  I048/010: =0x 00 01
    //    ;  Data Source Identifier: 0x0001 (SAC=0; SIC=1)
    //    ;  I048/140: =0x 41 6d eb
    //    ;  Time of Day: 0x416deb (4287979; 33499.835938 secs; 09:18:19.836 UTC)
    //    ;  I048/020: =0x a8
    //    ;  Target Report Descriptor: TYP='Single ModeS Roll-Call' ACT RDP-2
    //    ;  I048/040: =0x 49 ec 3f c4
    //    ;  Measured Position: srg=18924 (73.922 nmi); azm=16324 (89.670 deg)
    //    ;  I048/070: =0x 21 38
    //    ;  Mode 3/A Code: v=0; g=0; l=1; code=00470
    //    ;  I048/090: =0x 05 c8
    //    ;  Flight Level: v=0; g=0; code=1480 (370.00 FL)
    //    ;  I048/130: 0x 20 c1
    //    ;  Radar Plot Characteristics:
    //    ;   Amplitude of (M)SSR reply: -63 dBm
    //    ;  I048/220: =0x ab 4c bd
    //    ;  Aircraft Address: 0xab4cbd (11226301)
    //    ;  I048/240: =0x 49 94 b5 61  78 20
    //    ;  Aircraft Identification: "RYR5XW  "
    //    ;  I048/250: =0x 03 8b d9 eb  2f bf e4 00  60 80 91 9f  39 a0 04 dd
    //    ;            +0x 50 c8 48 00  30 a8 00 00  40
    //    ;  Mode S MB Data:
    //    ;   BDS 6,0 data=0x 8b d9 eb 2f bf e4 00
    //    ;   BDS 5,0 data=0x 80 91 9f 39 a0 04 dd
    //    ;   BDS 4,0 data=0x c8 48 00 30 a8 00 00
    //    ;           mcp_fcu_selected_altitude=37008[ft]
    //    ;  I048/161: =0x 03 97
    //    ;  Track Number: num=919
    //    ;  I048/200: =0x 08 3c 17 30
    //    ;  Calculated Track Velocity: spd=2108 (463.184 kts); hdg=5936 (32.607 deg)
    //    ;  I048/170: =0x 40
    //    ;  Track Status: CNF SRT LVL
    //    ;  I048/230: =0x 20 fd
    //    ;  Communications Capability: CC=1; FS=0 (airborne); MSSC; ARC=25ft; AIC; B1A=1; B1B=13
}


int main (int argc, char **argv)
{
    static_assert (sizeof(size_t) >= 8, "code requires size_t with at least 8 bytes");

    // setup logging
    log4cpp::Appender *console_appender_ = new log4cpp::OstreamAppender("console", &std::cout);
    console_appender_->setLayout(new log4cpp::SimpleLayout());

    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::INFO);
    root.addAppender(console_appender_);

    std::string definition_path;

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
            loginf << desc;
            return 1;
        }
    }
    catch (exception& e)
    {
        logerr << "jASTERIX test: unable to parse command line parameters: \n" << e.what();
        return -1;
    }

    try
    {
        loginf << "jASTERIX test: startup with definition_path '" << definition_path << "'";

        jASTERIX::jASTERIX asterix (definition_path, true, true);

        test_cat048(asterix);

    }
    catch (exception &ex)
    {
        logerr << "jASTERIX test: caught exception: " << ex.what();

        return -1;
    }
    catch(...)
    {
        logerr << "jASTERIX test: caught exception";

        return -1;
    }

    loginf << "jASTERIX: shutdown";

    return 0;
}
