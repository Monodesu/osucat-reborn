#pragma once
#ifndef IMAGEMAKER_HPP
#define IMAGEMAKER_HPP

//using namespace Magick;
using namespace osucat::osu_api_v1;

namespace osucat::imageMaker {
    struct ScorePanelData {
        user_info user_info;
        oppai_result pp_info;
        score_info score_info;
        // 95, 97, 98, 99, 100
        int confirmed_acc[5];
        int fc;
        beatmap_info beatmap_info;
        string customPanel;
        mode mode;
    };

    struct UserPanelData {
        user_info user_info;
        pplus_info pplus_info;
        string customPanel;
        int badgeid = EOF;
    };

    //Todo ÒÆÖ²ImageMakerÄÚÈÝ
}
#endif