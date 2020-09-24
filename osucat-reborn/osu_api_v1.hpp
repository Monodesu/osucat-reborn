#pragma once
#ifndef osu_api_v1_hpp
#define osu_api_v1_hpp

#define OSU_API_V1 "https://osu.ppy.sh/api/"
#define OSU_FILE_URL "https://osu.ppy.sh/osu/"
#define SAYO_API "https://api.sayobot.cn/beatmapinfo?0=" // BID
char OSU_KEY[128];

namespace osucat::osu_api_v1 {
	enum beatmap_status { graveyard = -2, WIP, pending, ranked, approved, qualified, loved };
	enum mode { std = 0, taiko, ctb, mania };
	enum Mods {
		None = 0,
		NoFail = 1,
		Easy = 2,
		TouchDevice = 4,
		Hidden = 8,
		HardRock = 16,
		SuddenDeath = 32,
		DoubleTime = 64,
		Relax = 128,
		HalfTime = 256,
		Nightcore = 512, // Only set along with DoubleTime. i.e: NC only gives 576
		Flashlight = 1024,
		Autoplay = 2048,
		SpunOut = 4096,
		Relax2 = 8192, // Autopilot
		Perfect = 16384, // Only set along with SuddenDeath. i.e: PF only gives 16416
		Key4 = 32768,
		Key5 = 65536,
		Key6 = 131072,
		Key7 = 262144,
		Key8 = 524288,
		FadeIn = 1048576,
		Random = 2097152,
		Cinema = 4194304,
		Target = 8388608,
		Key9 = 16777216,
		KeyCoop = 33554432,
		Key1 = 67108864,
		Key3 = 134217728,
		Key2 = 268435456,
		ScoreV2 = 536870912,
		Mirror = 1073741824,
		KeyMod = Key1 | Key2 | Key3 | Key4 | Key5 | Key6 | Key7 | Key8 | Key9 | KeyCoop,
		FreeModAllowed =
		NoFail | Easy | Hidden | HardRock | SuddenDeath | Flashlight | FadeIn | Relax | Relax2 | SpunOut | KeyMod,
		ScoreIncreaseMods = Hidden | HardRock | DoubleTime | Flashlight | FadeIn
	};

	struct beatmap_info {
		int64_t beatmap_id, beatmapset_id;
		string artist, title;
		// value is -1 if timestamp not existed
		int64_t submit_timestamp, lastupdate_timestamp, approved_timestamp;
		string creator;
		int64_t creator_id;
		float bpm;
		double stars, aim_stars, speed_stars;
		float cs, ar, od, hp;
		vector<string> tags;
		string source;
		string version;
		string filemd5;
		// in seconds
		int64_t game_length, total_length;
		beatmap_status approved;
		float rating;
		int favourite_count;
		int64_t playcount, passcount;
		int count_circle, count_slider, count_spinner;
		int maxcombo;
		bool hasStoryboard, hasVideo, canDownload;
		mode mode;
	};
	struct user_info {
		int64_t user_id;
		string username;
		int64_t registed_timestamp;
		int64_t n300, n100, n50;
		int64_t playcount;
		int64_t total_score, ranked_score;
		float pp;
		int64_t country_rank, global_rank;
		int count_ssh, count_ss, count_sh, count_s, count_a;
		int64_t playtime;
		float level;
		double accuracy;
		string country;
		int mode;
		bool updatepplus;
		int days_before = 0;
	};
	struct pplus_info {
		int jump;
		int flow;
		int pre;
		int acc;
		int spd;
		int sta;
	};
	struct score_info {
		int64_t beatmap_id;
		int64_t score;
		int64_t user_id;
		int n300, n100, n50, nmiss;
		// for ctb, taiko, mania
		int nkatu, ngeki;
		int combo;
		int64_t mods;
		long achieved_timestamp;
		double pp;
		bool hasReplay;
		string rank;
		string username;
	};

