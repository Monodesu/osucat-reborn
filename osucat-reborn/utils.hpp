#pragma once

#include <algorithm> 
#include <cctype>
#include <locale>
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
        static inline void string_replace(std::string& str, const std::string& old_val, const std::string& new_val) {
            // see https://stackoverflow.com/a/29752943

            std::string res;
            res.reserve(str.length()); // avoids a few memory allocations

            std::string::size_type last_pos = 0;
            std::string::size_type pos;
            while (std::string::npos != (pos = str.find(old_val, last_pos))) {
                res.append(str, last_pos, pos - last_pos);
                res += new_val;
                last_pos = pos + old_val.length();
            }

            // care for the rest after last occurrence
            res += str.substr(last_pos);
            str.swap(res);
        }
        /*
        about trim see : https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
        */
        static inline void ltrim(std::string& s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
                }));
        }
        static inline void rtrim(std::string& s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
                }).base(), s.end());
        }
        static inline void trim(std::string& s) {
            ltrim(s);
            rtrim(s);
        }
    };

}


