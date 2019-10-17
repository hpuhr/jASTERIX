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

void test_cat020_callback (std::unique_ptr<nlohmann::json> json_data, size_t num_frames, size_t num_records, size_t num_errors)
{
    loginf << "cat020 test: decoded " << num_frames << " frames, " << num_records << " records, " << num_errors
           << " errors: " << json_data->dump(4) << logendl;
    assert (num_errors == 0);

//    {
//        "data_blocks": [
//            {
//                "category": 20,
//                "content": {
//                    "index": 3,
//                    "length": 98,
//                    "records": [
//                        {
//                            "010": {
//                                "SAC": 0,
//                                "SIC": 2
//                            },
//                            "020": {
//                                "CHN": 0,
//                                "CRT": 0,
//                                "DME": 0,
//                                "FX": 1,
//                                "FX2": 0,
//                                "GBS": 0,
//                                "HF": 0,
//                                "MS": 1,
//                                "OT": 0,
//                                "RAB": 0,
//                                "SIM": 0,
//                                "SPI": 0,
//                                "SSR": 0,
//                                "TST": 0,
//                                "UAT": 0,
//                                "VDL4": 0
//                            },
//                            "041": {
//                                "Latitude": 47.88232132925,
//                                "Longitude": 16.32056296698
//                            },
//                            "042": {
//                                "X": 173529.5,
//                                "Y": 45109.0
//                            },
//                            "070": {
//                                "G": 0,
//                                "L": 1,
//                                "Mode-3/A code": 7000,
//                                "V": 0
//                            },
//                            "090": {
//                                "Flight Level": 11.25,
//                                "G": 0,
//                                "V": 0
//                            },
//                            "140": {
//                                "Time of Day": 33502.7109375
//                            },
//                            "161": {
//                                "Track Number": 3528
//                            },
//                            "170": {
//                                "CDM": 3,
//                                "CNF": 0,
//                                "CST": 0,
//                                "FX": 0,
//                                "MAH": 0,
//                                "STH": 0,
//                                "TRE": 0
//                            },
//                            "202": {
//                                "Vx": -13.75,
//                                "Vy": -9.25
//                            },
//                            "210": {
//                                "Ax": 0.0,
//                                "Ay": 0.0
//                            },
//                            "220": {
//                                "Target Address": 148527
//                            },
//                            "230": {
//                                "AIC": 0,
//                                "ARC": 1,
//                                "B1A": 0,
//                                "B1B": 0,
//                                "COM": 1,
//                                "MSSC": 0,
//                                "STAT": 0
//                            },
//                            "250": {
//                                "Mode S MB Data": [
//                                    {
//                                        "BDS1": 1,
//                                        "BDS2": 0,
//                                        "MB Data": "10000000a00000"
//                                    },
//                                    {
//                                        "BDS1": 1,
//                                        "BDS2": 7,
//                                        "MB Data": "00000000000000"
//                                    }
//                                ],
//                                "REP": 2
//                            },
//                            "400": {
//                                "Contributing Receivers": [
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 16
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 32
//                                    },
//                                    {
//                                        "RUx": 0
//                                    },
//                                    {
//                                        "RUx": 34
//                                    }
//                                ],
//                                "REP": 16
//                            },
//                            "FSPEC": [
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                true,
//                                false,
//                                true,
//                                false,
//                                false,
//                                true,
//                                false,
//                                true,
//                                false,
//                                false,
//                                false,
//                                true,
//                                true,
//                                true,
//                                true,
//                                false,
//                                false,
//                                false,
//                                false,
//                                true,
//                                false,
//                                false
//                            ],
//                            "REF": {
//                                "PA": {
//                                    "DOP": {
//                                        "DOP-x": 4.5,
//                                        "DOP-xy": -3.75,
//                                        "DOP-y": 3.75
//                                    },
//                                    "SDC": {
//                                        "COV-XY (Covariance Component)": -30.5,
//                                        "SDC (X-Component)": 34.25,
//                                        "SDC (Y-Component)": 31.0
//                                    },
//                                    "SDW": {
//                                        "COV-WGS (Lat/Long Covariance Component)": -0.00033795783,
//                                        "SDW (Latitude Component)": 0.00028431372999999997,
//                                        "SDW (Longitude Component)": 0.00044524603
//                                    },
//                                    "available": [
//                                        true,
//                                        true,
//                                        false,
//                                        true,
//                                        false,
//                                        false,
//                                        false,
//                                        false
//                                    ]
//                                },
//                                "REF_FSPEC": [
//                                    true,
//                                    false,
//                                    false,
//                                    false,
//                                    false,
//                                    false,
//                                    false,
//                                    false
//                                ]
//                            }
//                        }
//                    ]
//                },
//                "length": 101
//            }
//        ]
//    }

    loginf << "cat020 test: data block" << logendl;

    assert (json_data->find ("data_blocks") != json_data->end());
    assert (json_data->at("data_blocks").is_array());
    assert (json_data->at("data_blocks").size() == 1);
    assert (json_data->at("data_blocks")[0]["category"] == 20);
    assert (json_data->at("data_blocks")[0]["length"] == 101);

    loginf << "cat020 test: num records" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records").size() == 1);

    //    ; FSPEC: 0x ff e9 47 84
    // 11111111111010010100011110000100

    loginf << "cat020 test: fspec" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("FSPEC").size() == 4*8);

    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("FSPEC")
            == std::vector<bool>({1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,0,0,0,1,0,0}));

    //    ;  I020/010: =0x 00 02
    //    ;  Data Source Identifier: 0x0002 (SAC=0; SIC=2)

    loginf << "cat020 test: 010" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("010").at("SAC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("010").at("SIC") == 2);

    //    ;  I020/020: =0x 41 00
    //    ;  Target Report Descriptor: TYP=32 (MS); CHN=1

    loginf << "cat020 test: 020" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("SSR") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("MS") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("HF") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("VDL4") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("UAT") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("DME") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("OT") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("FX") == 1);

    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("RAB") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("SPI") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("CHN") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("GBS") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("CRT") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("SIM") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("TST") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("020").at("FX2") == 0);

    //    ;  I020/140: =0x 41 6f 5b
    //    ;  Time of Day: 0x416f5b (4288347; 09:18:22.711 UTC)

    loginf << "cat020 test: 140" << logendl;
    double tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("140").at("Time of Day");
    assert (fabs(tmp_d-33502.7109375) < 10e-6);

    //    ;  I020/041: =0x 00 88 32 e5  00 2e 6c 4a
    //    ;  Position in WGS-84 Coordinates:
    //    ;   lat=8925925 (47:52:56.615N); lon=3042378 (016:19:14.115E)

    loginf << "cat020 test: 041" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("041").at("Latitude");
    assert (fabs(tmp_d-47.8823930025) < 10e-4);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("041").at("Longitude");
    assert (fabs(tmp_d-16.3205873966) < 10e-4);

    //    ;  I020/042: =0x 05 4b b3 01  60 6a
    //    ;  Position in Cartesian coordinates:
    //    ;   x=347059 (173529.5 mtr =93.698 nmi); y=90218 (45109.0 mtr =24.357 nmi)

    loginf << "cat020 test: 042" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("042").at("X");
    assert (fabs(tmp_d - 173529.5) < 10e-1);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("042").at("Y");
    assert (fabs(tmp_d - 45109.0) < 10e-1);

    //    ;  I020/161: =0x 0d c8
    //    ;  Track Number: 3528

    loginf << "cat020 test: 161" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("161").at("Track Number") == 3528);

    //    ;  I020/170: =0x 18
    //    ;  Track Status: CNF CDM=3

    loginf << "cat020 test: 170" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("CNF") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("TRE") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("CST") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("CDM") == 3);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("MAH") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("STH") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("170").at("FX") == 0);

    //    ;  I020/070: =0x 2e 00
    //    ;  Mode 3/A Code: v=0; g=0; l=1; code=07000

    loginf << "cat020 test: 070" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("070").at("V") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("070").at("G") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("070").at("L") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("070").at("Mode-3/A code") == 7000);

    //    ;  I020/202: =0x ff c9 ff db
    //    ;  Calculated Track Velocity: vx=-55 (-13.75 m/s); vy=-37 (-9.25 m/s)

    loginf << "cat020 test: 202" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("202").at("Vx");
    assert (fabs(tmp_d - -13.75) < 10e-3);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("202").at("Vy");
    assert (fabs(tmp_d - -9.25) < 10e-3);

    //    ;  I020/090: =0x 00 2d
    //    ;  Flight Level: v=0; g=0; code=45 (11.25 FL)

    loginf << "cat020 test: 090" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("090").at("V") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("090").at("G") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("090").at("Flight Level") == 11.25);

    //    ;  I020/220: =0x 02 44 2f
    //    ;  Target Address: 0x02442f (148527)

    loginf << "cat020 test: 220" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("220").at("Target Address") == 148527);

    //    ;  I020/210: =0x 00 00
    //    ;  Calculated Acceleration: ax=0 (0.00 m/s**2); ay=0 (0.00 m/s**2)

    loginf << "cat020 test: 210" << logendl;
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("210").at("Ax");
    assert (fabs(tmp_d - 0.0) < 10e-3);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("210").at("Ay");
    assert (fabs(tmp_d - 0.0) < 10e-3);

    //    ;  I020/400: =0x 10 00 00 00  00 00 00 00  00 00 00 10  00 00 20 00
    //    ;            +0x 22
    //    ;  Contributing Receivers: 0x 00 00 00 00 00 00 00 00 00 00 10 00 00 20 00 22

    loginf << "cat020 test: 400" << logendl;

    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers").size() == 16);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[0].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[1].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[2].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[3].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[4].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[5].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[6].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[7].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[8].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[9].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[10].at("RUx") == 16);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[11].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[12].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[13].at("RUx") == 32);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[14].at("RUx") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("400").at("Contributing Receivers")[15].at("RUx") == 34);

    //    ;  I020/250: =0x 02 10 00 00  00 a0 00 00  10 00 00 00  00 00 00 00
    //    ;            +0x 17
    //    ;  Mode S MB Data:
    //    ;   BDS 1,0 data=0x 10 00 00 00 a0 00 00
    //    ;   BDS 1,7 data=0x 00 00 00 00 00 00 00

    loginf << "cat020 test: 250" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[0].size() == 3);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[0].at("BDS1") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[0].at("BDS2") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[0].at("MB Data")
            == "10000000a00000");
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[1].size() == 3);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[1].at("BDS1") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[1].at("BDS2") == 7);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("250").at("Mode S MB Data")[1].at("MB Data")
            == "00000000000000");

    //    ;  I020/230: =0x 20 40
    //    ;  Communications/ACAS Capability and Flight Status:
    //    ;   COM=1; STAT=0; ARC=1; AIC=0; B1A=0; B1B=0

    loginf << "cat020 test: 230" << logendl;
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("230").at("COM") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("230").at("STAT") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("230").at("ARC") == 1);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("230").at("AIC") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("230").at("B1A") == 0);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("230").at("B1B") == 0);

    //    ;  I020/RE:  0x 15 80 d0 00  12 00 0f ff  f1 00 89 00  7c ff 86 00
    //    ;            0x 35 00 53 ff  c1

    loginf << "cat020 test: REF: PA" << logendl;
    //    ;  Position Accuracy:
    //    ;   DOP of Position: x=18 (4.50); y=15 (3.75); xy=-15 (-3.75)
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("DOP").at("DOP-x")
            == 4.5);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("DOP").at("DOP-y")
            == 3.75);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("DOP").at("DOP-xy")
            == -3.75);

    //    ;   Standard Deviation of Position: x=137 (34.25 mtr); y=124 (31.00 mtr); xy=-122 (-30.50 mtr)
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("SDC").at(
                "SDC (X-Component)") == 34.25);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("SDC").at(
                "SDC (Y-Component)") == 31);
    assert (json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("SDC").at(
                "COV-XY (Covariance Component)") == -30.5);

    //    ;   Standard Deviation of Position: lat=53 (0.000284 deg); lon=83 (0.000445 deg); cov=-63 (-0.000338 deg)
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("SDW").at(
                "SDW (Latitude Component)");
    assert (tmp_d - 0.000284 < 10e-6);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("SDW").at(
                "SDW (Longitude Component)");
    assert (tmp_d - 0.000445 < 10e-6);
    tmp_d = json_data->at("data_blocks")[0].at("content").at("records")[0].at("REF").at("PA").at("SDW").at(
                "COV-WGS (Lat/Long Covariance Component)");
    assert (tmp_d + 0.000338 < 10e-6);
}

