#pragma once
#ifndef OSUCAT_ADDONS_HPP
#define OSUCAT_ADDONS_HPP

#include "games.hpp"

namespace osucat::addons {

	struct driftingBottle
	{
		int id;
		int pickcount;
		string msg;
		int64_t sender;
		string nickname;
		int sendTime;
	};

	class entertainment {
	public:
		static bool cmdParse(string msg, Target tar, SenderInfo senderinfo, string* params) {
			try {
				if (_stricmp(msg.substr(0, 18).c_str(), u8"剩余打捞次数") == 0) {
					Database db;
					db.Connect();
					int t = db.getUserBottleRemaining(tar.user_id);
					if (t > 0) {
						*params = u8"你当前还可以捡" + to_string(t) + u8"个瓶子！";
					}
					else {
						*params = u8"你当前没有捡瓶子的机会了，扔漂流瓶可以获得一次打捞的机会，或每日首次使用漂流瓶也可获得20次免费打捞的机会~";
					}
					return true;
				}
				if (_stricmp(msg.substr(0, 15).c_str(), u8"漂流瓶参数") == 0) {
					Database db;
					db.Connect();
					*params = db.getBottleStatistics();
					return true;
				}
				if (_stricmp(msg.substr(0, 21).c_str(), u8"海上漂流瓶数量") == 0) {
					Database db;
					db.Connect();
					int n = db.getNumberOfAvailableBottles();
					if (n == 0) {
						*params = u8"海上目前还没有漂流瓶呢...";
					}
					else {
						*params = u8"当前海上有 " + to_string(n) + u8" 个漂流瓶正在远航...";
					}
					return true;
				}
				if (_stricmp(msg.substr(0, 12).c_str(), u8"扔漂流瓶") == 0 || _stricmp(msg.substr(0, 12).c_str(), u8"丢漂流瓶") == 0) {
					if (driftingBottleVoid(true, msg.substr(12), tar, senderinfo, params))  return true; else return false;
				}
				if (_stricmp(msg.substr(0, 12).c_str(), u8"捡漂流瓶") == 0 || _stricmp(msg.substr(0, 12).c_str(), u8"捞漂流瓶") == 0) {
					if (driftingBottleVoid(false, msg.substr(12), tar, senderinfo, params))  return true; else return false;
				}
				if (_stricmp(msg.substr(0, 4).c_str(), "roll") == 0) {
					roll(msg.substr(4), tar, params);
					imagemonitor(msg, senderinfo, tar);
					return true;
				}
				if (_stricmp(msg.substr(0, 3).c_str(), "chp") == 0) {
					chp(params);
					return true;
				}
				/*if (_stricmp(msg.substr(0, 5).c_str(), "sleep") == 0) {
					sleep(params);
					return true;
				}*/
				if (_stricmp(msg.substr(0, 18).c_str(), u8"营销号生成器") == 0) {
					marketingGenerator(msg.substr(18), params);
					imagemonitor(msg, senderinfo, tar);
					return true;
				}
				if (_stricmp(msg.substr(0, 7).c_str(), "nbnhhsh") == 0) {
					nbnhhsh(msg.substr(7), params);
					imagemonitor(msg, senderinfo, tar);
					return true;
				}
				if (msg.find(u8"还是") != string::npos || msg.find(u8"不") != string::npos || msg.find(u8"没") != string::npos) {
					randEvents(msg, params);
					imagemonitor(msg, senderinfo, tar);
					return true;
				}
				/*if (_stricmp(msg.substr(0, 6).c_str(), u8"上号") == 0) {
					wyy(params);
					return true;
				}*/
				//网抑云
				if (_stricmp(msg.substr(0, 12).c_str(), u8"舔狗日记") == 0) {
					tgrj(params);
					return true;
				}
				//舔狗日记
				if (_stricmp(msg.substr(0, 7).c_str(), "cardimg") == 0) {
					return false;
					cardimagetest(msg.substr(7), tar, senderinfo, params);
					return true;
				}
				//猜单词游戏部分

				if (_stricmp(msg.substr(0, 15).c_str(), u8"猜单词帮助") == 0) {
					*params = u8"[CQ:image,file=osucat\\help\\hangmanhelp.png]";
					return true;
				}
				if (_stricmp(msg.substr(0, 15).c_str(), u8"猜单词状态") == 0) {
					HangmanGame::gameStatus(params);
					return true;
				}
				if (_stricmp(msg.substr(0, 18).c_str(), u8"猜单词排行榜") == 0) {
					int page = 1;
					string page_str = msg.substr(18);
					if (utils::isNum(page_str)) page = stoi(page_str);
					HangmanGame::hangmanRanking(page, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 10).c_str(), u8"猜单词+") == 0) {
					HangmanGame::startHangman(senderinfo, tar, msg.substr(10), params);
					return true;
				}
				if (_stricmp(msg.substr(0, 9).c_str(), u8"猜单词") == 0) {
					HangmanGame::startHangman(senderinfo, tar, "", params);
					return true;
				}
				if (_stricmp(msg.substr(0, 6).c_str(), u8"放弃") == 0) {
					HangmanGame::giveupHangman(senderinfo, tar, params);
					return true;
				}
				if (msg.length() == 1 && HangmanGame::findPlayer(tar.user_id) != -1) {
					HangmanGame::inputHangman(senderinfo, tar, msg, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 15).c_str(), u8"我的猜单词") == 0) {
					HangmanGame::showPlayerData(senderinfo, tar, params);
					return true;
				}
				if (_stricmp(msg.substr(0, 27).c_str(), "recalculateallhangmanscores") == 0) {
					for (int i = 0; i < adminlist.size(); i++) {
						if (tar.user_id == adminlist[i].user_id) {
							HangmanGame::recalculateAllScores(params);
							return true;
						}
					}
					*params = u8"权限不足";
					return true;
				}
			}
			catch (osucat::database_exception& ex) {
				*params = u8"访问数据库时出现了一个错误，请稍后重试...";
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"Mysql出现错误\n"
					u8"错误代码：%d\n"
					u8"详细信息：%s\n",
					utils::unixTime2Str(time(NULL)).c_str(),
					ex.Code(),
					ex.Info().c_str()
				);
				Target exceptionReport;
				exceptionReport.message_type = Target::MessageType::PRIVATE;
				exceptionReport.user_id = owner_userid;
				exceptionReport.message = reportMsg;
				activepush(exceptionReport);
				return true;
			}
			catch (osucat::NetWork_Exception& ex) {
				*params = u8"访问api时超时...请稍后重试...";
				return true;
			}
			catch (std::exception& ex) {
				*params = u8"addons模块出现了一个未知错误，请稍后重试...";
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"已捕获std::exception\n"
					u8"操作者：%lld\n"
					u8"触发指令：%s\n"
					u8"详细信息：%s\n",
					utils::unixTime2Str(time(NULL)).c_str(),
					tar.user_id,
					tar.message.c_str(),
					ex.what()
				);
				Target exceptionReport;
				exceptionReport.message_type = Target::MessageType::PRIVATE;
				exceptionReport.user_id = owner_userid;
				exceptionReport.message = reportMsg;
				activepush(exceptionReport);
				return true;
			}
			return false;
		}
		static void imagemonitor(string msg, SenderInfo senderinfo, Target tar) {
			if (msg.find("[CQ:image") != string::npos) {
				char reportMsg[1024];
				sprintf_s(reportMsg,
					"[%s]\n"
					u8"用户 %s(%lld) 上传了图片,消息内容如下：\n%s",
					utils::unixTime2Str(time(NULL)).c_str(),
					senderinfo.nickname.c_str(),
					tar.user_id,
					tar.message.c_str()
				);
				send_message(Target::MessageType::GROUP, management_groupid, reportMsg);
			}
		}
		static void roll(string cmd, Target tar, string* params) {
			cmd = utils::unescape(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) {
				//Target activepushTar;
				//activepushTar.message_type =  Target::MessageType::PRIVATE ;
				//activepushTar.user_id = MONO;
				//activepushTar.message = u8"用户 " + to_string(tar.user_id) + u8" 触发了违禁词。";
				//activepush(activepushTar);
				*params = u8"不想理你...";
				return;
			}
			if (cmd.length() > 199) {
				*params = u8"你想干嘛...";
				return;
			}
			utils::trim(cmd);
			int64_t value;
			if (cmd.empty()) {
				*params = to_string(utils::randomNum(1, 100));
				return;
			}
			if (utils::isNum(cmd)) {
				try {
					value = stoll(cmd);
					if (value < 1) value = 100;
				}
				catch (std::exception) {
					value = 100;
				}
				*params = to_string(utils::randomNum(1, value));
				return;
			}
			vector<string> temp = utils::string_split(cmd, ' ');
			if (temp.size() > 1) {
				*params = u8"当然是" + temp[utils::randomNum(1, (int)temp.size()) - 1] + u8"咯";
			}
			else
				*params = u8"需要两个或以上的参数啦..你给的不够喵...";
		}
		static void chp(string* params) {
			try {
				*params = NetConnection::HttpsGet("https://chp.shadiao.app/api.php");
			}
			catch (osucat::NetWork_Exception) {
				*params = u8"访问api时超时...请稍后再试...";
			}
		}
		static void sleep(string* params) {
			switch (utils::randomNum(1, 4)) {
			case 1:
				*params = u8"[CQ:record,file=osucat\\你怎么睡得着的.mp3]";
				break;
			case 2:
				*params = u8"[CQ:record,file=osucat\\睡不着啊 硬邦邦.mp3]";
				break;
			case 3:
				*params = u8"[CQ:record,file=osucat\\丁丁-睡不着啊.mp3]";
				break;
			case 4:
				*params = u8"[CQ:record,file=osucat\\小乐乐-睡觉啦不要聊天啦.mp3]";
				break;
			default:
				break;
			}
		}
		static void marketingGenerator(string cmd, string* params) {
			cmd = utils::unescape(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) {
				*params = u8"不想理你...";
				return;
			}
			if (cmd.length() > 400) {
				*params = u8"太长了！";
				return;
			}
			utils::trim(cmd);
			std::vector<std::string> temp = utils::string_split(cmd, '#');
			if (temp.size() != 3) {
				*params = u8"参数有误，生成失败。\n参数格式为 主体#事件#事件的另一种说法";
				return;
			}
			std::string 主体 = temp[0], 事件 = temp[1], 事件的另一种说法 = temp[2];
			char message[3072];
			sprintf_s(message,
				3072,
				u8"%s%s是怎么回事呢？%s相信大家都很熟悉，但是%s%s"
				u8"是怎么回事呢，下面就让小编带大家一起了解下吧。"
				u8"%s%s，其实就是%s，大家可能会很惊讶%s怎么"
				u8"%s呢？但事实就是这样，小编也感到非常惊讶。"
				u8"这就是关于%s%s的事情了，大家有什么想法呢，欢迎在评论区和小编一起讨论哦~",
				主体.c_str(),
				事件.c_str(),
				主体.c_str(),
				主体.c_str(),
				事件.c_str(),
				主体.c_str(),
				事件.c_str(),
				事件的另一种说法.c_str(),
				主体.c_str(),
				事件.c_str(),
				主体.c_str(),
				事件.c_str());
			*params = message;
		}
		static void randEvents(string cmd, string* params) {
			try {
				cmd = utils::unescape(cmd);
				if (utils::forbiddenWordsLibrary(cmd) == true) {
					*params = u8"不想理你...";
					return;
				}
				if (cmd.length() > 1024) {
					*params = u8"你想干嘛...";
					return;
				}
				utils::trim(cmd);
				utils::string_replace(cmd, u8"我", u8"{@@}");
				utils::string_replace(cmd, u8"你", u8"我");
				utils::string_replace(cmd, u8"{@@}", u8"你");
				if (cmd.substr(cmd.length() - 3, 3) == u8"？" || cmd.substr(cmd.length() - 1, 1) == "?")
					cmd = cmd.substr(cmd.length() - 3, 3) == u8"？" ? cmd.substr(0, cmd.length() - 3)
					: cmd.substr(0, cmd.length() - 1);
				if (cmd.substr(cmd.length() - 3, 3) == "吗" || cmd.substr(cmd.length() - 3, 3) == "呢")
					cmd = cmd.substr(0, cmd.length() - 3);
				if (cmd.find(u8"还是") != string::npos) {
					string str1, str2;
					str1 = cmd.substr(0, cmd.find(u8"还是"));
					str2 = cmd.substr(cmd.find(u8"还是") + 6);
					cout << str1 + " " << endl;
					cout << str2 + " " << endl;
					if (str1.empty() || str2.empty()) return;
					switch (utils::randomNum(1, 2)) {
					case 1:
						*params = str1;
						break;
					case 2:
						*params = str2;
						break;
					}
					return;
				}
				if (cmd.find(u8"不") != string::npos) {
					if ((cmd[cmd.find(u8"不") - 1] & 0x80) != 0) {
						//是汉字
						if (cmd.substr(cmd.find(u8"不") - 3, 3) == cmd.substr(cmd.find(u8"不") + 3, 3)) {
							//do something here
							string str1 = cmd.substr(0, cmd.find(u8"不") - 3), str2 = cmd.substr(cmd.find(u8"不") + 3);
							switch (utils::randomNum(1, 2)) {
							case 1:
								*params = str1 + str2;
								break;
							case 2:
								*params = str1 + u8"不" + str2;
								break;
							}
						}
					}
					else {
						//不是汉字
						if (cmd.substr(0, cmd.find(u8"不")) == cmd.substr(cmd.find(u8"不") + 3)) {
							//do something here
							string str1 = cmd.substr(0, cmd.find(u8"不")), str2 = cmd.substr(cmd.find(u8"不") + 3);
							switch (utils::randomNum(1, 2)) {
							case 1:
								*params = str2;
								break;
							case 2:
								*params = u8"不" + str2;
								break;
							}
						}
					}
				}
			}
			catch (std::exception& ex) {
				cout << ex.what() << endl;
			}
		}
		static void nbnhhsh(string cmd, string* params) {
			cmd = utils::unescape(cmd);
			utils::trim(cmd);
			if (utils::forbiddenWordsLibrary(cmd) == true) {
				*params = u8"不想理你...";
				return;
			}
			if (cmd.length() > 199) {
				*params = u8"太长了！";
				return;
			}
			if (cmd == "") {
				*params = u8"你都不告诉我你要翻译啥（恼）";
				return;
			}
			json jp;
			jp["text"] = cmd;
			try {
				string tmp1, tmp2 = NetConnection::HttpsPost("https://lab.magiconch.com/api/nbnhhsh/guess", jp).substr(1);
				tmp2 = tmp2.substr(0, tmp2.length() - 1);
				try {
					json j = json::parse(tmp2)["trans"];
					for (int ii = 1, i = 0; i < j.size(); ++i) {
						tmp2 = "\n[" + to_string(ii) + "] " + j[i].get<string>();
						if (!utils::forbiddenWordsLibrary(j[i].get<string>())) {
							if (ii < 10) {
								tmp1 += "\n/" + to_string(ii) + "/  " + j[i].get<string>();
							}
							else if (ii > 10) {
								break;
							}
							else {
								tmp1 += "\n/" + to_string(ii) + "/ " + j[i].get<string>();
							}
							++ii;
						}
					}
					if (tmp1 != "") {
						char trntmp[8192];
						//sprintf printf一类的 必须跟c_str不然乱码
						sprintf_s(trntmp,
							u8"以下是\"%s\"的人话：%s",
							cmd.c_str(),
							tmp1.c_str());
						*params = trntmp;
					}
					else {
						*params = u8"没有找到这堆缩写的翻译;w;";
					}
				}
				catch (json::exception) {
					//*params = u8"是不是写错了什么。。？再看看？";
					*params = u8"没有找到这堆缩写的翻译;w;";
					return;
				}
			}
			catch (osucat::NetWork_Exception) {
				*params = u8"获取翻译时超时...请稍后再试...";
				return;
			}
		}
		/* static void wyy(string* params) {
			try {
				*params = NetConnection::HttpsGet("https://api.moebot.im/wyy/");
			}
			catch (osucat::NetWork_Exception) {
				*params = u8"抑郁太多对身体不好...";
			}
		} */
		static void tgrj(string* params) {
			try {
				*params = NetConnection::HttpsGet("https://api.moebot.im/tg/");
			}
			catch (osucat::NetWork_Exception) {
				*params = u8"访问api时超时...请稍后再试...";
			}
		}
		static bool driftingBottleVoid(bool ThrowOrPick, string cmd, Target tar, SenderInfo senderinfo, string* params) {
			Database db;
			db.Connect();
			json j = db.getBottles();
			if (ThrowOrPick) {
				int throwcount = 0;
				for (int i = 0; i < j.size(); ++i) {
					if (stoll(j[i]["sender"].get<std::string>()) == tar.user_id) {
						++throwcount;
					}
				}
				if (throwcount > 20) {
					*params = u8"你已经扔了20个瓶子出去了...休息一下再扔吧...";
					return true;
				}
				cmd = utils::unescape(cmd);
				utils::trim(cmd);
				if (utils::forbiddenWordsLibrary(cmd) == true) {
					*params = u8"不想理你...";
					return true;
				}
				if (cmd.length() == 0) {
					*params = u8"不如写点什么再扔...?";
					return true;
				}
				if (cmd.length() > 5000) {
					*params = u8"太长了！";
					return true;
				}
				time_t timetmp = time(NULL);
				db.setBottleRemaining(2, tar.user_id);
				db.writeBottle(driftingBottleDBEvent::WRITEIN, 0, timetmp, tar.user_id, senderinfo.nickname, utils::ocescape(cmd));
				db.addPickThrowCount(false);
				char reportMsg[6000];
				if (cmd.find("[CQ:image") != string::npos) {
					send_message(tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP,
						tar.message_type == Target::MessageType::PRIVATE ? tar.user_id : tar.group_id, u8"你的漂流瓶已经漂往远方....");
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"用户 %s(%lld) 在漂流瓶内上传了图片\n漂流瓶ID: %d\n消息内容如下：\n%s",
						utils::unixTime2Str(time(NULL)).c_str(),
						senderinfo.nickname.c_str(),
						tar.user_id,
						db.getBottleID(tar.user_id, utils::ocescape(cmd), timetmp),
						tar.message.c_str()
					);
					send_message(Target::MessageType::GROUP, management_groupid, reportMsg);
				}
				else {
					send_message(tar.message_type == Target::MessageType::PRIVATE ? Target::MessageType::PRIVATE : Target::MessageType::GROUP,
						tar.message_type == Target::MessageType::PRIVATE ? tar.user_id : tar.group_id, u8"你的漂流瓶已经漂往远方....");
					sprintf_s(reportMsg,
						"[%s]\n"
						u8"用户 %s(%lld) 上传了漂流瓶\n漂流瓶ID: %d\n消息内容如下：\n%s",
						utils::unixTime2Str(time(NULL)).c_str(),
						senderinfo.nickname.c_str(),
						tar.user_id,
						db.getBottleID(tar.user_id, utils::ocescape(cmd), timetmp),
						tar.message.c_str()
					);
					send_message(Target::MessageType::GROUP, management_groupid, reportMsg);
				}
				return false;
			}

			int a = db.getUserBottleRemaining(tar.user_id);
			if (a > 0) {
				if (j.size() != 0) {
					db.setBottleRemaining(3, tar.user_id);
					int tempi = utils::randomNum(0, j.size() - 1);
					for (int i = 0; i < j.size(); ++i) {
						if (stoll(j[tempi]["sender"].get<std::string>()) != tar.user_id) {
							break;
						}
						else { Sleep(314); tempi = utils::randomNum(0, j.size() - 1); }
					}
					driftingBottle dfb;
					dfb.msg = utils::ocunescape(j[tempi]["message"].get<std::string>());
					dfb.nickname = j[tempi]["nickname"].get<std::string>();
					dfb.sender = stoll(j[tempi]["sender"].get<std::string>());
					dfb.sendTime = stoi(j[tempi]["sendtime"].get<std::string>());
					dfb.id = stoi(j[tempi]["id"].get<std::string>());
					dfb.pickcount = stoi(j[tempi]["pickcount"].get<std::string>());
					db.addPickThrowCount(true);
					db.writeBottle(osucat::addons::driftingBottleDBEvent::ADDCOUNTER, dfb.id, 0, 0, "", "");
					char tempm[6000];
					sprintf_s(tempm,
						u8"这是来自 %s(%lld) 的漂流瓶....\n"
						u8"ID:%d 发于 %s\n"
						u8"内容是....\n%s",
						dfb.nickname.c_str(), dfb.sender, dfb.id, utils::unixTime2StrChinese(dfb.sendTime).c_str(), dfb.msg.c_str());
					*params = tempm;
					if (db.RemoveBottle(floor((time(NULL) - stoll(j[tempi]["sendtime"].get<std::string>())) / 86400), dfb.id)) {
						Target tar1;
						tar1.user_id = dfb.sender;
						tar1.message_type = Target::MessageType::PRIVATE;
						if (dfb.pickcount == 0) {
							sprintf_s(tempm,
								u8"你发于 %s\n"
								u8"的内容为....%s的消息已经被 %s(%lld) 捞起来了....\n",
								utils::unixTime2StrChinese(dfb.sendTime).c_str(),
								dfb.msg.c_str(),
								senderinfo.nickname.c_str(), tar.user_id);
						}
						else {
							sprintf_s(tempm,
								u8"你发于 %s\n"
								u8"的内容为....%s的消息已经被 %s(%lld) 捞起来了....\n在此之前你的瓶子还被阅读了 %d 次...",
								utils::unixTime2StrChinese(dfb.sendTime).c_str(),
								dfb.msg.c_str(),
								senderinfo.nickname.c_str(), tar.user_id, dfb.pickcount);
						}
						tar1.message = tempm;
						activepush(tar1);
					}
				}
				else {
					*params = u8"还没有人丢过漂流瓶呢...";
				}
			}
			else {
				*params = u8"你当前没有捡瓶子的机会了，扔<一个>漂流瓶可以获得<两次>打捞的机会，或每日<首次>使用漂流瓶也可获得<10次>免费打捞的机会~";
			}
			return true;
		}
		static void cardimagetest(string cmd, Target tar, SenderInfo senderinfo, string* params) {
			*params = "[CQ:cardimage,file=https://i0.hdslb.com/bfs/article/f1fce3050d0184ac6830041a19aba2517f03c06a.jpg]";
		}
		private:
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
					char msg[4096];
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
					char msg[4096];
					sprintf_s(msg,
						u8"[%s] [osucat][↑]: 发送至群 %lld 的消息：%s",
						utils::unixTime2Str(time(NULL)).c_str(),
						tar.group_id,
						tar.message.c_str());
					cout << msg << endl;
				}
			}
			static void eventActivePush(Request req) {
				if (req.request_type == Request::RequestType::FRIEND) {
					json jp;
					jp["flag"] = req.flag;
					jp["approve"] = true;
					try {
						NetConnection::HttpPost("http://127.0.0.1:5700/set_friend_add_request", jp);
					}
					catch (osucat::NetWork_Exception& ex) {
						cout << ex.Show() << endl;
					}
					char msg[4096];
					sprintf_s(msg,
						u8"[%s] [osucat][↑]: 已通过来自 %lld 的好友请求。",
						utils::unixTime2Str(time(NULL)).c_str(),
						req.user_id);
					cout << msg << endl;
				}
				if (req.request_type == Request::RequestType::GROUP) {
					json jp;
					jp["flag"] = req.flag;
					jp["sub_type"] = req.GR_SubType;
					jp["approve"] = true;
					try {
						NetConnection::HttpPost("http://127.0.0.1:5700/set_group_add_request", jp);
					}
					catch (osucat::NetWork_Exception& ex) {
						cout << ex.Show() << endl;
					}
					char msg[4096];
					sprintf_s(msg,
						u8"[%s] [osucat][↑]: 已通过来自好友 %lld 的群 %lld 邀请请求。",
						utils::unixTime2Str(time(NULL)).c_str(),
						req.user_id,
						req.group_id);
					cout << msg << endl;
				}
			}
			static PictureInfo getImage(string file_name) {
				json jp;
				jp["file"] = file_name;
				PictureInfo rtn;
				rtn.size = 0;
				rtn.filename = "";
				rtn.url = "";
				rtn.format = "";
				try {
					string data = NetConnection::HttpPost("http://127.0.0.1:5700/get_image", jp);
					cout << data << endl;
					json j = json::parse(data)["data"];
					rtn.size = j["size"].get<int32_t>();
					rtn.filename = j["filename"].get<string>();
					rtn.url = j["url"].get<string>();
					rtn.format = rtn.filename.substr(rtn.filename.length() - 4);
				}
				catch (osucat::NetWork_Exception& ex) {
					cout << ex.Show() << endl;
				}
				char msg[4096];
				sprintf_s(msg,
					u8"[%s] [osucat][↓]: 接收图片 %s | 图片格式：%s | 大小: %ld | URL: %s",
					utils::unixTime2Str(time(NULL)).c_str(),
					rtn.filename.c_str(),
					rtn.format,
					rtn.size,
					rtn.url.c_str());
				cout << msg << endl;
				return rtn;
			}
	};
}

#endif
