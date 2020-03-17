#ifndef SYSTEM_H
#define SYSTEM_H

//#include <sys/sysinfo.h>
//#include <stdint.h>

#include <sys/resource.h>

#include <fstream>
#include <limits>
#include <sstream>
#include <string>

#include "logger.h"

const double megabyte = 1024 * 1024;
const double gigabyte = 1024 * 1024 * 1024;

namespace Utils
{
namespace System
{
float getProcessRAMinGB()
{
    struct rusage info;
    getrusage(RUSAGE_SELF, &info);

    //    long int ru_maxrss
    //    The maximum resident set size used, in kilobytes. That is, the maximum number of kilobytes
    //    of physical memory that processes used simultaneously.

    return (info.ru_maxrss) / megabyte;
}

float getFreeRAMinGB()
{
    std::string token;
    std::ifstream file("/proc/meminfo");
    while (file >> token)
    {
        if (token == "MemAvailable:")
        {
            unsigned long mem;

            if (file >> mem)  // returns in kB
            {
                return mem / megabyte;
            }
            else
            {
                return 0;
            }
        }
        // ignore rest of the line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;  // nothing found
}

}  // namespace System
}  // namespace Utils

#endif  // SYSTEM_H