void test_cat020 (jASTERIX::jASTERIX& jasterix)
{
    loginf << "cat020 test: start" << logendl;

    // echo -n 140065ffe9478400024100416f5b008832e5002e6c4a054bb301606a0dc8182e00ffc9ffdb002d02442f000010000000000000000000001000002000220210000000a0000010000000000000001720401580d00012000ffff10089007cff8600350053ffc1 | xxd -r -p > cat020ed1.5.bin

    const char *cat020_ed15 =
            "140065ffe9478400024100416f5b008832e5002e6c4a054bb301606a0dc8182e00ffc9ffdb002d02442f000010000000000000000000001000002000220210000000a0000010000000000000001720401580d00012000ffff10089007cff8600350053ffc1";
    char* target = new char[strlen(cat020_ed15)/2];

    size_t size = hex2bin (cat020_ed15, target);

    loginf << "cat020 ed 1.5 test: src len " << strlen(cat020_ed15) << " bin len " << size << logendl;

    assert (size == 101);

    assert (jasterix.hasCategory(20));
    std::shared_ptr<jASTERIX::Category> cat020 = jasterix.category(20);
    assert (cat020->hasEdition("1.5"));
    cat020->setCurrentEdition("1.5");
    cat020->setCurrentMapping("");

    jasterix.decodeASTERIX(target, size, test_cat020_callback);

    delete[] target;

    loginf << "cat020 ed 1.5 test: end" << logendl;
}


