#pragma once
#ifndef OSUCAT_BADGE_HPP
#define OSUCAT_BADGE_HPP

namespace osucat::badgeSystem {
    class main {
    public:
        enum badge {
            OSUCAT_TEAMMEMBER = 0,
            OSUCAT_SPECIALTHANKS,
            ONCS1_STAFF,
            ONCS1_WINNER,
            OCCS1_STAFF,
            OCCS1_CHAMPION,
            OCCS1_RUNNERUP,
            OCCS1_SNDRUNNERUP,
            OCCS1_CONTESTANT,
            EZC_DEMO_STAFF,
            EZC_DEMO_1ST,
            EZC_DEMO_2ND,
            EZC_DEMO_3RD,
            EZC_DEMO_4TH,
            SELF_MAXIUMINDEX
        };

        string getBadgeStr(int badgenum) {
            switch (badgenum) {
            case 0:
                return "(ID:0)OsuCat_Team";
            case 1:
                return "(ID:1)OsuCat_Special_Thanks";
            case 2:
                return "(ID:2)ONC_S1_Staff";
            case 3:
                return "(ID:3)ONC_S1_Winner";
            case 4:
                return "(ID:4)OCC_S1_Staff";
            case 5:
                return "(ID:5)OCC_S1_Champion";
            case 6:
                return "(ID:6)OCC_S1_Runner_up";
            case 7:
                return "(ID:7)OCC_S1_Second_Runner_up";
            case 8:
                return "(ID:8)OCC_S1_Contestant";
            case 9:
                return "(ID:9)EZC_DEMO_Staff";
            case 10:
                return "(ID:10)EZC_DEMO_Winner";
            case 11:
                return "(ID:11)EZC_DEMO_2ND_Place";
            case 12:
                return "(ID:12)EZC_DEMO_3RD_Place";
            case 13:
                return "(ID:13)EZC_DEMO_4TH_Place";
            default:
                return "no badge name found.";
            }
        }

        vector<int> badgeParser(string badgeStr) {
            string word;
            vector<int> rtn;
            for (size_t i = 0; i < badgeStr.size(); ++i) {
                char c = badgeStr[i];
                if (c == ',') {
                    rtn.push_back(stoi(word));
                    word.clear();
                }
                else {
                    word += c;
                }
                if (i == badgeStr.size() - 1) rtn.push_back(stoi(word));
            }
            return rtn;
        }
    };
} // namespace badgeSystem
#endif