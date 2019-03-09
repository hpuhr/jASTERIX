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
#include "string_conv.h"

#include <cmath>

void test_cat020_callback (nlohmann::json& json_data, size_t num_frames, size_t num_records)
{
    loginf << "cat020 test: decoded " << num_frames << " frames, " << num_records << " records: " << json_data.dump(4)
               << logendl;

    loginf << "cat020 test: data block" << logendl;

    assert (json_data["data_block"]["category"] == 20);
    assert (json_data["data_block"]["length"] == 101);

//    {
//        "data_block": {
//            "category": 20,
//            "content": {
//                "index": 3,
//                "length": 98,
//                "records": [
//                    {
//                        "010": {
//                            "sac": 0,
//                            "sic": 2
//                        },
//                        "020": {
//                            "chn": 0,
//                            "crt": 0,
//                            "dme": 0,
//                            "fx": 1,
//                            "fx2": 0,
//                            "gbs": 0,
//                            "hf": 0,
//                            "ms": 1,
//                            "ot": 0,
//                            "rab": 0,
//                            "sim": 0,
//                            "spi": 0,
//                            "ssr": 0,
//                            "tst": 0,
//                            "uat": 0,
//                            "vdl4": 0
//                        },
//                        "041": {
//                            "latitude_in_wgs-84": 47.88232132925,
//                            "longitude_in_wgs-84": 16.32056296698
//                        },
//                        "042": {
//                            "x": 173529.5,
//                            "y": 45109.0
//                        },
//                        "070": {
//                            "g": 0,
//                            "l": 1,
//                            "mode-3/a": 7000,
//                            "v": 0
//                        },
//                        "090": {
//                            "flight_level": 11.25,
//                            "g": 0,
//                            "v": 0
//                        },
//                        "140": {
//                            "time_of_day": 33502.7109375
//                        },
//                        "161": {
//                            "track_number": 3528
//                        },
//                        "170": {
//                            "cdm": 3,
//                            "cnf": 0,
//                            "cst": 0,
//                            "fx": 0,
//                            "mah": 0,
//                            "sth": 0,
//                            "tre": 0
//                        },
//                        "202": {
//                            "v_x": -13.75,
//                            "v_y": -9.25
//                        },
//                        "210": {
//                            "ax": 0.0,
//                            "ay": 0.0
//                        },
//                        "220": {
//                            "target_address": 148527
//                        },
//                        "230": {
//                            "aic": 0,
//                            "arc": 1,
//                            "b1a": 0,
//                            "b1b": 0,
//                            "com": 1,
//                            "mssc": 0,
//                            "stat": 0
//                        },
//                        "250": {
//                            "mode_s_mb_data": [
//                                {
//                                    "bds1": 1,
//                                    "bds2": 0,
//                                    "mb_data": "10000000a00000"
//                                },
//                                {
//                                    "bds1": 1,
//                                    "bds2": 7,
//                                    "mb_data": "00000000000000"
//                                }
//                            ],
//                            "rep": 2
//                        },
//                        "400": {
//                            "contributing_receivers": [
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 16
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 32
//                                },
//                                {
//                                    "rux": 0
//                                },
//                                {
//                                    "rux": 34
//                                }
//                            ],
//                            "rep": 16
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
//                            false,
//                            true,
//                            false,
//                            false,
//                            true,
//                            false,
//                            true,
//                            false,
//                            false,
//                            false,
//                            true,
//                            true,
//                            true,
//                            true,
//                            false,
//                            false,
//                            false,
//                            false,
//                            true,
//                            false,
//                            false
//                        ]
//                    }
//                ]
//            },
//            "length": 101
//        }
//    }


    loginf << "cat020 test: num records" << logendl;
    assert (json_data.at("data_block").at("content").at("records").size() == 1);

    //    ; FSPEC: 0x ff e9 47 84
    // 11111111111010010100011110000100

    loginf << "cat020 test: fspec" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("fspec").size() == 4*8);

    assert (json_data.at("data_block").at("content").at("records")[0].at("fspec")
            == std::vector<bool>({1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,0,0,0,1,0,0}));

    //    ;  I020/010: =0x 00 02
    //    ;  Data Source Identifier: 0x0002 (SAC=0; SIC=2)

    loginf << "cat020 test: 010" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("010").at("sac") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("010").at("sic") == 2);

    //    ;  I020/020: =0x 41 00
    //    ;  Target Report Descriptor: TYP=32 (MS); CHN=1

    loginf << "cat020 test: 020" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("ssr") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("ms") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("hf") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("vdl4") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("uat") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("dme") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("ot") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("fx") == 1);

    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("rab") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("spi") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("chn") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("gbs") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("crt") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("sim") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("tst") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("020").at("fx2") == 0);

    //    ;  I020/140: =0x 41 6f 5b
    //    ;  Time of Day: 0x416f5b (4288347; 09:18:22.711 UTC)

    loginf << "cat020 test: 140" << logendl;
    double tmp_d = json_data.at("data_block").at("content").at("records")[0].at("140").at("time_of_day");
    assert (fabs(tmp_d-33502.7109375) < 10e-6);

    //    ;  I020/041: =0x 00 88 32 e5  00 2e 6c 4a
    //    ;  Position in WGS-84 Coordinates:
    //    ;   lat=8925925 (47:52:56.615N); lon=3042378 (016:19:14.115E)

    loginf << "cat020 test: 041" << logendl;
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("041").at("latitude_in_wgs-84");
    assert (fabs(tmp_d-47.8823930025) < 10e-4);
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("041").at("longitude_in_wgs-84");
    assert (fabs(tmp_d-16.3205873966) < 10e-4);

    //    ;  I020/042: =0x 05 4b b3 01  60 6a
    //    ;  Position in Cartesian coordinates:
    //    ;   x=347059 (173529.5 mtr =93.698 nmi); y=90218 (45109.0 mtr =24.357 nmi)

    loginf << "cat020 test: 042" << logendl;
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("042").at("x");
    assert (fabs(tmp_d - 173529.5) < 10e-1);
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("042").at("y");
    assert (fabs(tmp_d - 45109.0) < 10e-1);

    //    ;  I020/161: =0x 0d c8
    //    ;  Track Number: 3528

    loginf << "cat020 test: 161" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("161").at("track_number") == 3528);

    //    ;  I020/170: =0x 18
    //    ;  Track Status: CNF CDM=3

    loginf << "cat020 test: 170" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("cnf") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("tre") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("cst") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("cdm") == 3);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("mah") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("sth") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("170").at("fx") == 0);

    //    ;  I020/070: =0x 2e 00
    //    ;  Mode 3/A Code: v=0; g=0; l=1; code=07000

    loginf << "cat020 test: 070" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("v") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("g") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("l") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("070").at("mode-3/a") == 7000);

    //    ;  I020/202: =0x ff c9 ff db
    //    ;  Calculated Track Velocity: vx=-55 (-13.75 m/s); vy=-37 (-9.25 m/s)

    loginf << "cat020 test: 202" << logendl;
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("202").at("v_x");
    assert (fabs(tmp_d - -13.75) < 10e-3);
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("202").at("v_y");
    assert (fabs(tmp_d - -9.25) < 10e-3);

    //    ;  I020/090: =0x 00 2d
    //    ;  Flight Level: v=0; g=0; code=45 (11.25 FL)

    loginf << "cat020 test: 090" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("v") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("g") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("090").at("flight_level") == 11.25);

    //    ;  I020/220: =0x 02 44 2f
    //    ;  Target Address: 0x02442f (148527)

    loginf << "cat020 test: 220" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("220").at("target_address") == 148527);

    //    ;  I020/210: =0x 00 00
    //    ;  Calculated Acceleration: ax=0 (0.00 m/s**2); ay=0 (0.00 m/s**2)

    loginf << "cat020 test: 210" << logendl;
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("210").at("ax");
    assert (fabs(tmp_d - 0.0) < 10e-3);
    tmp_d = json_data.at("data_block").at("content").at("records")[0].at("210").at("ay");
    assert (fabs(tmp_d - 0.0) < 10e-3);

    //    ;  I020/400: =0x 10 00 00 00  00 00 00 00  00 00 00 10  00 00 20 00
    //    ;            +0x 22
    //    ;  Contributing Receivers: 0x 00 00 00 00 00 00 00 00 00 00 10 00 00 20 00 22

    loginf << "cat020 test: 400" << logendl;

    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers").size() == 16);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[0].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[1].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[2].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[3].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[4].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[5].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[6].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[7].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[8].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[9].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[10].at("rux") == 16);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[11].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[12].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[13].at("rux") == 32);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[14].at("rux") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("400").at("contributing_receivers")[15].at("rux") == 34);

    //    ;  I020/250: =0x 02 10 00 00  00 a0 00 00  10 00 00 00  00 00 00 00
    //    ;            +0x 17
    //    ;  Mode S MB Data:
    //    ;   BDS 1,0 data=0x 10 00 00 00 a0 00 00
    //    ;   BDS 1,7 data=0x 00 00 00 00 00 00 00

    loginf << "cat020 test: 250" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].size() == 3);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].at("bds1") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].at("bds2") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[0].at("mb_data")
            == "10000000a00000");
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[1].size() == 3);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[1].at("bds1") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[1].at("bds2") == 7);
    assert (json_data.at("data_block").at("content").at("records")[0].at("250").at("mode_s_mb_data")[1].at("mb_data")
            == "00000000000000");

    //    ;  I020/230: =0x 20 40
    //    ;  Communications/ACAS Capability and Flight Status:
    //    ;   COM=1; STAT=0; ARC=1; AIC=0; B1A=0; B1B=0

    loginf << "cat020 test: 230" << logendl;
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("com") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("stat") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("arc") == 1);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("aic") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("b1a") == 0);
    assert (json_data.at("data_block").at("content").at("records")[0].at("230").at("b1b") == 0);

    //    ;  I020/RE:  0x 15 80 d0 00  12 00 0f ff  f1 00 89 00  7c ff 86 00
    //    ;            0x 35 00 53 ff  c1
    //    ;  Position Accuracy:
    //    ;   DOP of Position: x=18 (4.50); y=15 (3.75); xy=-15 (-3.75)
    //    ;   Standard Deviation of Position: x=137 (34.25 mtr); y=124 (31.00 mtr); xy=-122 (-30.50 mtr)
    //    ;   Standard Deviation of Position: lat=53 (0.000284 deg); lon=83 (0.000445 deg); cov=-63 (-0.000338 deg)
}

void test_cat020 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat020 test: start" << logendl;

    // echo -n 140065ffe9478400024100416f5b008832e5002e6c4a054bb301606a0dc8182e00ffc9ffdb002d02442f000010000000000000000000001000002000220210000000a0000010000000000000001720401580d00012000ffff10089007cff8600350053ffc1 | xxd -r -p > cat020ed1.5.bin

    const char *cat020_ed15 =
            "140065ffe9478400024100416f5b008832e5002e6c4a054bb301606a0dc8182e00ffc9ffdb002d02442f000010000000000000000000001000002000220210000000a0000010000000000000001720401580d00012000ffff10089007cff8600350053ffc1";
    char* target = new char[strlen(cat020_ed15)/2];

    size_t size = hex2bin (cat020_ed15, target);

    loginf << "cat020 test: src len " << strlen(cat020_ed15) << " bin len " << size << logendl;

    assert (size == 101);

    assert (jasterix.hasCategory("020"));
    assert (jasterix.hasEdition("020", "1.5"));
    jasterix.setEdition("020", "1.5");
    jasterix.setMapping("020", "");

    jasterix.decodeASTERIX(target, size, test_cat020_callback);

    loginf << "cat020 test: end" << logendl;
}


