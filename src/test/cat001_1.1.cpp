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
#include "string_conv.h"

#include <cmath>

void test_cat001_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat001 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4)
               << logendl;

    loginf << "cat001 test: data block" << logendl;

    assert (json_data["data_block"]["category"] == 1);
    assert (json_data["data_block"]["length"] == 20);

//    {
//        "data_block": {
//            "category": 1,
//            "content": {
//                "index": 3,
//                "length": 17,
//                "records": [
//                    {
//                        "010": {
//                            "SAC": 0,
//                            "SIC": 1
//                        },
//                        "020": {
//                            "ANT": 0,
//                            "FX": 0,
//                            "RAB": 0,
//                            "SIM": 0,
//                            "SPI": 0,
//                            "SSR/PSR": 2,
//                            "TYP": 0
//                        },
//                        "040": {
//                            "RHO": 364.96875,
//                            "THETA": 16.0125732349
//                        },
//                        "042": {
//                            "X-Component": 23.75,
//                            "Y-Component": -250.109375
//                        },
//                        "070": {
//                            "G": 0,
//                            "L": 0,
//                            "Mode-3/A reply": 0,
//                            "V": 0
//                        },
//                        "161": {
//                            "TRACK/PLOT NUMBER": 16312
//                        },
//                        "200": {
//                            "CALCULATED GROUNDSPEED": -0.9932860310999999,
//                            "CALCULATED HEADING": 257.34374988288
//                        },
//                        "FSPEC": [
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            true,
//                            false
//                        ]
//                    }
//                ]
//            },
//            "length": 20
//        }
//    }





    loginf << "cat001 test: num records" << logendl;
    assert (json_data.at("data_block").at("content").at("records").size() == 1);

    //    ; FSPEC: 0x fe
    // 11111110

    loginf << "cat001 test: fspec" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("FSPEC").size() == 8);

    assert (json_data.at("data_block").at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,1,1,1,1,1,1,0}));

    //    ; Data Record:
    //    ;  I001/010: =0x 00 01
    //    ;  Data Source Identifier: 0x0001 (SAC=0; SIC=1)

    loginf << "cat001 test: 010" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("010").at("SIC") == 1);

    //    ;  I001/020: =0x 20
    //    ;  Target Report Descriptor: PLT ACT SEC A1

    loginf << "cat001 test: 020" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("TYP") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("SIM") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("SSR/PSR") == 2);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("ANT") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("SPI") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("RAB") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("FX") == 0);

    //    ;  I001/040: =0x 3f b8 b6 7c
    //    ;  Measured Position: rng=16312 (127.438 nmi); azm=46716 (256.619 deg)
    loginf << "cat001 test: 040" << logendl;
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("040").at("RHO");
    assert (fabs(tmp_d - 73.922) < 10e-3);
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("040").at("THETA");
    assert (fabs(tmp_d - 89.670) < 10e-3);

    //    ;  I001/070: =0x 0b 63
    //    ;  Mode 3/A Code: v=0; g=0; l=0; code=05543
    //    ;  I001/090: =0x 05 f0
    //    ;  Mode C Code: v=0; g=0; code=1520 (380.00 FL)
    //    ;  I001/130: =0x c1 79 c0
    //    ;  Radar Plot Characteristics: 0x c1 79 c0
    //    ;  I001/141: =0x 6e b7
    //    ;  Truncated Time of Day: 0x6eb7
    //     [--:--:--.---] - --:--:--.--- -- 0x0001 A1 P:SSR ----- AR  256.619  127.438                       A:05543 --- C: 380    -- ;


    //    ;  I001/140: =0x 41 6d eb
    //    ;  Time of Day: 0x416deb (4287979; 33499.835938 secs; 09:18:19.836 UTC)

    loginf << "cat001 test: 140" << logendl;
    double tmp_d = json_data.at("data_block").at("content").at("records")[0].at("140").at("Time-of-Day");
    assert (fabs(tmp_d-33499.835938) < 10e-6);



    //    ;  I001/070: =0x 21 38
    //    ;  Mode 3/A Code: v=0; g=0; l=1; code=00470

    loginf << "cat001 test: 070" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("V") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("G") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("L") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("Mode-3/A reply") == 470);

    //    ;  I001/090: =0x 05 c8
    //    ;  Flight Level: v=0; g=0; code=1480 (370.00 FL)

    loginf << "cat001 test: 090" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("V") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("G") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("Flight Level") == 370.0);

    //    ;  I001/130: 0x 20 c1
    //    ;  Radar Plot Characteristics:
    //    ;   Amplitude of (M)SSR reply: -63 dBm

    loginf << "cat001 test: 130" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("130").at("available")
            == std::vector<bool>({0,0,0,0,0,1,0,0}));
    assert (json_data.at("data_block").at("content").at("records")[0].at("130").at("SAM").at("value") == -63);

    //    ;  I001/220: =0x ab 4c bd
    //    ;  Aircraft Address: 0xab4cbd (11226301)

    loginf << "cat001 test: 220" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("220").at("AIRCRAFT ADDRESS") == 11226301);

    //    ;  I001/240: =0x 49 94 b5 61  78 20
    //    ;  Aircraft Identification: "RYR5XW  "

    loginf << "cat001 test: 240" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("240").at("Aircraft Identification") == "RYR5XW  ");

    //    ;  I001/250: =0x 03 8b d9 eb  2f bf e4 00  60 80 91 9f  39 a0 04 dd
    //    ;            +0x 50 c8 48 00  30 a8 00 00  40
    //    ;  Mode S MB Data:
    //    ;   BDS 6,0 data=0x 8b d9 eb 2f bf e4 00
    //    ;   BDS 5,0 data=0x 80 91 9f 39 a0 04 dd
    //    ;   BDS 4,0 data=0x c8 48 00 30 a8 00 00
    //    ;           mcp_fcu_selected_altitude=37008[ft]

    loginf << "cat001 test: 250" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("Mode S MB Data").size() == 3);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("Mode S MB Data")[0].at("BDS1") == 6);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("Mode S MB Data")[0].at("BDS2") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("Mode S MB Data")[0].at("MB Data")
            == "8bd9eb2fbfe400");

    //    ;  I001/161: =0x 03 97
    //    ;  Track Number: num=919

    loginf << "cat001 test: 161" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("161").at("TRACK NUMBER") == 919);

    //    ;  I001/200: =0x 08 3c 17 30
    //    ;  Calculated Track Velocity: spd=2108 (463.184 kts); hdg=5936 (32.607 deg)

    loginf << "cat001 test: 200" << logendl;
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("200").at("CALCULATED GROUNDSPEED");
    assert (fabs(tmp_d - 463.184) < 10e-3);
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("200").at("CALCULATED HEADING");
    assert (fabs(tmp_d - 32.607) < 10e-3);

    //    ;  I001/170: =0x 40
    //    ;  Track Status: CNF SRT LVL

    loginf << "cat001 test: 170" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("CNF") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("RAD") == 2);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("DOU") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("MAH") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("CDM") == 0);

    //    ;  I001/230: =0x 20 fd
    //    ;  Communications Capability: CC=1; FS=0 (airborne); MSSC; ARC=25ft; AIC; B1A=1; B1B=13

    loginf << "cat001 test: 230" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("COM") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("STAT") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("MSSC") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("ARC") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("AIC") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("B1A") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("B1B") == 13);
}

void test_cat001 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat001 test: start" << logendl;

    //      010014fe0001203fb8b67c0b6305f0c179c06eb7
    // echo -n 010014fe0001203fb8b67c0b6305f0c179c06eb7 | xxd -r -p > cat001ed1.1.bin

    const char *cat001_ed11 = "010014fe0001203fb8b67c0b6305f0c179c06eb7";
    char* target = new char[strlen(cat001_ed11)/2];

    size_t size = hex2bin (cat001_ed11, target);

    loginf << "cat001 test: src len " << strlen(cat001_ed11) << " bin len " << size << logendl;

    assert (size == 20);

    assert (jasterix.hasCategory("001"));
    assert (jasterix.hasEdition("001", "1.1"));
    jasterix.setEdition("001", "1.1");
    jasterix.setMapping("001", "");

    jasterix.decodeASTERIX(target, size, test_cat001_callback);

    loginf << "cat001 test: end" << logendl;
}


