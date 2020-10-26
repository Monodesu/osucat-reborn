#pragma once

#define STEAM_WEB_API_KEY "A6572E63E138CEBB7F1119E7C6BEA492"

namespace osucat::steamcheck {
	struct UserStatus {
		int64_t steamid;
		int communityvisibilitystate;
		int profilestate;
		string personaname;
		int commentpermission;
		string profileurl;
		string avatar;
		string avatarmedium;
		string avatarfull;
		string avatarhash;
		int64_t lastlogoff;
		int personastate;
		string realname;
		string primaryclanid;
		int64_t timecreated;
		int personastateflags;
		string loccountrycode;
	};
	struct UserBanStats
	{
		int64_t SteamId;
		bool CommunityBanned;
		bool VACBanned;
		int NumberOfVACBans;
		int NumberOfGameBans;
		int DaysSinceLastBan;
		string EconomyBan;
	};

	class csgocheck {
	public:
		static bool cmdParse(string msg, Target tar, SenderInfo sdr, string* params) {
			if (_stricmp(msg.substr(0, 7).c_str(), "s check") == 0) {
				string tmp = msg.substr(7);
				utils::trim(tmp);
				if (tmp.empty()) {
					*params = u8"请提供要查询的信息，URL/SteamId/VanityURL/VantiyURLName均可";
					return true;
				}
				if (tmp.find("steamcommunity.com/id/") != string::npos) {
					string t = tmp.substr(tmp.find("/id/") + 4);
					utils::string_replace(t, "/", "");
					string source = NetConnection::HttpGet("http://api.steampowered.com/ISteamUser/ResolveVanityURL/v0001/?key="  STEAM_WEB_API_KEY "&vanityurl=" + t);
					json j = json::parse(source)["response"];
					if (j["success"].get<int>() == 1) {
						UserBanStats ubs = { 0 };
						if (ban_check(stoll(j["steamid"].get<string>()), &ubs)) {
							UserStatus us = get_user_status(stoll(j["steamid"].get<string>()));
							*params = checkrtnmsg(us, ubs);
							return true;
						}
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				if (tmp.find("steamcommunity.com/profiles/") != string::npos) {
					string t = tmp.substr(tmp.find("/profiles/") + 10);
					utils::string_replace(t, "/", "");
					UserBanStats ubs = { 0 };
					if (ban_check(stoll(t), &ubs)) {
						UserStatus us = get_user_status(stoll(t));
						*params = checkrtnmsg(us, ubs);
						return true;
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				if (utils::isNum(tmp) && tmp.length() > 13) {
					UserBanStats ubs = { 0 };
					if (ban_check(stoll(tmp), &ubs)) {
						UserStatus us = get_user_status(stoll(tmp));
						*params = checkrtnmsg(us, ubs);
						return true;
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				else {
					json j = json::parse(NetConnection::HttpGet("http://api.steampowered.com/ISteamUser/ResolveVanityURL/v0001/?key=" STEAM_WEB_API_KEY "&vanityurl=" + tmp))["response"];
					if (j["success"].get<int>() == 1) {
						UserBanStats ubs = { 0 };
						if (ban_check(stoll(j["steamid"].get<string>()), &ubs)) {
							UserStatus us = get_user_status(stoll(j["steamid"].get<string>()));
							*params = checkrtnmsg(us, ubs);
							return true;
						}
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
			}
			if (_stricmp(msg.substr(0, 8).c_str(), "s listen") == 0) {
				string tmp = msg.substr(8);
				utils::trim(tmp);
				if (tmp.empty()) {
					*params = u8"请提供要查询的信息，URL/SteamId/VanityURL/VantiyURLName均可";
					return true;
				}
				int64_t steamid;
				if (tmp.find("steamcommunity.com/id/") != string::npos) {
					string t = tmp.substr(tmp.find("/id/") + 4);
					utils::string_replace(t, "/", "");
					string source = NetConnection::HttpGet("http://api.steampowered.com/ISteamUser/ResolveVanityURL/v0001/?key="  STEAM_WEB_API_KEY "&vanityurl=" + t);
					json j = json::parse(source)["response"];
					if (j["success"].get<int>() == 1) {
						steamid = stoll(j["steamid"].get<string>());
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				if (tmp.find("steamcommunity.com/profiles/") != string::npos) {
					string t = tmp.substr(tmp.find("/profiles/") + 10);
					utils::string_replace(t, "/", "");
					UserBanStats ubs = { 0 };
					if (ban_check(stoll(t), &ubs)) {
						steamid = stoll(t);
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				if (utils::isNum(tmp) && tmp.length() > 13) {
					UserBanStats ubs = { 0 };
					if (ban_check(stoll(tmp), &ubs)) {
						steamid = stoll(tmp);
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				else {
					json j = json::parse(NetConnection::HttpGet("http://api.steampowered.com/ISteamUser/ResolveVanityURL/v0001/?key=" STEAM_WEB_API_KEY "&vanityurl=" + tmp))["response"];
					if (j["success"].get<int>() == 1) {
						steamid = stoll(j["steamid"].get<string>());
					}
					else {
						*params = u8"未找到此用户";
						return true;
					}
				}
				Database db;
				db.Connect();
				if (tar.message_type == Target::MessageType::GROUP) {
					if (db.steam_add_ban_listening(steamid, tar.user_id, tar.group_id)) {
						*params = u8"用户已成功添加到监听列表，将会在其受到封禁后推送至本群";
					}
					else {
						if (db.steam_change_ban_listening_receive_groupid(steamid, tar.group_id)) {
							*params = u8"用户已成功添加到监听列表，将会在其受到封禁后推送至本群";
						}
						else {
							if (db.steam_change_ban_listening_receive_userid(steamid, tar.user_id)) {
								*params = u8"该用户已存在于群推送列表中，已将其加入您的私聊推送列表";
							}
							else {
								*params = u8"该用户已存在于您的推送列表中";
							}
						}
					}
					return true;
				}
				else {
					if (db.steam_add_ban_listening(steamid, tar.user_id)) {
						*params = u8"用户已成功添加到监听列表，将会在其受到封禁后通过私聊推送";
					}
					else {
						if (db.steam_change_ban_listening_receive_userid(steamid, tar.user_id)) {
							*params = u8"该用户已存在于群推送列表中，已将其加入您的私聊推送列表";
						}
						else {
							*params = u8"该用户已存在于您的推送列表中";
						}
					}
					return true;
				}
			}
			return false;
		}
		static string checkrtnmsg(UserStatus us, UserBanStats ubs) {
			string returnmsg, radstr = osucat::utils::rand_str();
			string path = to_string(OC_ROOT_PATH) + "\\data\\images\\osucat\\" + radstr + ".png";
			if (us.avatarfull != "null") {
				NetConnection::DownloadFile(us.avatarfull, path);
				returnmsg += u8"[CQ:image,file=osucat\\" + radstr + ".png]" + "\n";
			}
			if (utils::forbiddenWordsLibrary(us.personaname)) {
				returnmsg += u8"用户名：*违禁词屏蔽*";
			}
			else returnmsg += u8"用户名：" + us.personaname;
			if (us.realname != "null") {
				if (utils::forbiddenWordsLibrary(us.realname)) {
					returnmsg += u8"\n真实姓名：*违禁词屏蔽*";
				}
				else returnmsg += u8"\n真实姓名：" + us.realname;
			}
			if (us.loccountrycode != "null") {
				returnmsg += u8"\n地区：" + utils::getCountryNameByCode(us.loccountrycode);
			}
			if (us.lastlogoff == 0) {
				returnmsg += u8"\n这位用户已经很久没有登陆了";
			}
			else returnmsg += u8"\n最后登录于：" + utils::unixTime2Str(us.lastlogoff);
			returnmsg += u8"\nSteamID：" + to_string(us.steamid);
			if (ubs.CommunityBanned) {
				returnmsg += u8"\n用户当前已被社区封禁";
			}
			if (ubs.EconomyBan != "none") {
				returnmsg += u8"\n用户当前已被禁止交易";
			}
			if (ubs.VACBanned) {
				returnmsg += u8"\nVAC封禁：是";
			}
			if (ubs.NumberOfGameBans > 0) {
				returnmsg += u8"\n游戏封禁：是";
			}
			if (ubs.VACBanned) {
				returnmsg += u8"\nVAC封禁次数：" + to_string(ubs.NumberOfVACBans);
			}
			if (ubs.NumberOfGameBans > 0) {
				returnmsg += u8"\n游戏封禁次数：" + to_string(ubs.NumberOfGameBans);
			}
			if (ubs.VACBanned || ubs.NumberOfGameBans > 0) {
				returnmsg += u8"\n上次封禁于：" + to_string(ubs.DaysSinceLastBan) + u8" 天前";
			}
			if (ubs.CommunityBanned == false && ubs.EconomyBan == "none" && ubs.NumberOfGameBans == 0 && ubs.VACBanned == false) {
				returnmsg += u8"\n该用户无任何不良记录。";
			}
			if (us.profileurl.find("/profiles/") == string::npos) {
				returnmsg += u8"\n自定义链接：" + us.profileurl;
			}
			else returnmsg += u8"\n主页链接：" + us.profileurl;
			thread DFH(bind(&DelFileHandler, path, 20));
			DFH.detach();
			return returnmsg;
		}
		static void _AutoCheck() {
			thread csgocheck_autocheck(bind(&autocheck));
			csgocheck_autocheck.detach();
		}
		static void autocheck() {
			cout << u8"steam bans auto-check thread created." << endl;
			Sleep(1000 * 10);
			while (true) {
				Database db;
				db.Connect();
				vector<CSGOUserInfo> cui;
				cui = db.steam_get_csgo_listen_list(); 
				if (cui[0].SteamId != -1) {				
					for (int i = 0; i < cui.size(); ++i) {
						thread autobancheckt(bind(&checkthread, cui[i]));
						autobancheckt.detach();
						Sleep(2000);
					}
				}
				Sleep(1000 * 60 * 10);
			}
		}
		static void checkthread(CSGOUserInfo cui) {
			UserBanStats ubs = { 0 };
			bool isbanned = false;
			UserStatus us;
			if (ban_check(cui.SteamId, &ubs)) {
				if (ubs.VACBanned) {
					isbanned = true;
					us = get_user_status(cui.SteamId);
					if (cui.ReceiveGroupId.find(';') == string::npos) {
						if (cui.ReceiveGroupId != "null") {
							send_message(
								Target::MessageType::GROUP,
								stoll(cui.ReceiveGroupId),
								banrtnmsg(us, ubs, true));
							Sleep(1000);
						}
					}
					else {
						vector<string> b = utils::string_split(cui.ReceiveGroupId, ';');
						for (int i = 0; i < b.size(); ++i) {
							send_message(
								Target::MessageType::GROUP,
								stoll(b[i]),
								banrtnmsg(us, ubs, true));
							Sleep(1000);
						}
					}
					if (cui.ReceiveUserId.find(';') == string::npos) {
						if (cui.ReceiveUserId != "null") {
							send_message(
								Target::MessageType::PRIVATE,
								stoll(cui.ReceiveUserId),
								banrtnmsg(us, ubs, true));
						}
					}
					else {
						vector<string> b = utils::string_split(cui.ReceiveUserId, ';');
						for (int i = 0; i < b.size(); ++i) {
							send_message(
								Target::MessageType::PRIVATE,
								stoll(b[i]),
								banrtnmsg(us, ubs, true));
							Sleep(1000);
						}
					}
					Database db;
					db.Connect();
					db.steam_change_ban_stats(ubs.SteamId);
				}
				if (ubs.NumberOfGameBans > 0) {
					if (!isbanned)us = get_user_status(cui.SteamId);
					if (cui.ReceiveGroupId.find(';') != string::npos) {
						if (cui.ReceiveGroupId != "null") {
							send_message(
								Target::MessageType::GROUP,
								stoll(cui.ReceiveGroupId),
								banrtnmsg(us, ubs, false));
							Sleep(1000);
						}
					}
					else {
						vector<string> b = utils::string_split(cui.ReceiveGroupId, ';');
						for (int i = 0; i < b.size(); ++i) {
							send_message(
								Target::MessageType::GROUP,
								stoll(b[i]),
								banrtnmsg(us, ubs, false));
							Sleep(1000);
						}
					}
					if (cui.ReceiveUserId.find(';') != string::npos) {
						if (cui.ReceiveUserId != "null") {
							send_message(
								Target::MessageType::PRIVATE,
								stoll(cui.ReceiveUserId),
								banrtnmsg(us, ubs, false));
						}
					}
					else {
						vector<string> b = utils::string_split(cui.ReceiveUserId, ';');
						for (int i = 0; i < b.size(); ++i) {
							send_message(
								Target::MessageType::PRIVATE,
								stoll(b[i]),
								banrtnmsg(us, ubs, false));
							Sleep(1000);
						}
					}
					Database db;
					db.Connect();
					db.steam_change_ban_stats(ubs.SteamId);
				}
			}
		}
		static string banrtnmsg(UserStatus us, UserBanStats ubs, bool VACorGameBan) {
			string returnmsg;// radstr = osucat::utils::rand_str();
			/*string path = to_string(OC_ROOT_PATH) + "\\data\\images\\osucat\\" + radstr + ".png";
			if (us.avatarfull != "null") {
				NetConnection::DownloadFile(us.avatarfull, path);
				returnmsg += u8"[CQ:image,file=osucat\\" + radstr + ".png]" + "\n";
			}*/
			if (VACorGameBan) {
				returnmsg += u8"查询到一项新的VAC封禁 详细信息如下：";
			}
			else returnmsg += u8"查询到一项新的游戏封禁 详细信息如下：";
			if (utils::forbiddenWordsLibrary(us.personaname)) {
				returnmsg += u8"\n用户名：*违禁词屏蔽*";
			}
			else returnmsg += u8"\n用户名：" + us.personaname;
			if (us.realname != "null") {
				if (utils::forbiddenWordsLibrary(us.realname)) {
					returnmsg += u8"\n真实姓名：*违禁词屏蔽*";
				}
				else returnmsg += u8"\n真实姓名：" + us.realname;
			}
			if (us.loccountrycode != "null") {
				returnmsg += u8"\n地区：" + utils::getCountryNameByCode(us.loccountrycode);
			}
			if (us.lastlogoff == 0) {
				returnmsg += u8"\n该用户已经很久没有登陆";
			}
			else returnmsg += u8"\n最后登录于：" + utils::unixTime2Str(us.lastlogoff);
			returnmsg += u8"\nSteamID：" + to_string(us.steamid);
			if (ubs.CommunityBanned) {
				returnmsg += u8"\n用户当前已被社区封禁";
			}
			if (ubs.EconomyBan != "none") {
				returnmsg += u8"\n用户当前已被禁止交易";
			}
			if (ubs.VACBanned) {
				returnmsg += u8"\nVAC封禁：是";
			}
			if (ubs.NumberOfGameBans > 0) {
				returnmsg += u8"\n游戏封禁：是";
			}
			if (ubs.VACBanned) {
				returnmsg += u8"\nVAC封禁次数：" + to_string(ubs.NumberOfVACBans);
			}
			if (ubs.NumberOfGameBans > 0) {
				returnmsg += u8"\n游戏封禁次数：" + to_string(ubs.NumberOfGameBans);
			}
			if (ubs.VACBanned || ubs.NumberOfGameBans > 0) {
				returnmsg += u8"\n上次封禁于：" + to_string(ubs.DaysSinceLastBan) + u8" 天前";
			}
			if (ubs.CommunityBanned == false && ubs.EconomyBan == "none" && ubs.NumberOfGameBans == 0 && ubs.VACBanned == false) {
				returnmsg += u8"\n该用户无任何不良记录。";
			}
			if (us.profileurl.find("/profiles/") == string::npos) {
				returnmsg += u8"\n自定义链接：" + us.profileurl;
			}
			else returnmsg += u8"\n主页链接：" + us.profileurl;
			/*thread DFH(bind(&DelFileHandler, path, 20));
			DFH.detach();*/
			return returnmsg;
		}
	private:
		static void DelFileHandler(string filename, int delayTime = 0) {
			if (delayTime > 0)Sleep(delayTime * 1000);
			DeleteFileA(filename.c_str());
		}
		static UserStatus get_user_status(int64_t SteamId) {
			char tmp[1024];
			sprintf_s(tmp, "http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key=%s&steamids=%lld", STEAM_WEB_API_KEY, SteamId);
			json j = json::parse(NetConnection::HttpGet(tmp))["response"]["players"][0];
			UserStatus us;
			us.steamid = stoll(j["steamid"].get<string>());
			try { us.communityvisibilitystate = j["communityvisibilitystate"].get<int>(); }
			catch (std::exception) { us.communityvisibilitystate = 0; }
			try { us.profilestate = j["profilestate"].get<int>(); }
			catch (std::exception) { us.profilestate = 0; }
			try { us.personaname = j["personaname"].get<string>(); }
			catch (std::exception) { us.personaname = "null"; }
			try { us.commentpermission = j["commentpermission"].get<int>(); }
			catch (std::exception) { us.commentpermission = 0; }
			try { us.profileurl = j["profileurl"].get<string>(); }
			catch (std::exception) { us.profileurl = "null"; }
			try { us.avatar = j["avatar"].get<string>(); }
			catch (std::exception) { us.avatar = "null"; }
			try { us.avatarmedium = j["avatarmedium"].get<string>(); }
			catch (std::exception) { us.avatarmedium = "null"; }
			try { us.avatarfull = j["avatarfull"].get<string>(); }
			catch (std::exception) { us.avatarfull = "null"; }
			try { us.avatarhash = j["avatarhash"].get<string>(); }
			catch (std::exception) { us.avatarhash = "null"; }
			try { us.lastlogoff = j["lastlogoff"].get<int64_t>(); }
			catch (std::exception) { us.lastlogoff = 0; }
			try { us.personastate = j["personastate"].get<int>(); }
			catch (std::exception) { us.personastate = 0; }
			try { us.realname = j["realname"].get<string>(); }
			catch (std::exception) { us.realname = "null"; }
			try { us.primaryclanid = j["primaryclanid"].get<string>(); }
			catch (std::exception) { us.primaryclanid = "null"; }
			try { us.timecreated = j["timecreated"].get<int64_t>(); }
			catch (std::exception) { us.timecreated = 0; }
			try { us.personastateflags = j["personastateflags"].get<int>(); }
			catch (std::exception) { us.personastateflags = 0; }
			try { us.loccountrycode = j["loccountrycode"].get<string>(); }
			catch (std::exception) { us.loccountrycode = "null"; }
			return us;
		}
		static bool ban_check(int64_t SteamId, UserBanStats* rtn) {
			try {
				string source = NetConnection::HttpGet("http://api.steampowered.com/ISteamUser/GetPlayerBans/v1/?key="  STEAM_WEB_API_KEY "&steamids=" + to_string(SteamId));
				json j = json::parse(source)["players"][0];
				UserBanStats ubs;
				ubs.SteamId = stoll(j["SteamId"].get<string>());
				ubs.CommunityBanned = j["CommunityBanned"].get<bool>();
				ubs.VACBanned = j["VACBanned"].get<bool>();
				ubs.NumberOfGameBans = j["NumberOfGameBans"].get<int>();
				ubs.NumberOfVACBans = j["NumberOfVACBans"].get<int>();
				ubs.DaysSinceLastBan = j["DaysSinceLastBan"].get<int>();
				ubs.EconomyBan = j["EconomyBan"].get<string>();
				*rtn = ubs;
			}
			catch (osucat::NetWork_Exception) {
				return false;
			}
			catch (std::exception& e) {
				return false;
			}
			return true;
		}
		static void send_message(Target::MessageType messagetype, int64_t recipient, const string message) {
			Target activepushTar;
			messagetype == Target::MessageType::GROUP ? activepushTar.message_type = Target::MessageType::GROUP : activepushTar.message_type = Target::MessageType::PRIVATE;
			messagetype == Target::MessageType::GROUP ? activepushTar.group_id = recipient : activepushTar.user_id = recipient;
			activepushTar.message = message;
			activepush(activepushTar);
		}
		static void activepush(Target tar) {
			if (tar.message_type == Target::MessageType::PRIVATE) {
				json jp;
				jp["user_id"] = tar.user_id;
				jp["message"] = tar.message;
				try {
					utils::fileExist(".\\.remote") ? NetConnection::HttpPost("http://192.168.0.103:5700/send_private_msg", jp) : NetConnection::HttpPost("http://127.0.0.1:5700/send_private_msg", jp);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[8192];
				sprintf_s(msg,
					u8"[%s] [osucat][↑]: 发送至好友 %lld 的消息：%s",
					utils::unixTime2Str(time(NULL)).c_str(),
					tar.user_id,
					tar.message.c_str());
				cout << msg << endl;
			}
			if (tar.message_type == Target::MessageType::GROUP) {
				json jp;
				jp["group_id"] = tar.group_id;
				jp["message"] = tar.message;
				try {
					utils::fileExist(".\\.remote") ? NetConnection::HttpPost("http://192.168.0.103:5700/send_group_msg", jp) : NetConnection::HttpPost("http://127.0.0.1:5700/send_group_msg", jp);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[8192];
				sprintf_s(msg,
					u8"[%s] [osucat][↑]: 发送至群 %lld 的消息：%s",
					utils::unixTime2Str(time(NULL)).c_str(),
					tar.group_id,
					tar.message.c_str());
				cout << msg << endl;
			}
		}
	};




}