# jASTERIX

jASTERIX is a C++ Library for EUROCONTROL's [ASTERIX](https://www.eurocontrol.int/services/asterix) to [JSON](https://www.json.org/) conversion. It is part of the [ATSDB project](https://github.com/hpuhr/ATSDB/), but released as a separate library to allow easy usage in other projects.

The library allows decoding of binary ASTERIX data into JSON. The ASTERIX definitions are configuration only, so additional or customized categories or editions can be added without having to recompile. Also, using the Intel TBB library allows for reasonable performance using multi-threading.

## Features

Many ASTERIX decoder libraries exist today, but none of them was suitable/available for the ATSDB project. For this reason, the author decided to implement one, with a strong focus on flexibility first and performance second.

- High flexibility using configuration only
  - New framings
  - New categories 
  - New editions
- Reasonable performance using multi-threading
- Client binary
  - Reading of input file
  - Printing decoded JSON
  - Writing decoded JSON to text or ZIP file 

Currently support framings:
- None: Raw, netto, unframed ASTERIX data blocks
- IOSS: IOSS Final Format
- RFF: Comsoft RFF format

Currently supported ASTERIX categories & editions, reserved expansion fields and special purpose fields:

| CAT |       Editions | REFs|       SPFs|
|-----|----------------|-----|-----------|
|  001|             1.1|     |           |
|  002|             1.0|     |           |
|  019|             1.2|     |           |
|  020|        1.5, 1.8|  1.3|           |
|  021|             2.1|     |           |
|  023|             1.2|     |           |
|  034|            1.26|     |           |
|  048|            1.15|     |           |
|  062|      1.12, 1.16|     |           |
|  063|             1.0|     | ARTAS TRIs|
|  063|             1.1|     |           |
|  065|        1.2, 1.3|     |           |


## Contents
- Folder "cmake_modules": Contains cmake find scripts
- Folder "definitions": Contains ASTERIX configuration definitions
- Folder "include": Main headers
- Folder "src": Contains source code
- CMakeLists.txt: CMake config file
- LICENSE: GPL license
- README.md: This file

## Requirements / Used Libraries

The following libraries are mandatory:
- Intel Thread Building Blocks

The following libaries are optional (can be deactivated in CMakeList.txt):
- Boost
- Log4Cpp

Also, the Nlohmann::JSON library is used.

## Installation / Building

Download the source code from this page, then execute the following commands in the check-out folder:
```
mkdir build
cmake ..
make
```

To install the built libary in the system, execute the following command in the build folder:
```
sudo make install
```

## Usage

After building, the following jASTERIX client can be used (from the check-out folder):
```
/build/bin/jasterix_client --help
INFO    : Allowed options:
  --help                 produce help message
  --filename arg         input file name
  --definition_path arg  path to jASTERIX definition files
  --framing arg          input framine format, as specified in the framing 
                         definitions. netto is default
  --frame_limit arg      number of frames to process, default -1, use -1 to 
                         disable.
  --frame_chunk_size arg number of frames to process in one chunk, default 
                         1000, use -1 to disable.
  --data_write_size arg  number of frame chunks to write in one file write, 
                         default 100, use -1 to disable.
  --debug                print debug output
  --print                print JSON output
  --print_indent arg     intendation of json print, use -1 to disable.
  --write_type arg       optional write type, e.g. text,zip. needs 
                         write_filename.
  --write_filename arg   optional write filename, e.g. test.zip.

```

To decode and print an ASTERIX file as JSON text, use:
```
./build/bin/jasterix_client --definition_path definitions/ --filename src/test/cat020ed1.5.bin --print
INFO    : {
    "data_blocks": [
        {
            "category": 20,
            "content": {
                "index": 3,
                "length": 98,
                "records": [
                    {
                        "010": {
                            "SAC": 0,
                            "SIC": 2
                        },
                        "020": {
                            "CHN": 0,
                            "CRT": 0,
                            "DME": 0,
                            "FX": 1,
                            "FX2": 0,
                            "GBS": 0,
                            "HF": 0,
                            "MS": 1,
                            "OT": 0,
                            "RAB": 0,
                            "SIM": 0,
                            "SPI": 0,
                            "SSR": 0,
                            "TST": 0,
                            "UAT": 0,
                            "VDL4": 0
                        },
                        "041": {
                            "Latitude": 47.88232132925,
                            "Longitude": 16.32056296698
                        },
                        "042": {
                            "X": 173529.5,
                            "Y": 45109.0
                        },
                        "070": {
                            "G": 0,
                            "L": 1,
                            "Mode-3/A code": 7000,
                            "V": 0
                        },
                        "090": {
                            "Flight Level": 11.25,
                            "G": 0,
                            "V": 0
                        },
                        "140": {
                            "Time of Day": 33502.7109375
                        },
                        "161": {
                            "Track Number": 3528
                        },
                        "170": {
                            "CDM": 3,
                            "CNF": 0,
                            "CST": 0,
                            "FX": 0,
                            "MAH": 0,
                            "STH": 0,
                            "TRE": 0
                        },
                        "202": {
                            "Vx": -13.75,
                            "Vy": -9.25
                        },
                        "210": {
                            "Ax": 0.0,
                            "Ay": 0.0
                        },
                        "220": {
                            "Target Address": 148527
                        },
                        "230": {
                            "AIC": 0,
                            "ARC": 1,
                            "B1A": 0,
                            "B1B": 0,
                            "COM": 1,
                            "MSSC": 0,
                            "STAT": 0
                        },
                        "250": {
                            "Mode S MB Data": [
                                {
                                    "BDS1": 1,
                                    "BDS2": 0,
                                    "MB Data": "10000000a00000"
                                },
                                {
                                    "BDS1": 1,
                                    "BDS2": 7,
                                    "MB Data": "00000000000000"
                                }
                            ],
                            "REP": 2
                        },
                        "400": {
                            "Contributing Receivers": [
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 16
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 32
                                },
                                {
                                    "RUx": 0
                                },
                                {
                                    "RUx": 34
                                }
                            ],
                            "REP": 16
                        },
                        "FSPEC": [
                            true,
                            true,
                            true,
                            true,
                            true,
                            true,
                            true,
                            true,
                            true,
                            true,
                            true,
                            false,
                            true,
                            false,
                            false,
                            true,
                            false,
                            true,
                            false,
                            false,
                            false,
                            true,
                            true,
                            true,
                            true,
                            false,
                            false,
                            false,
                            false,
                            true,
                            false,
                            false
                        ]
                    }
                ]
            },
            "length": 101
        }
    ]
}
INFO    : jASTERIX client: decoded 0 frames, 1 records in 0h 0m 0s 1ms: 0 fr/s, 1000 rec/s
INFO    : jASTERIX: shutdown
```

Decoded JSON can also be written to a text or ZIP file using the appropriate arguments.

## Questions & Request

For additional information or request for additional categories / editions please contact the author.

## Author
Helmut Puhr
Contact: atsdb@gmx.at

## Licenses
The source code is released under [GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html)

Disclaimer
----------

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
