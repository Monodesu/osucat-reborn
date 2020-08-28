#pragma once
#ifndef SAYOBOT_MODS_HPP
#define SAYOBOT_MODS_HPP


namespace osu {
    enum Mods {
        None = 1 >> 1,
        NoFail = 1 << 0,
        Easy = 1 << 1,
        TouchDevice = 1 << 2,
        Hidden = 1 << 3,
        HardRock = 1 << 4,
        SuddenDeath = 1 << 5,
        DoubleTime = 1 << 6,
        Relax = 1 << 7,
        HalfTime = 1 << 8,
        Nightcore = 1 << 9 | DoubleTime, // Only set along with DoubleTime. i.e: NC only gives 576
        Flashlight = 1 << 10,
        Autoplay = 1 << 11,
        SpunOut = 1 << 12,
        Relax2 = 1 << 13, // Autopilot
        Perfect =
        1 << 14 | SuddenDeath, // Only set along with SuddenDeath. i.e: PF only gives 16416
        Key4 = 1 << 15,
        Key5 = 1 << 16,
        Key6 = 1 << 17,
        Key7 = 1 << 18,
        Key8 = 1 << 19,
        FadeIn = 1 << 20,
        Random = 1 << 21,
        Cinema = 1 << 22,
        Target = 1 << 23,
        Key9 = 1 << 24,
        KeyCoop = 1 << 25,
        Key1 = 1 << 26,
        Key3 = 1 << 27,
        Key2 = 1 << 28,
        ScoreV2 = 1 << 29,
        Mirror = 1 << 30,
        KeyMod = Key1 | Key2 | Key3 | Key4 | Key5 | Key6 | Key7 | Key8 | Key9 | KeyCoop,
        FreeModAllowed = NoFail | Easy | Hidden | HardRock | SuddenDeath | Flashlight | FadeIn
        | Relax | Relax2 | SpunOut | KeyMod,
        ScoreIncreaseMods = Hidden | HardRock | DoubleTime | Flashlight | FadeIn
    };
}

namespace osucat {
    class Mod {
    public:
        // mod数字解析
        Mod(int32_t mod_num) {
            for (int i = 0; i < 31; ++i) {
                bool flag = mod_num & 0x1;
                if (flag) {
                    int32_t mod = 1 << i;
                    if (i == 9) // Nightcore
                    {
                        enabled_mods.erase(osu::Mods::DoubleTime);
                        mod = mod | (int32_t)osu::Mods::DoubleTime;
                    }
                    if (i == 14) // Prefect
                    {
                        enabled_mods.erase(osu::Mods::SuddenDeath);
                        mod = mod | (int32_t)osu::Mods::SuddenDeath;
                    }
                    enabled_mods.insert((osu::Mods)mod);
                }
                mod_num >>= 1;
            }
        }
        // mod字符串解析
        Mod(std::string _mod_str) {
            std::transform(_mod_str.begin(), _mod_str.end(), _mod_str.begin(), ::toupper);

            // 去除mod字串中A-Z 0-9以外的字符
            for (auto it = _mod_str.begin(); it != _mod_str.end(); ++it) {
                if ((*it < 'A' || *it > 'Z') && (*it < '0' || *it > '9')) {
                    _mod_str.erase(it);
                }
            }

            // mod字串个数不是2的倍数
            // 移除最后一个字符
            if (_mod_str.size() % 2) {
                _mod_str.erase(_mod_str.size() - 1);
            }

            for (int i = 0; i < _mod_str.size() / 2; ++i) {
                std::string amod = _mod_str.substr(i * 2, 2);
                for (int i = 0; i < 31; ++i) {
                    if (!mods_str[i].compare(amod)) {
                        int32_t mod_num = 1 << i;
                        if (i == 9) {
                            mod_num += (int32_t)osu::Mods::DoubleTime;
                        }
                        if (i == 14) {
                            mod_num += (int32_t)osu::Mods::SuddenDeath;
                        }
                        enabled_mods.insert((osu::Mods)mod_num);
                        break;
                    }
                }
            }
        }
        // 获取mod字符串，已被排序
        // inpent为mod与mod之间的分隔字符串
        std::string GetModString(const std::string& inpent = "") {
            std::string ret = "";
            int32_t mod_num = GetModNumber();
            for (int i = 30; i >= 0; --i) {
                bool flag = mod_num & (1 << i);
                if (flag) {
                    if (i == 9) // Nightcore
                    {
                        mod_num -= (int32_t)osu::Mods::DoubleTime;
                    }
                    if (i == 14) // Prefect
                    {
                        mod_num -= (int32_t)osu::Mods::SuddenDeath;
                    }
                    ret += mods_str[i];
                    ret += inpent;
                }
            }
            return ret.substr(0, ret.size() - inpent.size());
        }
        // 获取mod数字
        int32_t GetModNumber() {
            int32_t ret = 0;
            for (auto it : enabled_mods) {
                ret = ret | it;
            }
            return ret;
        }
        // 检查mod是否合法
        bool isVaild() {
            bool flag = true;
            int32_t mod_num = GetModNumber();
            for (auto it : invaild_mods) {
                if ((mod_num & it) == it) {
                    flag = false;
                    break;
                }
            }
            return flag;
        }

    private:
        const static std::vector<std::string> mods_str;
        const static std::vector<int32_t> invaild_mods;
        std::set<osu::Mods> enabled_mods;
    };

    const std::vector<std::string> Mod::mods_str = { "NF", "EZ", "TD", "HD", "HR", "SD", "DT", "RX",
                                                    "HT", "NC", "FL", "AU", "SO", "AP", "PF", "K4",
                                                    "K5", "K6", "K7", "K8", "FI", "RD", "CN", "TG",
                                                    "K9", "KC", "K1", "K3", "K2", "S2", "MR" };

    const std::vector<int32_t> Mod::invaild_mods = {
        (int32_t)(osu::Mods::Easy | osu::Mods::HardRock),
        (int32_t)(osu::Mods::HalfTime | osu::Mods::DoubleTime),
        (int32_t)(osu::Mods::NoFail | osu::Mods::SuddenDeath),
        (int32_t)(osu::Mods::Relax | osu::Mods::Relax2),
    };
} // namespace OsuCat

#endif
