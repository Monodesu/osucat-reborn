#pragma once

#include <ctime>
namespace osucat {
    class OCUtils {
    public:
        static std::string unixTime2Str(int timetick)
        {
            char rtn[128];
            time_t tick;
            struct tm tm = { 0 };
            tick = timetick;
            tm = *localtime(&tick);
            strftime(rtn, 128, "%Y-%m-%d %H:%M:%S", &tm);
            return rtn;
        }
    };

}