	class api {
	private:
		const static vector<string> mods_str;
		const static vector<int32_t> invaild_mods;
		static set<Mods> enabled_mods;

	public:
		static string modParser(int64_t mods) {
			string MODS;
			if (mods == (int)Mods::None) {
				return "None";
			}
			else {
				if (mods & Mods::NoFail) {
					MODS = MODS + "NF";
				}
				if (mods & Mods::Easy) {
					MODS = MODS + "EZ";
				}
				if (mods & Mods::TouchDevice) {
					MODS = MODS + "TD";
				}
				if (mods & Mods::Hidden) {
					MODS = MODS + "HD";
				}
				if (mods & Mods::HardRock) {
					MODS = MODS + "HR";
				}
				if (mods & Mods::SuddenDeath) {
					MODS = MODS + "SD";
				}
				if (mods & Mods::DoubleTime) {
					MODS = MODS + "DT";
				}
				if (mods & Mods::Relax) {
					MODS = MODS + "RX";
				}
				if (mods & Mods::HalfTime) {
					MODS = MODS + "HT";
				}
				if (mods & Mods::Nightcore) {
					MODS = MODS.replace(MODS.find("DT"), 2, "NC");
				}
				if (mods & Mods::Flashlight) {
					MODS = MODS + "FL";
				}
				if (mods & Mods::Autoplay) {
					MODS = MODS + "AT";
				}
				if (mods & Mods::SpunOut) {
					MODS = MODS + "SO";
				}
				if (mods & Mods::Relax2) {
					MODS = MODS + "AP";
				}
				if (mods & Mods::Perfect) {
					MODS = MODS.replace(MODS.find("SD"), 2, "PF");
				}
				if (mods & Mods::Key4) {
					MODS = MODS + "4K";
				}
				if (mods & Mods::Key5) {
					MODS = MODS + "5K";
				}
				if (mods & Mods::Key6) {
					MODS = MODS + "6K";
				}
				if (mods & Mods::Key7) {
					MODS = MODS + "7K";
				}
				if (mods & Mods::Key8) {
					MODS = MODS + "8K";
				}
				if (mods & Mods::FadeIn) {
					MODS = MODS + "FD";
				}
				if (mods & Mods::Random) {
					MODS = MODS + "RD";
				}
				if (mods & Mods::Cinema) {
					MODS = MODS + "CN";
				}
				if (mods & Mods::Target) {
					MODS = MODS + "TG";
				}
				if (mods & Mods::Key9) {
					MODS = MODS + "9K";
				}
				if (mods & Mods::KeyCoop) {
					MODS = MODS + "KC";
				}
				if (mods & Mods::Key1) {
					MODS = MODS + "1K";
				}
				if (mods & Mods::Key3) {
					MODS = MODS + "3K";
				}
				if (mods & Mods::Key2) {
					MODS = MODS + "2K";
				}
				if (mods & Mods::ScoreV2) {
					MODS = MODS + "V2";
				}
				if (mods & Mods::Mirror) {
					MODS = MODS + "MR";
				}
			}
			return MODS;
		}
		static int GetUser(int64_t uid, mode mode, user_info* info) {
			char url[512];
			sprintf_s(url, 512, OSU_API_V1 "get_user?type=id&k=%s&u=%I64d&m=%d", OSU_KEY, uid, (int)mode);
			string response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response)[0];
				tm time_temp;
				info->user_id = stoll(data["user_id"].get<string>());
				info->username = data["username"].get<string>();
				utils::fakestrptime(data["join_date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
				info->registed_timestamp = (int64_t)mktime(&time_temp) + 16 * 3600;
				try { info->n50 = stoll(data["count50"].get<string>()); }
				catch (std::exception) { info->n50 = 0; }
				try { info->n100 = stoll(data["count100"].get<string>()); }
				catch (std::exception) { info->n100 = 0; }
				try { info->n300 = stoll(data["count300"].get<string>()); }
				catch (std::exception) { info->n300 = 0; }
				try { info->playcount = stoll(data["playcount"].get<string>()); }
				catch (std::exception) { info->playcount = 0; }
				try { info->ranked_score = stoll(data["ranked_score"].get<string>()); }
				catch (std::exception) { info->ranked_score = 0; }
				try { info->total_score = stoll(data["total_score"].get<string>()); }
				catch (std::exception) { info->total_score = 0; }
				try { info->global_rank = stoll(data["pp_rank"].get<string>()); }
				catch (std::exception) { info->global_rank = 0; }
				try { info->level = stof(data["level"].get<string>()); }
				catch (std::exception) { info->level = 0.0; }
				try { info->pp = stof(data["pp_raw"].get<string>()); }
				catch (std::exception) { info->pp = 0.0; }
				try { info->accuracy = stod(data["accuracy"].get<string>()); }
				catch (std::exception) { info->accuracy = 0.0; }
				try { info->count_ss = stoi(data["count_rank_ss"].get<string>()); }
				catch (std::exception) { info->count_ss = 0; }
				try { info->count_ssh = stoi(data["count_rank_ssh"].get<string>()); }
				catch (std::exception) { info->count_ssh = 0; }
				try { info->count_s = stoi(data["count_rank_s"].get<string>()); }
				catch (std::exception) { info->count_s = 0; }
				try { info->count_sh = stoi(data["count_rank_sh"].get<string>()); }
				catch (std::exception) { info->count_sh = 0; }
				try { info->count_a = stoi(data["count_rank_a"].get<string>()); }
				catch (std::exception) { info->count_a = 0; }
				try { info->playtime = stoll(data["total_seconds_played"].get<string>()); }
				catch (std::exception) { info->playtime = 0; }
				try { info->country_rank = stoll(data["pp_country_rank"].get<string>()); }
				catch (std::exception) { info->country_rank = 0; }
				info->country = data["country"].get<string>();
				info->mode = (int)mode;
				return 1;
			}
			return 0;
		}
		static int GetUser(string username, mode mode, user_info* info) {
			char url[512];
			sprintf_s(url, 512, OSU_API_V1 "get_user?type=string&k=%s&u=%s&m=%d", OSU_KEY, username.c_str(), (int)mode);
			string response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response)[0];
				tm time_temp;
				info->user_id = stoi(data["user_id"].get<string>());
				info->username = data["username"].get<string>();
				utils::fakestrptime(data["join_date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
				info->registed_timestamp = (long)mktime(&time_temp) + 16 * 3600;
				try { info->n50 = stoll(data["count50"].get<string>()); }
				catch (std::exception) { info->n50 = 0; }
				try { info->n100 = stoll(data["count100"].get<string>()); }
				catch (std::exception) { info->n100 = 0; }
				try { info->n300 = stoll(data["count300"].get<string>()); }
				catch (std::exception) { info->n300 = 0; }
				try { info->playcount = stoll(data["playcount"].get<string>()); }
				catch (std::exception) { info->playcount = 0; }
				try { info->ranked_score = stoll(data["ranked_score"].get<string>()); }
				catch (std::exception) { info->ranked_score = 0; }
				try { info->total_score = stoll(data["total_score"].get<string>()); }
				catch (std::exception) { info->total_score = 0; }
				try { info->global_rank = stoll(data["pp_rank"].get<string>()); }
				catch (std::exception) { info->global_rank = 0; }
				try { info->level = stof(data["level"].get<string>()); }
				catch (std::exception) { info->level = 0.0; }
				try { info->pp = stof(data["pp_raw"].get<string>()); }
				catch (std::exception) { info->pp = 0.0; }
				try { info->accuracy = stod(data["accuracy"].get<string>()); }
				catch (std::exception) { info->accuracy = 0.0; }
				try { info->count_ss = stoi(data["count_rank_ss"].get<string>()); }
				catch (std::exception) { info->count_ss = 0; }
				try { info->count_ssh = stoi(data["count_rank_ssh"].get<string>()); }
				catch (std::exception) { info->count_ssh = 0; }
				try { info->count_s = stoi(data["count_rank_s"].get<string>()); }
				catch (std::exception) { info->count_s = 0; }
				try { info->count_sh = stoi(data["count_rank_sh"].get<string>()); }
				catch (std::exception) { info->count_sh = 0; }
				try { info->count_a = stoi(data["count_rank_a"].get<string>()); }
				catch (std::exception) { info->count_a = 0; }
				try { info->playtime = stoll(data["total_seconds_played"].get<string>()); }
				catch (std::exception) { info->playtime = 0; }
				try { info->country_rank = stoll(data["pp_country_rank"].get<string>()); }
				catch (std::exception) { info->country_rank = 0; }
				info->country = data["country"].get<string>();
				info->mode = (int)mode;
				return 1;
			}
			return 0;
		}
		static int GetBeatmap(int64_t bid, beatmap_info* info) {
			char url[512];
			sprintf_s(url, 512, OSU_API_V1 "get_beatmaps?k=%s&b=%I64d", OSU_KEY, bid);
			string response;
			response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response)[0];
				info->approved = (beatmap_status)stoi(data["approved"].get<string>());
				tm time_temp;
				string temp_str;
				if (!data["submit_date"].is_null()) {
					utils::fakestrptime(data["submit_date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
					info->submit_timestamp = mktime(&time_temp) + 8 * 3600;
				}
				else {
					info->submit_timestamp = -1;
				}
				if (!data["approved_date"].is_null()) {
					utils::fakestrptime(data["approved_date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
					info->approved_timestamp = mktime(&time_temp) + 8 * 3600;
				}
				else {
					info->approved_timestamp = -1;
				}
				if (!data["last_update"].is_null()) {
					utils::fakestrptime(data["last_update"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
					info->lastupdate_timestamp = mktime(&time_temp) + 8 * 3600;
				}
				else {
					info->lastupdate_timestamp = -1;
				}
				info->artist = data["artist"].get<string>();
				info->title = data["title"].get<string>();
				info->beatmap_id = stoi(data["beatmap_id"].get<string>());
				info->beatmapset_id = stoi(data["beatmapset_id"].get<string>());
				info->bpm = stof(data["bpm"].get<string>());
				info->creator = data["creator"].get<string>();
				info->creator_id = stoi(data["creator_id"].get<string>());
				info->stars = stod(data["difficultyrating"].get<string>());
				try { info->aim_stars = stod(data["diff_aim"].get<string>()); }
				catch (std::exception) { info->aim_stars = 0.0; }
				try { info->speed_stars = stod(data["diff_speed"].get<string>()); }
				catch (std::exception) { info->speed_stars = 0.0; }
				info->cs = stof(data["diff_size"].get<string>());
				info->ar = stof(data["diff_approach"].get<string>());
				info->od = stof(data["diff_overall"].get<string>());
				info->hp = stof(data["diff_drain"].get<string>());
				info->game_length = stoi(data["hit_length"].get<string>());
				info->total_length = stoi(data["total_length"].get<string>());
				info->version = data["version"].get<string>();
				info->filemd5 = data["file_md5"].get<string>();
				info->mode = (mode)stoi(data["mode"].get<string>());
				info->tags.clear();
				stringstream ss(data["tags"].get<string>());
				string temp;
				while (!ss.eof()) {
					ss >> temp;
					info->tags.push_back(temp);
				}
				info->favourite_count = stoi(data["favourite_count"].get<string>());
				info->rating = stof(data["rating"].get<string>());
				info->playcount = stoi(data["playcount"].get<string>());
				info->passcount = stoi(data["passcount"].get<string>());
				info->count_circle = stoi(data["count_normal"].get<string>());
				info->count_slider = stoi(data["count_slider"].get<string>());
				info->count_spinner = stoi(data["count_spinner"].get<string>());
				try { info->maxcombo = stoi(data["max_combo"].get<string>()); }
				catch (std::exception) { info->maxcombo = 0; }
				info->hasStoryboard = stoi(data["storyboard"].get<string>());
				info->hasVideo = stoi(data["video"].get<string>());
				info->canDownload = stoi(data["download_unavailable"].get<string>()) != 0;
				return 1;
			}
			return 0;
		}
		static int GetBeatmapset(int64_t sid, vector<beatmap_info>& info) {
			info.clear();
			char url[512];
			sprintf_s(url, 512, OSU_API_V1 "get_beatmaps?k=%s&s=%I64d", OSU_KEY, sid);
			string response;
			response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response);
				for (auto it : data) {
					beatmap_info single;
					single.approved = (beatmap_status)stoi(it["approved"].get<string>());
					tm time_temp;
					string temp_str;
					if (!it["submit_date"].is_null()) {
						utils::fakestrptime(it["submit_date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
						single.submit_timestamp = mktime(&time_temp) + 16 * 3600;
					}
					else {
						single.submit_timestamp = -1;
					}
					if (!it["approved_date"].is_null()) {
						utils::fakestrptime(it["approved_date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
						single.approved_timestamp = mktime(&time_temp) + 16 * 3600;
					}
					else {
						single.approved_timestamp = -1;
					}
					if (!it["last_update"].is_null()) {
						utils::fakestrptime(it["last_update"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
						single.lastupdate_timestamp = mktime(&time_temp) + 16 * 3600;
					}
					else {
						single.lastupdate_timestamp = -1;
					}
					single.artist = data["artist"].get<string>();
					single.title = data["title"].get<string>();
					single.beatmap_id = stoi(it["beatmap_id"].get<string>());
					single.beatmapset_id = stoi(it["beatmapset_id"].get<string>());
					single.bpm = stof(it["bpm"].get<string>());
					single.creator = it["creator"].get<string>();
					single.creator_id = stoi(it["creator_id"].get<string>());
					single.stars = stod(it["difficultyrating"].get<string>());
					try { single.aim_stars = stod(it["diff_aim"].get<string>()); }
					catch (std::exception) { single.aim_stars = 0.0; }
					try { single.speed_stars = stod(it["diff_speed"].get<string>()); }
					catch (std::exception) { single.speed_stars = 0.0; }
					single.cs = stof(it["diff_size"].get<string>());
					single.ar = stof(it["diff_approach"].get<string>());
					single.od = stof(it["diff_overall"].get<string>());
					single.hp = stof(it["diff_drain"].get<string>());
					single.game_length = stoi(it["hit_length"].get<string>());
					single.total_length = stoi(it["total_length"].get<string>());
					single.version = it["version"].get<string>();
					single.filemd5 = it["file_md5"].get<string>();
					single.mode = (mode)stoi(it["mode"].get<string>());
					single.tags.clear();
					stringstream ss(it["tags"].get<string>());
					string temp;
					while (!ss.eof()) {
						ss >> temp;
						single.tags.push_back(temp);
					}
					single.favourite_count = stoi(it["favourite_count"].get<string>());
					single.rating = stof(it["rating"].get<string>());
					single.playcount = stoi(it["playcount"].get<string>());
					single.passcount = stoi(it["passcount"].get<string>());
					single.count_circle = stoi(it["count_normal"].get<string>());
					single.count_slider = stoi(it["count_slider"].get<string>());
					single.count_spinner = stoi(it["count_spinner"].get<string>());
					try { single.maxcombo = stoi(it["max_combo"].get<string>()); }
					catch (std::exception) { single.maxcombo = 0; }
					single.hasStoryboard = stoi(it["storyboard"].get<string>());
					single.hasVideo = stoi(it["video"].get<string>());
					single.canDownload = stoi(it["download_unavailable"].get<string>()) != 0;
					info.push_back(single);
				}
				return 1;
			}
			return 0;
		}
		static int GetUserRecent(int64_t uid, mode mode, score_info* info) {
			char url[512];
			sprintf_s(url, 512, OSU_API_V1 "get_user_recent?type=id&k=%s&u=%I64d&m=%d", OSU_KEY, uid, (int)mode);
			string response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response)[0];
				info->beatmap_id = stol(data["beatmap_id"].get<string>());
				info->score = stoll(data["score"].get<string>());
				info->combo = stoi(data["maxcombo"].get<string>());
				info->n50 = stoi(data["count50"].get<string>());
				info->n100 = stoi(data["count100"].get<string>());
				info->n300 = stoi(data["count300"].get<string>());
				info->nkatu = stoi(data["countkatu"].get<string>());
				info->ngeki = stoi(data["countgeki"].get<string>());
				info->nmiss = stoi(data["countmiss"].get<string>());
				info->mods = stoll(data["enabled_mods"].get<string>());
				info->user_id = stoll(data["user_id"].get<string>());
				info->rank = data["rank"].get<string>();
				tm time_temp;
				utils::fakestrptime(data["date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
				info->achieved_timestamp = (long)mktime(&time_temp) + 16 * 3600;
				return 1;
			}
			return 0;
		}
		static int GetUserRecent(const string& username, mode mode, score_info* info) {
			char url[512];
			sprintf_s(url,
				512,
				OSU_API_V1 "get_user_recent?type=string&k=%s&u=%s&m=%d",
				OSU_KEY,
				username.c_str(),
				(int)mode);
			string response;
			response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response)[0];
				info->beatmap_id = stoi(data["beatmap_id"].get<string>());
				info->score = stoll(data["score"].get<string>());
				info->combo = stoi(data["maxcombo"].get<string>());
				info->n50 = stoi(data["count50"].get<string>());
				info->n100 = stoi(data["count100"].get<string>());
				info->n300 = stoi(data["count300"].get<string>());
				info->nkatu = stoi(data["countkatu"].get<string>());
				info->ngeki = stoi(data["countgeki"].get<string>());
				info->nmiss = stoi(data["countmiss"].get<string>());
				info->mods = stoll(data["enabled_mods"].get<string>());
				info->user_id = stoll(data["user_id"].get<string>());
				info->rank = data["rank"].get<string>();
				tm time_temp;
				utils::fakestrptime(data["date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
				info->achieved_timestamp = (long)mktime(&time_temp) + 16 * 3600;
				return 1;
			}
			return 0;
		}
		static int GetUserBest(int64_t uid, int count, mode mode, vector<score_info>& info) {
			char url[512];
			sprintf_s(url,
				512,
				OSU_API_V1 "get_user_best?type=id&k=%s&u=%I64d&m=%d&limit=%d",
				OSU_KEY,
				uid,
				(int)mode,
				count);
			string response;
			response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response);
				info.clear();
				for (auto it : data) {
					score_info single;
					single.beatmap_id = stoi(it["beatmap_id"].get<string>());
					single.score = stoll(it["score"].get<string>());
					single.combo = stoi(it["maxcombo"].get<string>());
					single.n50 = stoi(it["count50"].get<string>());
					single.n100 = stoi(it["count100"].get<string>());
					single.n300 = stoi(it["count300"].get<string>());
					single.nkatu = stoi(it["countkatu"].get<string>());
					single.ngeki = stoi(it["countgeki"].get<string>());
					single.nmiss = stoi(it["countmiss"].get<string>());
					single.mods = stoi(it["enabled_mods"].get<string>());
					single.user_id = stoi(it["user_id"].get<string>());
					single.rank = it["rank"].get<string>();
					single.pp = stod(it["pp"].get<string>());
					tm time_temp;
					utils::fakestrptime(it["date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
					single.achieved_timestamp = (long)mktime(&time_temp) + 16 * 3600;
					info.push_back(single);
				}
				return info.size();
			}
			return 0;
		}
		static int GetUserBest(const string& username, int count, mode mode, vector<score_info>& info) {
			char url[512];
			sprintf_s(url,
				512,
				OSU_API_V1 "get_user_best?type=string&k=%s&u=%s&m=%d&limit=%d",
				OSU_KEY,
				username.c_str(),
				(int)mode,
				count);
			string response;
			response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response);
				info.clear();
				for (auto it : data) {
					score_info single;
					single.beatmap_id = stoi(it["beatmap_id"].get<string>());
					single.score = stoll(it["score"].get<string>());
					single.combo = stoi(it["maxcombo"].get<string>());
					single.n50 = stoi(it["count50"].get<string>());
					single.n100 = stoi(it["count100"].get<string>());
					single.n300 = stoi(it["count300"].get<string>());
					single.nkatu = stoi(it["countkatu"].get<string>());
					single.ngeki = stoi(it["countgeki"].get<string>());
					single.nmiss = stoi(it["countmiss"].get<string>());
					single.mods = stoi(it["enabled_mods"].get<string>());
					single.user_id = stoi(it["user_id"].get<string>());
					single.rank = it["rank"].get<string>();
					single.pp = stod(it["pp"].get<string>());
					tm time_temp;
					utils::fakestrptime(it["date"].get<string>().c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
					single.achieved_timestamp = (long)mktime(&time_temp) + 16 * 3600;
					info.push_back(single);
				}
				return info.size();
			}
			return 0;
		}
		static int GetUserScore(int64_t uid, mode mode, int64_t beatmapid, score_info* info) {
			char url[512];
			sprintf_s(url,
				512,
				OSU_API_V1 "get_scores?type=id&k=%s&u=%lld&m=%d&limit=%d&b=%lld",
				OSU_KEY,
				uid,
				(int)mode,
				1,
				beatmapid);
			string response = NetConnection::HttpsGet(url);
			if (response.length() > 50) {
				json data = json::parse(response)[0];
				info->beatmap_id = beatmapid;
				try { info->score = stoll(data["score"].get<string>()); }
				catch (exception) { info->score = 0; }
				try { info->combo = stoi(data["maxcombo"].get<string>()); }
				catch (exception) { info->combo = 0; }
				try { info->n50 = stoi(data["count50"].get<string>()); }
				catch (exception) { info->n50 = 0; }
				try { info->n100 = stoi(data["count100"].get<string>()); }
				catch (exception) { info->n100 = 0; }
				try { info->n300 = stoi(data["count300"].get<string>()); }
				catch (exception) { info->n300 = 0; }
				try { info->nkatu = stoi(data["countkatu"].get<string>()); }
				catch (exception) { info->nkatu = 0; }
				try { info->ngeki = stoi(data["countgeki"].get<string>()); }
				catch (exception) { info->ngeki = 0; }
				try { info->nmiss = stoi(data["countmiss"].get<string>()); }
				catch (exception) { info->nmiss = 0; }
				try { info->mods = stoi(data["enabled_mods"].get<string>()); }
				catch (exception) { info->mods = 0; }
				try { info->user_id = stoll(data["user_id"].get<string>()); }
				catch (exception) { info->user_id = 0; }
				try { info->username = data["username"].get<string>(); }
				catch (exception) { info->username = "error"; }
				try { info->rank = data["rank"].get<string>(); }
				catch (exception) { info->rank = "D"; }
				try { info->pp = stol(data["pp"].get<string>()); }
				catch (exception) { info->pp = 0.0; }
				tm time_temp;
				string date;
				try { date = data["date"].get<string>(); }
				catch (exception) { date = "2020-01-01 00:00:00"; }
				utils::fakestrptime(date.c_str(), "%Y-%m-%d %H:%M:%S", &time_temp);
				info->achieved_timestamp = (long)mktime(&time_temp) + 16 * 3600;
				return 1;
			}
			return 0;
		}
	};
}

#endif
