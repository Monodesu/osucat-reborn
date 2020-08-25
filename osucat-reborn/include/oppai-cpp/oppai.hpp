#ifndef oppai_hpp
#define oppai_hpp

#define OPPAI_IMPLEMENTATION

#include "oppai/oppai.c"
#include "oppai/mods.hpp"

#define al_round(x) (float)floor((x) + 0.5f)
#define twodec(x) (al_round((x)*100.0f) / 100.0f)

enum score_result {
    PERFECT = 1,
    NO_BREAK = 2,
    SLIDER_BREAK = 4,
    ONE_MISS = 8,
    END_CHOKE = 16,
    CLEAR = 32,
    FULL_COMBO = PERFECT | NO_BREAK,
    CHOKE = SLIDER_BREAK | ONE_MISS | END_CHOKE
};

struct oppai_result {
    int32_t code;
    std::string message;
    struct {
        std::string artist, artistUnicode;
        std::string title, titleUnicode;
        std::string creator;
        std::string map_version;
        float cs, od, ar, hp;
        double total_star, aim_star, speed_star;
        double  aim_pp, speed_pp, acc_pp;
        optional<double> total_pp;
        int mode;
        int mods_num;
        std::string mods_str;
        double accuracy;
        int length, maxlength;  // milliseconds
        int combo, maxcombo;
        double bpm;
        score_result sr;
        int n300, n100, n50, nmiss;
    } data;
};

class oppai {
public:
    oppai() {
        this->ez = ezpp_new();
        this->_n100 = this->_n50 = this->_nmiss = 0;
        ezpp_set_mode(this->ez, 0);
        this->data = "";
    }
    static std::string version() { return oppai_version_str(); }

    void read_map_file(const std::string& mapfile) {
        this->data = "";
        std::ifstream file(mapfile);
        while (!file.eof()) {
            this->data += (char)file.get();
        }
        file.close();
        this->data.pop_back();
    }

    void read_data(const std::string& data) { this->data = data; }

    float accuracy_percent() { return ezpp_accuracy_percent(this->ez); }

    void accuracy_percent(float acc) { ezpp_set_accuracy_percent(this->ez, acc); }

    int end() { return this->ez->end; }

    void end(int end) { ezpp_set_end(this->ez, end); }

    int n300() { return this->ez->n300; };

    void n300(int n300) { this->ez->n300 = n300; }

    int n100() { return ezpp_n100(this->ez); }

    void n100(int n100) {
        this->ez->n100 = n100;
    }

    int n50() { return ezpp_n50(this->ez); }

    void n50(int n50) {
        this->ez->n50 = n50;
    }

    int nmiss() { return ezpp_nmiss(this->ez); }

    void nmiss(int nmiss) { ezpp_set_nmiss(this->ez, nmiss); }

    float speed_stars() { return ezpp_speed_stars(this->ez); }

    void speed_stars(float speed_stars) {
        ezpp_set_speed_stars(this->ez, speed_stars);
    }

    float aim_stars() { return ezpp_aim_stars(this->ez); }

    void aim_stars(float aim_stars) { ezpp_set_aim_stars(this->ez, aim_stars); }

    int combo() { return ezpp_combo(this->ez); }

    void combo(int combo) { ezpp_set_combo(this->ez, combo); }

    void set_score_version(int score_version) {
        if (score_version < 1 || score_version > 2) return;
        ezpp_set_score_version(this->ez, score_version);
    }

    float ar() { return ezpp_ar(this->ez); }

    void ar(float ar) { ezpp_set_base_ar(this->ez, ar); }

    float od() { return ezpp_od(this->ez); }

    void od(float od) { ezpp_set_base_od(this->ez, od); }

    float cs() { return ezpp_cs(this->ez); }

    void cs(float cs) { ezpp_set_base_cs(this->ez, cs); }

    float hp() { return ezpp_hp(this->ez); }

    void hp(float hp) { ezpp_set_base_hp(this->ez, hp); }

    int mode() { return ezpp_mode(this->ez); }

    void mode(int mode_num) { ezpp_set_mode_override(this->ez, mode_num); }

    OsuCat::Mod mods() {
        return OsuCat::Mod(ezpp_mods(this->ez));
    }

    void mods(OsuCat::Mod& mods) {
        ezpp_set_mods(this->ez, mods.GetModNumber());
    }

    void mods(int mods) { ezpp_set_mods(this->ez, mods); }

    oppai_result calc(const std::string& inpent = "") {
        oppai_result result;
        int ret = ezpp_data(this->ez, (char*)this->data.c_str(),
            (int)this->data.length());
        result.code = ret;
        result.message = "";
        if (ret < 0) {
            result.message = errstr(ret);
            return result;
        }

        result.data.artist = ezpp_artist(this->ez);
        result.data.artistUnicode = ezpp_artist_unicode(this->ez);
        result.data.title = ezpp_title(this->ez);
        result.data.titleUnicode = ezpp_title_unicode(this->ez);
        result.data.map_version = ezpp_version(this->ez);
        result.data.creator = ezpp_creator(this->ez);
        result.data.ar = ezpp_ar(this->ez);
        result.data.od = ezpp_od(this->ez);
        result.data.cs = ezpp_cs(this->ez);
        result.data.hp = ezpp_hp(this->ez);
        result.data.total_star = ezpp_stars(this->ez);
        result.data.aim_star = ezpp_aim_stars(this->ez);
        result.data.speed_star = ezpp_speed_stars(this->ez);
        result.data.accuracy = ezpp_accuracy_percent(this->ez);
        result.data.total_pp = ezpp_pp(this->ez);
        result.data.aim_pp = ezpp_aim_pp(this->ez);
        result.data.speed_pp = ezpp_speed_pp(this->ez);
        result.data.acc_pp = ezpp_acc_pp(this->ez);
        result.data.mode = ezpp_mode(this->ez);
        result.data.mods_num = ezpp_mods(this->ez);
        result.data.length = ezpp_time_at(this->ez, ezpp_nobjects(this->ez) - 1) -
            ezpp_time_at(this->ez, 0);
        result.data.mods_str = OsuCat::Mod(ezpp_mods(this->ez)).GetModString(inpent);
        result.data.maxcombo = ezpp_max_combo(this->ez);
        result.data.combo = ezpp_combo(this->ez);
        result.data.n300 = ezpp_n300(this->ez);
        result.data.n100 = ezpp_n100(this->ez);
        result.data.n50 = ezpp_n50(this->ez);
        result.data.nmiss = ezpp_nmiss(this->ez);
        if (this->ez->timing_points.data[0].ms_per_beat == 0) {
            result.data.bpm = -1.0;  // inf
        }
        else {
            result.data.bpm = 60000 / this->ez->timing_points.data[0].ms_per_beat;
        }
        double pct_combo = (double)result.data.combo / result.data.maxcombo;
        if (pct_combo == 1) {
            result.data.sr = score_result::PERFECT;
            // and -> &&
        }
        else if (pct_combo > 0.98 && result.data.nmiss == 0) {
            result.data.sr = score_result::NO_BREAK;
        }
        else if (pct_combo > 0.85) {
            result.data.sr = score_result::END_CHOKE;
        }
        else if (result.data.nmiss == 0) {
            result.data.sr = score_result::SLIDER_BREAK;
        }
        else if (result.data.nmiss == 1) {
            result.data.sr = score_result::ONE_MISS;
        }
        else {
            result.data.sr = score_result::CLEAR;
        }
        return result;
    }

    ~oppai() { ezpp_free(this->ez); }

private:
    ezpp_t ez;
    int _n100, _n50, _nmiss;
    std::string data;
};

#endif /* oppai_hpp */
