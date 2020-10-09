﻿#pragma once
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
			if (forbiddenWordsLibrary(cmd) == true) {
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
			if (forbiddenWordsLibrary(cmd) == true) {
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
				if (forbiddenWordsLibrary(cmd) == true) {
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
			if (forbiddenWordsLibrary(cmd) == true) {
				*params = u8"不想理你...";
				return;
			}
			if (cmd.length() > 199) {
				*params = u8"太长了！";
				return;
			}
			utils::trim(cmd);
			json jp;
			jp["text"] = cmd;
			try {
				string tmp1, tmp2 = NetConnection::HttpsPost("https://lab.magiconch.com/api/nbnhhsh/guess", jp).substr(1);
				tmp2 = tmp2.substr(0, tmp2.length() - 1);
				try {
					json j = json::parse(tmp2)["trans"];
					for (int ii = 1, i = 0; i < j.size(); ++i) {
						tmp2 = "\n[" + to_string(ii) + "] " + j[i].get<string>();
						if (!forbiddenWordsLibrary(j[i].get<string>())) {
							tmp1 += tmp2;
							++ii;
						}
						if (ii > 10) {
							break;
						}
					}
					if (tmp1 != "") {
						char trntmp[8192];
						//sprintf printf一类的 必须跟c_str不然乱码
						sprintf_s(trntmp,
							u8"\"%s\"的返回结果如下：%s",
							cmd.c_str(),
							tmp1.c_str());
						*params = trntmp;
					}
					else {
						*params = u8"该词尚未收录";
					}
				}
				catch (json::exception) {
					*params = u8"参数错误";
					return;
				}
			}
			catch (osucat::NetWork_Exception) {
				*params = u8"访问api时超时...请稍后再试...";
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
				if (forbiddenWordsLibrary(cmd) == true) {
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
				db.writeBottle(driftingBottleDBEvent::WRITEIN, 0, timetmp, tar.user_id, senderinfo.nickname, cmd);
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
						db.getBottleID(tar.user_id, cmd, timetmp),
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
						db.getBottleID(tar.user_id, cmd, timetmp),
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
					dfb.msg = j[tempi]["message"].get<std::string>();
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
						u8"发于 %s\n"
						u8"内容是....\n%s",
						dfb.nickname.c_str(), dfb.sender, utils::unixTime2StrChinese(dfb.sendTime).c_str(), dfb.msg.c_str());
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
		static bool forbiddenWordsLibrary(string str) {
			string traditionalChineseLibrary[1191] = { u8"愛液",
								  u8"按摩棒",
								  u8"拔出來",
								  u8"爆草",
								  u8"包二奶",
								  u8"暴幹",
								  u8"暴姦",
								  u8"暴乳",
								  u8"爆乳",
								  u8"暴淫",
								  u8"被操",
								  u8"被插",
								  u8"被幹",
								  u8"逼姦",
								  u8"倉井空",
								  u8"插暴",
								  u8"操逼",
								  u8"操黑",
								  u8"操爛",
								  u8"肏你",
								  u8"肏死",
								  u8"操死",
								  u8"操我",
								  u8"廁奴",
								  u8"插比",
								  u8"插b",
								  u8"插逼",
								  u8"插進",
								  u8"插你",
								  u8"插我",
								  u8"插陰",
								  u8"潮吹",
								  u8"潮噴",
								  u8"成人電影",
								  u8"成人論壇",
								  u8"成人色情",
								  u8"成人網站",
								  u8"成人文學",
								  u8"成人小說",
								  u8"艷情小說",
								  u8"成人遊戲",
								  u8"吃精",
								  u8"抽插",
								  u8"春藥",
								  u8"大波",
								  u8"大力抽送",
								  u8"大乳",
								  u8"蕩婦",
								  u8"蕩女",
								  u8"盜撮",
								  u8"發浪",
								  u8"放尿",
								  u8"肥逼",
								  u8"粉穴",
								  u8"風月大陸",
								  u8"幹死你",
								  u8"幹穴",
								  u8"肛交",
								  u8"肛門",
								  u8"龜頭",
								  u8"裹本",
								  u8"國產av",
								  u8"好嫩",
								  u8"豪乳",
								  u8"黑逼",
								  u8"後庭",
								  u8"後穴",
								  u8"虎騎",
								  u8"換妻俱樂部",
								  u8"黃片",
								  u8"幾吧",
								  u8"雞吧",
								  u8"雞巴",
								  u8"雞姦",
								  u8"妓女",
								  u8"姦情",
								  u8"叫床",
								  u8"腳交",
								  u8"精液",
								  u8"就去日",
								  u8"巨屌",
								  u8"菊花洞",
								  u8"菊門",
								  u8"巨奶",
								  u8"巨乳",
								  u8"菊穴",
								  u8"開苞",
								  u8"口爆",
								  u8"口活",
								  u8"口交",
								  u8"口射",
								  u8"口淫",
								  u8"褲襪",
								  u8"狂操",
								  u8"狂插",
								  u8"浪逼",
								  u8"浪婦",
								  u8"浪叫",
								  u8"浪女",
								  u8"狼友",
								  u8"聊性",
								  u8"凌辱",
								  u8"漏乳",
								  u8"露b",
								  u8"亂交",
								  u8"亂倫",
								  u8"輪暴",
								  u8"輪操",
								  u8"輪姦",
								  u8"裸陪",
								  u8"買春",
								  u8"美逼",
								  u8"美少婦",
								  u8"美乳",
								  u8"美腿",
								  u8"美穴",
								  u8"美幼",
								  u8"秘唇",
								  u8"迷姦",
								  u8"密穴",
								  u8"蜜穴",
								  u8"蜜液",
								  u8"摸奶",
								  u8"摸胸",
								  u8"母姦",
								  u8"奈美",
								  u8"奶子",
								  u8"男奴",
								  u8"內射",
								  u8"嫩逼",
								  u8"嫩女",
								  u8"嫩穴",
								  u8"捏弄",
								  u8"女優",
								  u8"炮友",
								  u8"砲友",
								  u8"噴精",
								  u8"屁眼",
								  u8"前凸後翹",
								  u8"強jian",
								  u8"強暴",
								  u8"強姦處女",
								  u8"情趣用品",
								  u8"情色",
								  u8"拳交",
								  u8"全裸",
								  u8"群交",
								  u8"人妻",
								  u8"人獸",
								  u8"日逼",
								  u8"日爛",
								  u8"肉棒",
								  u8"肉逼",
								  u8"肉唇",
								  u8"肉洞",
								  u8"肉縫",
								  u8"肉棍",
								  u8"肉莖",
								  u8"肉具",
								  u8"揉乳",
								  u8"肉穴",
								  u8"肉慾",
								  u8"乳爆",
								  u8"乳房",
								  u8"乳溝",
								  u8"乳交",
								  u8"乳頭",
								  u8"騷逼",
								  u8"騷比",
								  u8"騷女",
								  u8"騷水",
								  u8"騷穴",
								  u8"色逼",
								  u8"色界",
								  u8"色貓",
								  u8"色盟",
								  u8"色情網站",
								  u8"色區",
								  u8"色色",
								  u8"色誘",
								  u8"色欲",
								  u8"色b",
								  u8"少年阿賓",
								  u8"射爽",
								  u8"射顏",
								  u8"食精",
								  u8"釋欲",
								  u8"獸姦",
								  u8"獸交",
								  u8"手淫",
								  u8"獸慾",
								  u8"熟婦",
								  u8"熟母",
								  u8"熟女",
								  u8"爽片",
								  u8"雙臀",
								  u8"死逼",
								  u8"絲襪",
								  u8"絲誘",
								  u8"松島楓",
								  u8"酥癢",
								  u8"湯加麗",
								  u8"套弄",
								  u8"體姦",
								  u8"體位",
								  u8"舔腳",
								  u8"舔陰",
								  u8"調教",
								  u8"偷歡",
								  u8"推油",
								  u8"脫內褲",
								  u8"文做",
								  u8"舞女",
								  u8"無修正",
								  u8"吸精",
								  u8"夏川純",
								  u8"相姦",
								  u8"小逼",
								  u8"校雞",
								  u8"小穴",
								  u8"小xue",
								  u8"性感妖嬈",
								  u8"性感誘惑",
								  u8"性虎",
								  u8"性飢渴",
								  u8"性技巧",
								  u8"性交",
								  u8"性奴",
								  u8"性虐",
								  u8"性息",
								  u8"性慾",
								  u8"胸推",
								  u8"穴口",
								  u8"穴圖",
								  u8"亞情",
								  u8"顏射",
								  u8"陽具",
								  u8"楊思敏",
								  u8"要射了",
								  u8"夜勤病棟",
								  u8"一本道",
								  u8"一夜歡",
								  u8"一夜情",
								  u8"一ye情",
								  u8"陰部",
								  u8"淫蟲",
								  u8"陰唇",
								  u8"淫蕩",
								  u8"陰道",
								  u8"淫電影",
								  u8"陰阜",
								  u8"淫婦",
								  u8"淫河",
								  u8"陰核",
								  u8"陰戶",
								  u8"淫賤",
								  u8"淫叫",
								  u8"淫教師",
								  u8"陰莖",
								  u8"陰精",
								  u8"淫浪",
								  u8"淫媚",
								  u8"淫糜",
								  u8"淫魔",
								  u8"淫母",
								  u8"婬女",
								  u8"淫虐",
								  u8"淫妻",
								  u8"淫情",
								  u8"淫色",
								  u8"淫聲浪語",
								  u8"淫獸學園",
								  u8"淫書",
								  u8"淫術煉金士",
								  u8"淫水",
								  u8"淫娃",
								  u8"淫威",
								  u8"淫褻",
								  u8"淫樣",
								  u8"淫液",
								  u8"淫照",
								  u8"陰b",
								  u8"應召",
								  u8"幼交",
								  u8"慾火",
								  u8"欲女",
								  u8"玉乳",
								  u8"玉穴",
								  u8"援交",
								  u8"原味內衣",
								  u8"援助交際",
								  u8"招雞",
								  u8"招妓",
								  u8"抓胸",
								  u8"自慰",
								  u8"作愛",
								  u8"a片",
								  u8"fuck",
								  u8"gay片",
								  u8"g點",
								  u8"h動畫",
								  u8"h動漫",
								  u8"失身粉",
								  u8"淫蕩自慰器",
								  u8"習近平",
								  u8"平近習",
								  u8"xjp",
								  u8"習太子",
								  u8"習明澤",
								  u8"老習",
								  u8"溫家寶",
								  u8"溫加寶",
								  u8"溫x",
								  u8"溫jia寶",
								  u8"溫寶寶",
								  u8"溫加飽",
								  u8"溫加保",
								  u8"張培莉",
								  u8"溫雲松",
								  u8"溫如春",
								  u8"溫jb",
								  u8"胡溫",
								  u8"胡x",
								  u8"胡jt",
								  u8"胡boss",
								  u8"胡總",
								  u8"胡王八",
								  u8"hujintao",
								  u8"胡jintao",
								  u8"胡j濤",
								  u8"胡驚濤",
								  u8"胡景濤",
								  u8"胡緊掏",
								  u8"湖緊掏",
								  u8"胡緊套",
								  u8"錦濤",
								  u8"hjt",
								  u8"胡派",
								  u8"胡主席",
								  u8"劉永清",
								  u8"胡海峰",
								  u8"胡海清",
								  u8"江澤民",
								  u8"民澤江",
								  u8"江胡",
								  u8"江哥",
								  u8"江主席",
								  u8"江書記",
								  u8"江浙閩",
								  u8"江沢民",
								  u8"江浙民",
								  u8"擇民",
								  u8"則民",
								  u8"茳澤民",
								  u8"zemin",
								  u8"ze民",
								  u8"老江",
								  u8"老j",
								  u8"江core",
								  u8"江x",
								  u8"江派",
								  u8"江zm",
								  u8"jzm",
								  u8"江戲子",
								  u8"江蛤蟆",
								  u8"江某某",
								  u8"江賊",
								  u8"江豬",
								  u8"江氏集團",
								  u8"江綿恆",
								  u8"江綿康",
								  u8"王冶坪",
								  u8"江澤慧",
								  u8"鄧小平",
								  u8"平小鄧",
								  u8"xiao平",
								  u8"鄧xp",
								  u8"鄧曉平",
								  u8"鄧樸方",
								  u8"鄧榕",
								  u8"鄧質方",
								  u8"毛澤東",
								  u8"貓澤東",
								  u8"貓則東",
								  u8"貓賊洞",
								  u8"毛zd",
								  u8"毛zx",
								  u8"z東",
								  u8"ze東",
								  u8"澤d",
								  u8"zedong",
								  u8"毛太祖",
								  u8"毛相",
								  u8"主席畫像",
								  u8"改革歷程",
								  u8"朱鎔基",
								  u8"朱容基",
								  u8"朱鎔雞",
								  u8"朱容雞",
								  u8"朱雲來",
								  u8"李鵬",
								  u8"李peng",
								  u8"裡鵬",
								  u8"李月月鳥",
								  u8"李小鵬",
								  u8"李小琳",
								  u8"華主席",
								  u8"華國",
								  u8"國鋒",
								  u8"國峰",
								  u8"鋒同志",
								  u8"白春禮",
								  u8"薄熙來",
								  u8"薄一波",
								  u8"蔡赴朝",
								  u8"蔡武",
								  u8"曹剛川",
								  u8"常萬全",
								  u8"陳炳德",
								  u8"陳德銘",
								  u8"陳建國",
								  u8"陳良宇",
								  u8"陳紹基",
								  u8"陳同海",
								  u8"陳至立",
								  u8"戴秉國",
								  u8"丁一平",
								  u8"董建華",
								  u8"杜德印",
								  u8"杜世成",
								  u8"傅銳",
								  u8"郭伯雄",
								  u8"郭金龍",
								  u8"賀國強",
								  u8"胡春華",
								  u8"耀邦",
								  u8"華建敏",
								  u8"黃華華",
								  u8"黃麗滿",
								  u8"黃興國",
								  u8"回良玉",
								  u8"賈慶林",
								  u8"賈廷安",
								  u8"靖志遠",
								  u8"李長春",
								  u8"李春城",
								  u8"李建國",
								  u8"李克強",
								  u8"李嵐清",
								  u8"李沛瑤",
								  u8"李榮融",
								  u8"李瑞環",
								  u8"李鐵映",
								  u8"李先念",
								  u8"李學舉",
								  u8"李源潮",
								  u8"栗智",
								  u8"梁光烈",
								  u8"廖錫龍",
								  u8"林樹森",
								  u8"林炎志",
								  u8"林左鳴",
								  u8"令計劃",
								  u8"柳斌杰",
								  u8"劉奇葆",
								  u8"劉少奇",
								  u8"劉延東",
								  u8"劉云山",
								  u8"劉志軍",
								  u8"龍新民",
								  u8"路甬祥",
								  u8"羅箭",
								  u8"呂祖善",
								  u8"馬飚",
								  u8"馬愷",
								  u8"孟建柱",
								  u8"歐廣源",
								  u8"強衛",
								  u8"沈躍躍",
								  u8"宋平順",
								  u8"粟戎生",
								  u8"蘇樹林",
								  u8"孫家正",
								  u8"鐵凝",
								  u8"屠光紹",
								  u8"王東明",
								  u8"汪東興",
								  u8"王鴻舉",
								  u8"王滬寧",
								  u8"王樂泉",
								  u8"王洛林",
								  u8"王岐山",
								  u8"王勝俊",
								  u8"王太華",
								  u8"王學軍",
								  u8"王兆國",
								  u8"王振華",
								  u8"吳邦國",
								  u8"吳定富",
								  u8"吳官正",
								  u8"無官正",
								  u8"吳勝利",
								  u8"吳儀",
								  u8"奚國華",
								  u8"習仲勳",
								  u8"徐才厚",
								  u8"許其亮",
								  u8"徐紹史",
								  u8"楊潔篪",
								  u8"葉劍英",
								  u8"由喜貴",
								  u8"於幼軍",
								  u8"俞正聲",
								  u8"袁純清",
								  u8"曾培炎",
								  u8"曾慶紅",
								  u8"曾憲梓",
								  u8"曾蔭權",
								  u8"張德江",
								  u8"張定發",
								  u8"張高麗",
								  u8"張立昌",
								  u8"張榮坤",
								  u8"張志國",
								  u8"趙洪祝",
								  u8"紫陽",
								  u8"週生賢",
								  u8"周永康",
								  u8"朱海侖",
								  u8"中南海",
								  u8"大陸當局",
								  u8"中國當局",
								  u8"北京當局",
								  u8"共產黨",
								  u8"黨產共",
								  u8"共貪黨",
								  u8"阿共",
								  u8"產黨共",
								  u8"公產黨",
								  u8"工產黨",
								  u8"共c黨",
								  u8"共x黨",
								  u8"共鏟",
								  u8"供產",
								  u8"共慘",
								  u8"供鏟黨",
								  u8"供鏟讜",
								  u8"供鏟襠",
								  u8"共殘黨",
								  u8"共殘主義",
								  u8"共產主義的幽靈",
								  u8"拱鏟",
								  u8"老共",
								  u8"中共",
								  u8"中珙",
								  u8"中gong",
								  u8"gc黨",
								  u8"貢擋",
								  u8"gong黨",
								  u8"g產",
								  u8"狗產蛋",
								  u8"共殘襠",
								  u8"惡黨",
								  u8"邪黨",
								  u8"共產專制",
								  u8"共產王朝",
								  u8"襠中央",
								  u8"土共",
								  u8"土g",
								  u8"共狗",
								  u8"g匪",
								  u8"共匪",
								  u8"仇共",
								  u8"政府",
								  u8"症腐",
								  u8"政腐",
								  u8"政付",
								  u8"正府",
								  u8"政俯",
								  u8"政f",
								  u8"zhengfu",
								  u8"政zhi",
								  u8"擋中央",
								  u8"檔中央",
								  u8"中央領導",
								  u8"中國zf",
								  u8"中央zf",
								  u8"國wu院",
								  u8"中華帝國",
								  u8"gong和",
								  u8"大陸官方",
								  u8"北京政權",
								  u8"江澤民",
								  u8"胡錦濤",
								  u8"溫家寶",
								  u8"習近平",
								  u8"習仲勳",
								  u8"賀國強",
								  u8"賀子珍",
								  u8"周永康",
								  u8"李長春",
								  u8"李德生",
								  u8"王岐山",
								  u8"姚依林",
								  u8"回良玉",
								  u8"李源潮",
								  u8"李乾成",
								  u8"戴秉國",
								  u8"黃鎮",
								  u8"劉延東",
								  u8"劉瑞龍",
								  u8"俞正聲",
								  u8"黃敬",
								  u8"薄熙",
								  u8"薄一波",
								  u8"周小川",
								  u8"周建南",
								  u8"溫雲松",
								  u8"徐明",
								  u8"江澤慧",
								  u8"江綿恆",
								  u8"江綿康",
								  u8"李小鵬",
								  u8"李鵬",
								  u8"李小琳",
								  u8"朱雲來",
								  u8"朱容基",
								  u8"法輪功",
								  u8"李洪志",
								  u8"新疆騷亂",
								  u8"出售雷管",
								  u8"炸藥",
								  u8"炸藥",
								  u8"出售炸藥",
								  u8"出售炸藥 電話",
								  u8"製作火藥配方",
								  u8"炸藥出售 ",
								  u8"出售雷管炸藥",
								  u8"火藥配方",
								  u8"在家裡做原子彈",
								  u8"硝酸甘油炸彈製作",
								  u8"TNT 炸彈的製作",
								  u8"硝銨炸藥配方",
								  u8"硝酸甘油製作",
								  u8"炸彈配方",
								  u8"簡易炸藥",
								  u8"火藥製作簡易炸彈",
								  u8"炸彈製作",
								  u8"硝酸甘油",
								  u8"硝酸甘油炸彈製作方法",
								  u8"手把手教你做炸彈",
								  u8"恐怖分子傻瓜手冊",
								  u8"氫彈手工製作方法",
								  u8"起爆器",
								  u8"自製燃燒彈",
								  u8"燃燒彈",
								  u8"彈藥製造方法",
								  u8"液體炸藥配方",
								  u8"炸藥配方與製作",
								  u8"自製炸藥配方",
								  u8"tnt炸藥成分",
								  u8"炸藥的製作",
								  u8"tnt炸藥配方",
								  u8"各種炸藥配方大全",
								  u8"炸藥的成分",
								  u8"燃燒彈的製作方 ",
								  u8"黑火藥的配方",
								  u8"黑火藥的製作",
								  u8"黑火藥比例",
								  u8"燃燒彈 製作",
								  u8"黑火藥配方比例",
								  u8"黑火藥的成分",
								  u8"原子彈配方",
								  u8"原子彈DIY教程",
								  u8"C4炸藥",
								  u8"雷管出售",
								  u8"製作硝化甘油",
								  u8"DIY核彈",
								  u8"核彈製作方法",
								  u8"原子彈製作手冊",
								  u8"簡易原子彈教程",
								  u8"黑火藥配方",
								  u8"自製炸藥方法",
								  u8"教你怎麼用土辦法做武器",
								  u8"莫洛托夫雞尾酒的配方",
								  u8"zz炸彈的製作方法",
								  u8"C4炸藥的成分",
								  u8"炸藥製造生產工藝",
								  u8"原子彈製造完全手冊",
								  u8"「紅燒兔子」大餐",
								  u8"汽車炸彈製作",
								  u8"人體炸彈製作流程",
								  u8"炸彈製作從入門",
								  u8"雞尾酒炸彈製作",
								  u8"集束炸彈製作",
								  u8"硝酸銨的簡便制取方法",
								  u8"莫洛托夫燃燒彈",
								  u8"塑膠炸彈",
								  u8"用火藥混合塑料",
								  u8"高性能爆炸物質混合",
								  u8"原子彈 製作方法",
								  u8"手教你製作原子彈",
								  u8"輕型巡航導彈的製作",
								  u8"黑索金的製造過程",
								  u8"Zha藥製造進程",
								  u8"購買自爆材料",
								  u8"可塑炸彈製作方法",
								  u8"C4塑膠炸藥",
								  u8"水浴法制TNT",
								  u8"簡易核彈頭的製造",
								  u8"高爆炸藥的基本配方",
								  u8"如何制硝化甘油",
								  u8"燃燒劑的簡易製備",
								  u8"簡易大威力炸彈",
								  u8"炸彈製作入門",
								  u8"硝酸甘油炸藥製作",
								  u8"基本配方是硝酸甘",
								  u8"燃燒劑的簡易製作",
								  u8"製作性能優良的燃燒彈",
								  u8"酸甘油炸藥",
								  u8"簡易導彈製作",
								  u8"核彈頭的製造",
								  u8"可塑炸彈製作",
								  u8"炸藥硝酸甘油製法",
								  u8"原子彈製作方法",
								  u8"火藥的配方",
								  u8"手教你做原子彈",
								  u8"怎麼用土辦法做武器",
								  u8"手把手教你製作原子彈",
								  u8"燃燒彈的製作方法",
								  u8"怎麼自製燃燒彈",
								  u8"TNT炸彈製作",
								  u8"高爆炸藥基本配方",
								  u8"爆破和殺傷燃燒彈",
								  u8"炸彈製作從入門到精",
								  u8"寫的炸彈製作教程",
								  u8"原子彈製作 方法",
								  u8"教你製作原子彈",
								  u8"下硝化甘油的製作方法",
								  u8"DIY原子彈",
								  u8"原子彈製作",
								  u8"炸藥及其製造方法",
								  u8"ATOM彈製造",
								  u8"三分鐘教你原子彈",
								  u8"火藥原理",
								  u8"簡易炸彈",
								  u8"簡易燃燒彈",
								  u8"土炸藥成份",
								  u8"你怎麼用土辦法做武器",
								  u8"燃燒彈製作",
								  u8"燃燒瓶的製作方法",
								  u8"炸彈製作手冊 硫酸",
								  u8"原子彈製作方法DIY",
								  u8"汽油三分之一白糖混合",
								  u8"炸藥的製作配方",
								  u8"燃燒彈的製作方",
								  u8"炸藥的配方",
								  u8"如何自製燃燒彈",
								  u8"標準炸彈教程",
								  u8"黑火藥和硝酸鉀農藥混合",
								  u8"恐怖分子自己製造炸彈",
								  u8"手柄製作炸彈引爆器",
								  u8"製作炸彈首先要",
								  u8"燃燒瓶製作",
								  u8"出售炸藥QQ",
								  u8"出售氣槍",
								  u8"出售氣槍 QQ",
								  u8"出售氣槍 電話",
								  u8"出售手槍",
								  u8"出售手槍 QQ",
								  u8"出售手槍 電話",
								  u8"出售獵槍",
								  u8"出售獵槍 QQ",
								  u8"出售獵槍 電話",
								  u8"出售汽槍",
								  u8"出售步槍",
								  u8"出售狙擊槍",
								  u8"出售手狗",
								  u8"出售氣狗",
								  u8"出售獵狗",
								  u8"【手槍出售】聯繫電話",
								  u8"64式手槍出售",
								  u8"92式手槍出售",
								  u8"出售:高仿真槍",
								  u8"出售54式手槍",
								  u8"出售M1911",
								  u8"出售各類獵狗",
								  u8"出售國產軍用54手槍",
								  u8"出售火槍 –遊戲",
								  u8"出售進口金屬槍模",
								  u8"出售仿真槍",
								  u8"出售麻醉槍免收任何定金",
								  u8"出售手槍=貨到付款",
								  u8"出售外國槍模",
								  u8"出售袖珍手槍",
								  u8"娥眉氣槍出售",
								  u8"防身手槍QQ",
								  u8"防身武器手槍",
								  u8"仿真槍QQ",
								  u8"鋼珠槍小口徑步槍",
								  u8"高壓狗QQ",
								  u8"高壓氣槍,氣槍子彈",
								  u8"高壓氣槍|電狗專賣",
								  u8"高壓氣槍|氣槍貨到付款",
								  u8"高壓氣槍專賣店",
								  u8"各類軍用槍",
								  u8"各類軍用槍支",
								  u8"各式氣槍出售",
								  u8"工字牌氣槍出售專賣",
								  u8"氣槍",
								  u8"工字汽狗麻醉槍",
								  u8"供應軍用手槍",
								  u8"供應汽槍",
								  u8"購買槍支QQ",
								  u8"國產汽狗專賣|氣槍貨",
								  u8"集團出售手槍",
								  u8"集團出售手槍氣槍",
								  u8"進口氣槍,氣槍子彈",
								  u8"軍用54出售",
								  u8"軍用77出售",
								  u8"軍用手槍鋼珠槍",
								  u8"裡有汽槍出售",
								  u8"獵槍出售/槍",
								  u8"獵槍仿真槍QQ",
								  u8"獵槍麻醉槍鋼珠槍",
								  u8"獵槍哪裡有賣",
								  u8"獵槍手槍買賣",
								  u8"獵槍銷售手槍銷售氣槍",
								  u8"獵槍專賣店",
								  u8"獵槍子彈底火專賣",
								  u8"麻醉槍出售可驗貨",
								  u8"麻醉槍獵槍軍用",
								  u8"買賣54狗",
								  u8"買賣64狗",
								  u8"買賣警察槍支",
								  u8"買賣軍狗",
								  u8"買賣軍用槍支",
								  u8"賣64手槍",
								  u8"賣92手槍",
								  u8"賣鋼珠槍QQ",
								  u8"哪裡能買到自製手槍 ",
								  u8"哪裡有鋼珠槍",
								  u8"哪裡有連發獵槍買賣",
								  u8"哪裡有賣雙管獵",
								  u8"哪裡有氣槍賣",
								  u8"哪裡有槍出售",
								  u8"哪裡有真槍出售",
								  u8"哪裡有真槍賣買",
								  u8"哪能買到真槍",
								  u8"期出售手槍氣槍獵",
								  u8"氣狗 高壓氣槍",
								  u8"氣狗出售 進口氣狗",
								  u8"氣槍專賣網",
								  u8"氣槍/出售/專賣網",
								  u8"氣槍/獵槍/鋼珠槍",
								  u8"氣槍仿真槍出售",
								  u8"氣槍購買|氣槍組裝",
								  u8"氣槍貨到付款,汽狗出售",
								  u8"氣槍價格 仿真氣槍",
								  u8"氣槍價格，氣槍專賣店",
								  u8"氣槍看貨加QQ",
								  u8"氣槍聯繫QQ",
								  u8"氣槍獵槍麻醉槍",
								  u8"氣槍迷魂藥",
								  u8"汽狗高壓汽槍",
								  u8"鉛彈 上海工字氣槍",
								  u8"全金屬仿真槍專賣",
								  u8"熱武器QQ",
								  u8"三箭氣槍出售",
								  u8"手槍氣* 槍獵",
								  u8"手槍買賣自製手槍",
								  u8"售槍購買手槍",
								  u8"售手槍QQ",
								  u8"司馬仿真槍模型QQ",
								  u8"淘寶氣槍貨到付款",
								  u8"提供氣狗專賣店",
								  u8"提供專業及時的氣狗",
								  u8"禿鷹氣槍貨到付款",
								  u8"錫峰氣槍出售",
								  u8"仙鶴氣槍出售",
								  u8"銷售/專賣/買賣77式手槍",
								  u8"銷售電手狗",
								  u8"銷售氣手狗",
								  u8"銷售小口徑步槍",
								  u8"銷售運動步槍",
								  u8"小口徑禿鷹沙漠",
								  u8"有軍用槍支出售",
								  u8"長期出 售手槍",
								  u8"折疊狗QQ",
								  u8"轉讓 獵槍",
								  u8"子彈貨到付款",
								  u8"自製手槍哪裡買 ",
								  u8"自製手槍哪裡有賣",
								  u8"阻擊槍/汽槍/高壓氣槍",
								  u8"組裝手槍 ",
								  u8"最新銷售汽狗",
								  u8"出售警用手銬|仿真槍",
								  u8"獵槍|真槍",
								  u8"手槍|阻擊槍",
								  u8"弓弩專營",
								  u8"弓弩專賣網貨到付款",
								  u8"弓弩專賣網",
								  u8"弓弩專賣貨到付款",
								  u8"弓弩專賣店網",
								  u8"弓弩專賣店",
								  u8"弓弩專賣",
								  u8"弓弩製作",
								  u8"弓弩直銷",
								  u8"弓弩有限公司",
								  u8"弓弩營銷中心",
								  u8"弓弩一百專賣店",
								  u8"弓弩銷售",
								  u8"弓弩網 ",
								  u8"弓弩網",
								  u8"弓弩圖紙",
								  u8"弓弩特許經銷",
								  u8"弓弩狩獵網",
								  u8"自製手弩",
								  u8"追風弓弩麻醉箭專賣",
								  u8"專業弓弩網",
								  u8"中國戰神軍用弓弩",
								  u8"中國弩弓專賣",
								  u8"中國弓弩專賣網",
								  u8"中國弓弩直銷",
								  u8"中國弓弩網",
								  u8"中國弓弩狩獵網",
								  u8"中國弓駑網",
								  u8"製作簡易弓弩 ",
								  u8"鄭州弓弩專賣",
								  u8"趙氏弓弩專賣網",
								  u8"趙氏弓弩專賣店",
								  u8"趙氏弓弩專賣",
								  u8"趙氏弓弩銷售",
								  u8"小型弓弩專賣店",
								  u8"小獵人弓弩網",
								  u8"狩獵器材弓弩專賣",
								  u8"狩獵器材弓弩",
								  u8"狩獵弓弩專賣網",
								  u8"狩獵弓弩專賣",
								  u8"狩獵弓弩麻醉箭",
								  u8"手槍式折疊三用弩",
								  u8"三利達弓弩專賣網",
								  u8"三利達弓弩直營",
								  u8"三利達弓弩配件",
								  u8"三步倒藥箭批發",
								  u8"三步倒弩箭專賣",
								  u8"三步倒麻醉弩箭銷售",
								  u8"三步倒麻醉箭專賣",
								  u8"三步倒麻醉箭",
								  u8"三步倒捕狗藥",
								  u8"軍用弓弩專賣網",
								  u8"軍用弓弩專賣店",
								  u8"軍用弓弩批發",
								  u8"軍用弓弩公司",
								  u8"供應三利達弓弩麻醉箭",
								  u8"供應三步倒麻醉箭",
								  u8"供應秦氏弓弩",
								  u8"供應弩用麻醉箭",
								  u8"供應弩捕狗箭",
								  u8"供應麻醉箭三步倒",
								  u8"供應麻醉箭批發",
								  u8"供應麻醉箭",
								  u8"供應軍用弩折疊弩",
								  u8"供應軍用弓弩專賣",
								  u8"供應精品弓弩",
								  u8"供應弓弩麻醉箭",
								  u8"供應弓弩",
								  u8"供應鋼珠弓弩",
								  u8"弓弩商城專賣",
								  u8"弓弩商城",
								  u8"弓弩親兄弟貨到付款",
								  u8"弓弩批發",
								  u8"弓弩免定金貨到付款",
								  u8"弓弩麻醉箭",
								  u8"弓弩麻醉鏢",
								  u8"弓弩論壇 ",
								  u8"鋼珠弓弩專賣網",
								  u8"鋼珠弓弩專賣店",
								  u8"打狗弓弩三步倒",
								  u8"麻醉弓弩專賣店",
								  u8"出售軍刀",
								  u8"出售軍刺",
								  u8"出售彈簧刀",
								  u8"出售三棱刀",
								  u8"出售跳刀",
								  u8"軍刀網",
								  u8"南方軍刀網",
								  u8"戶外軍刀網",
								  u8"三棱軍刺專賣",
								  u8"出售開山刀軍刺",
								  u8"西點軍刀網",
								  u8"軍刀專 賣",
								  u8"戈博軍刀",
								  u8"阿蘭德龍戶外",
								  u8"出售軍品軍刀",
								  u8"勃朗寧軍刀",
								  u8"軍刀軍品網",
								  u8"阿蘭得龍野營刀具網",
								  u8"出售軍刺軍刀",
								  u8"警用刀具出售",
								  u8"折刀專賣網",
								  u8"陽江軍品軍刀網",
								  u8"野營刀專賣",
								  u8"砍刀精品折刀專賣",
								  u8"匕首蝴蝶甩刀專賣",
								  u8"軍刀專賣軍刺",
								  u8"軍刀專賣刀具批發",
								  u8"軍刀圖片砍刀",
								  u8"軍刀網軍刀專賣",
								  u8"軍刀價格軍用刀具",
								  u8"軍品軍刺網",
								  u8"軍刀軍刺甩棍",
								  u8"陽江刀具批發網",
								  u8"北方先鋒軍刀",
								  u8"正品軍刺出售",
								  u8"野營軍刀出售",
								  u8"開山刀砍刀出售",
								  u8"仿品軍刺出售",
								  u8"軍刀直刀專賣",
								  u8"手工獵刀專賣",
								  u8"自動跳刀專賣",
								  u8"軍刀電棍銷售",
								  u8"軍刀甩棍銷售",
								  u8"美國軍刀出售",
								  u8"極端武力折刀",
								  u8"防衛棍刀戶外刀具",
								  u8"阿蘭德龍野營刀",
								  u8"仿品軍刺網",
								  u8"野營砍刀戶外軍刀",
								  u8"手工獵刀戶外刀具",
								  u8"中國戶外刀具網",
								  u8"西點軍品軍刀網",
								  u8"野營開山刀軍刺",
								  u8"三利達弓弩軍刀",
								  u8"尼泊爾軍刀出售",
								  u8"防衛野營砍刀出售",
								  u8"防衛著名軍刀出售",
								  u8"防衛棍刀出售",
								  u8"防衛甩棍出售",
								  u8"防衛電棍出售",
								  u8"軍刺野營砍刀出售",
								  u8"著名精品折刀出售",
								  u8"戰術軍刀出售",
								  u8"刺刀專賣網",
								  u8"戶外軍刀出售",
								  u8"陽江刀具直銷網",
								  u8"冷鋼刀具直銷網",
								  u8"防衛刀具直銷網",
								  u8"極端武力直銷網",
								  u8"刀具直銷網",
								  u8"軍刀直銷網",
								  u8"直刀匕首直銷網",
								  u8"軍刀匕首直銷網",
								  u8"折刀砍刀軍品網",
								  u8"野營刀具軍品網",
								  u8"陽江刀具軍品網",
								  u8"冷鋼刀具軍品網",
								  u8"防衛刀具軍品網",
								  u8"極端武力軍品網",
								  u8"軍用刀具軍品網",
								  u8"軍刀直刀軍品網",
								  u8"折刀砍刀專賣",
								  u8"野營刀具專賣",
								  u8"陽江刀具專賣",
								  u8"冷鋼刀具專賣",
								  u8"防衛刀具專賣",
								  u8"出售美軍現役軍刀",
								  u8"兼職",
								  u8"招聘",
								  u8"網絡",
								  u8"QQ",
								  u8"招聘",
								  u8"有意者",
								  u8"到貨",
								  u8"本店",
								  u8"代購",
								  u8"扣扣",
								  u8"客服",
								  u8"微店",
								  u8"兼職",
								  u8"兼值",
								  u8"淘寶",
								  u8"小姐",
								  u8"妓女",
								  u8"包夜",
								  u8"3P",
								  u8"LY",
								  u8"JS",
								  u8"狼友",
								  u8"技師",
								  u8"推油",
								  u8"胸推",
								  u8"BT",
								  u8"毒龍",
								  u8"口爆",
								  u8"兼職",
								  u8"樓鳳",
								  u8"足交",
								  u8"口暴",
								  u8"口交",
								  u8"全套",
								  u8"SM",
								  u8"桑拿",
								  u8"吞精",
								  u8"咪咪",
								  u8"婊子",
								  u8"乳方",
								  u8"操逼",
								  u8"全職",
								  u8"性伴侶",
								  u8"網購",
								  u8"網絡工作",
								  u8"代理",
								  u8"專業代理",
								  u8"幫忙點一下",
								  u8"幫忙點下",
								  u8"請點擊進入",
								  u8"詳情請進入",
								  u8"私人偵探",
								  u8"私家偵探",
								  u8"針孔攝像",
								  u8"調查婚外情",
								  u8"信用卡提現",
								  u8"無抵押貸款",
								  u8"廣告代理",
								  u8"原音鈴聲",
								  u8"借腹生子",
								  u8"找個媽媽",
								  u8"找個爸爸",
								  u8"代孕媽媽",
								  u8"代生孩子",
								  u8"代開發票",
								  u8"騰訊客服電話",
								  u8"銷售熱線",
								  u8"免費訂購熱線",
								  u8"低價出售",
								  u8"款到發貨",
								  u8"回复可見",
								  u8"連鎖加盟",
								  u8"加盟連鎖",
								  u8"免費二級域名",
								  u8"免費使用",
								  u8"免費索取",
								  u8"蟻力神",
								  u8"嬰兒湯",
								  u8"售腎",
								  u8"刻章辦",
								  u8"買小車",
								  u8"套牌車",
								  u8"瑪雅網",
								  u8"電腦傳訊",
								  u8"視頻來源",
								  u8"下載速度",
								  u8"高清在線",
								  u8"全集在線",
								  u8"在線播放",
								  u8"txt下載",
								  u8"六位qq",
								  u8"6位qq",
								  u8"位的qq",
								  u8"個qb",
								  u8"送qb",
								  u8"用刀橫向切腹",
								  u8"完全自殺手冊",
								  u8"四海幫",
								  u8"足球投注",
								  u8"地下錢莊",
								  u8"中國復興黨",
								  u8"阿波羅網",
								  u8"曾道人",
								  u8"六合彩",
								  u8"改卷內幕",
								  u8"替考試",
								  u8"隱形耳機",
								  u8"出售答案",
								  u8"考中答案",
								  u8"答an",
								  u8"da案",
								  u8"資金周轉",
								  u8"救市",
								  u8"股市圈錢",
								  u8"崩盤",
								  u8"資金短缺",
								  u8"證監會",
								  u8"質押貸款",
								  u8"小額貸款",
								  u8"周小川",
								  u8"劉明康",
								  u8"尚福林",
								  u8"孔丹",
								  u8"分期u8" };
			string simplifiedChineseLibrary[1191] = { u8"爱液",
								  u8"按摩棒",
								  u8"拔出来",
								  u8"爆草",
								  u8"包二奶",
								  u8"暴干",
								  u8"暴奸",
								  u8"暴乳",
								  u8"爆乳",
								  u8"暴淫",
								  u8"被操",
								  u8"被插",
								  u8"被干",
								  u8"逼奸",
								  u8"仓井空",
								  u8"插暴",
								  u8"操逼",
								  u8"操黑",
								  u8"操烂",
								  u8"肏你",
								  u8"肏死",
								  u8"操死",
								  u8"操我",
								  u8"厕奴",
								  u8"插比",
								  u8"插b",
								  u8"插逼",
								  u8"插进",
								  u8"插你",
								  u8"插我",
								  u8"插阴",
								  u8"潮吹",
								  u8"潮喷",
								  u8"成人电影",
								  u8"成人论坛",
								  u8"成人色情",
								  u8"成人网站",
								  u8"成人文学",
								  u8"成人小说",
								  u8"艳情小说",
								  u8"成人游戏",
								  u8"吃精",
								  u8"抽插",
								  u8"春药",
								  u8"大波",
								  u8"大力抽送",
								  u8"大乳",
								  u8"荡妇",
								  u8"荡女",
								  u8"盗撮",
								  u8"发浪",
								  u8"放尿",
								  u8"肥逼",
								  u8"粉穴",
								  u8"风月大陆",
								  u8"干死你",
								  u8"干穴",
								  u8"肛交",
								  u8"肛门",
								  u8"龟头",
								  u8"裹本",
								  u8"国产av",
								  u8"好嫩",
								  u8"豪乳",
								  u8"黑逼",
								  u8"后庭",
								  u8"后穴",
								  u8"虎骑",
								  u8"换妻俱乐部",
								  u8"黄片",
								  u8"几吧",
								  u8"鸡吧",
								  u8"鸡巴",
								  u8"鸡奸",
								  u8"妓女",
								  u8"奸情",
								  u8"叫床",
								  u8"脚交",
								  u8"精液",
								  u8"就去日",
								  u8"巨屌",
								  u8"菊花洞",
								  u8"菊门",
								  u8"巨奶",
								  u8"巨乳",
								  u8"菊穴",
								  u8"开苞",
								  u8"口爆",
								  u8"口活",
								  u8"口交",
								  u8"口射",
								  u8"口淫",
								  u8"裤袜",
								  u8"狂操",
								  u8"狂插",
								  u8"浪逼",
								  u8"浪妇",
								  u8"浪叫",
								  u8"浪女",
								  u8"狼友",
								  u8"聊性",
								  u8"凌辱",
								  u8"漏乳",
								  u8"露b",
								  u8"乱交",
								  u8"乱伦",
								  u8"轮暴",
								  u8"轮操",
								  u8"轮奸",
								  u8"裸陪",
								  u8"买春",
								  u8"美逼",
								  u8"美少妇",
								  u8"美乳",
								  u8"美腿",
								  u8"美穴",
								  u8"美幼",
								  u8"秘唇",
								  u8"迷奸",
								  u8"密穴",
								  u8"蜜穴",
								  u8"蜜液",
								  u8"摸奶",
								  u8"摸胸",
								  u8"母奸",
								  u8"奈美",
								  u8"奶子",
								  u8"男奴",
								  u8"内射",
								  u8"嫩逼",
								  u8"嫩女",
								  u8"嫩穴",
								  u8"捏弄",
								  u8"女优",
								  u8"炮友",
								  u8"砲友",
								  u8"喷精",
								  u8"屁眼",
								  u8"前凸后翘",
								  u8"强jian",
								  u8"强暴",
								  u8"强奸处女",
								  u8"情趣用品",
								  u8"情色",
								  u8"拳交",
								  u8"全裸",
								  u8"群交",
								  u8"人妻",
								  u8"人兽",
								  u8"日逼",
								  u8"日烂",
								  u8"肉棒",
								  u8"肉逼",
								  u8"肉唇",
								  u8"肉洞",
								  u8"肉缝",
								  u8"肉棍",
								  u8"肉茎",
								  u8"肉具",
								  u8"揉乳",
								  u8"肉穴",
								  u8"肉欲",
								  u8"乳爆",
								  u8"乳房",
								  u8"乳沟",
								  u8"乳交",
								  u8"乳头",
								  u8"骚逼",
								  u8"骚比",
								  u8"骚女",
								  u8"骚水",
								  u8"骚穴",
								  u8"色逼",
								  u8"色界",
								  u8"色猫",
								  u8"色盟",
								  u8"色情网站",
								  u8"色区",
								  u8"色色",
								  u8"色诱",
								  u8"色欲",
								  u8"色b",
								  u8"少年阿宾",
								  u8"射爽",
								  u8"射颜",
								  u8"食精",
								  u8"释欲",
								  u8"兽奸",
								  u8"兽交",
								  u8"手淫",
								  u8"兽欲",
								  u8"熟妇",
								  u8"熟母",
								  u8"熟女",
								  u8"爽片",
								  u8"双臀",
								  u8"死逼",
								  u8"丝袜",
								  u8"丝诱",
								  u8"松岛枫",
								  u8"酥痒",
								  u8"汤加丽",
								  u8"套弄",
								  u8"体奸",
								  u8"体位",
								  u8"舔脚",
								  u8"舔阴",
								  u8"调教",
								  u8"偷欢",
								  u8"推油",
								  u8"脱内裤",
								  u8"文做",
								  u8"舞女",
								  u8"无修正",
								  u8"吸精",
								  u8"夏川纯",
								  u8"相奸",
								  u8"小逼",
								  u8"校鸡",
								  u8"小穴",
								  u8"小xue",
								  u8"性感妖娆",
								  u8"性感诱惑",
								  u8"性虎",
								  u8"性饥渴",
								  u8"性技巧",
								  u8"性交",
								  u8"性奴",
								  u8"性虐",
								  u8"性息",
								  u8"性欲",
								  u8"胸推",
								  u8"穴口",
								  u8"穴图",
								  u8"亚情",
								  u8"颜射",
								  u8"阳具",
								  u8"杨思敏",
								  u8"要射了",
								  u8"夜勤病栋",
								  u8"一本道",
								  u8"一夜欢",
								  u8"一夜情",
								  u8"一ye情",
								  u8"阴部",
								  u8"淫虫",
								  u8"阴唇",
								  u8"淫荡",
								  u8"阴道",
								  u8"淫电影",
								  u8"阴阜",
								  u8"淫妇",
								  u8"淫河",
								  u8"阴核",
								  u8"阴户",
								  u8"淫贱",
								  u8"淫叫",
								  u8"淫教师",
								  u8"阴茎",
								  u8"阴精",
								  u8"淫浪",
								  u8"淫媚",
								  u8"淫糜",
								  u8"淫魔",
								  u8"淫母",
								  u8"淫女",
								  u8"淫虐",
								  u8"淫妻",
								  u8"淫情",
								  u8"淫色",
								  u8"淫声浪语",
								  u8"淫兽学园",
								  u8"淫书",
								  u8"淫术炼金士",
								  u8"淫水",
								  u8"淫娃",
								  u8"淫威",
								  u8"淫亵",
								  u8"淫样",
								  u8"淫液",
								  u8"淫照",
								  u8"阴b",
								  u8"应召",
								  u8"幼交",
								  u8"欲火",
								  u8"欲女",
								  u8"玉乳",
								  u8"玉穴",
								  u8"援交",
								  u8"原味内衣",
								  u8"援助交际",
								  u8"招鸡",
								  u8"招妓",
								  u8"抓胸",
								  u8"自慰",
								  u8"作爱",
								  u8"a片",
								  u8"fuck",
								  u8"gay片",
								  u8"g点",
								  u8"h动画",
								  u8"h动漫",
								  u8"失身粉",
								  u8"淫荡自慰器",
								  u8"习近平",
								  u8"平近习",
								  u8"xjp",
								  u8"习太子",
								  u8"习明泽",
								  u8"老习",
								  u8"温家宝",
								  u8"温加宝",
								  u8"温x",
								  u8"温jia宝",
								  u8"温宝宝",
								  u8"温加饱",
								  u8"温加保",
								  u8"张培莉",
								  u8"温云松",
								  u8"温如春",
								  u8"温jb",
								  u8"胡温",
								  u8"胡x",
								  u8"胡jt",
								  u8"胡boss",
								  u8"胡总",
								  u8"胡王八",
								  u8"hujintao",
								  u8"胡jintao",
								  u8"胡j涛",
								  u8"胡惊涛",
								  u8"胡景涛",
								  u8"胡紧掏",
								  u8"湖紧掏",
								  u8"胡紧套",
								  u8"锦涛",
								  u8"hjt",
								  u8"胡派",
								  u8"胡主席",
								  u8"刘永清",
								  u8"胡海峰",
								  u8"胡海清",
								  u8"江泽民",
								  u8"民泽江",
								  u8"江胡",
								  u8"江哥",
								  u8"江主席",
								  u8"江书记",
								  u8"江浙闽",
								  u8"江沢民",
								  u8"江浙民",
								  u8"择民",
								  u8"则民",
								  u8"茳泽民",
								  u8"zemin",
								  u8"ze民",
								  u8"老江",
								  u8"老j",
								  u8"江core",
								  u8"江x",
								  u8"江派",
								  u8"江zm",
								  u8"jzm",
								  u8"江戏子",
								  u8"江蛤蟆",
								  u8"江某某",
								  u8"江贼",
								  u8"江猪",
								  u8"江氏集团",
								  u8"江绵恒",
								  u8"江绵康",
								  u8"王冶坪",
								  u8"江泽慧",
								  u8"邓小平",
								  u8"平小邓",
								  u8"xiao平",
								  u8"邓xp",
								  u8"邓晓平",
								  u8"邓朴方",
								  u8"邓榕",
								  u8"邓质方",
								  u8"毛泽东",
								  u8"猫泽东",
								  u8"猫则东",
								  u8"猫贼洞",
								  u8"毛zd",
								  u8"毛zx",
								  u8"z东",
								  u8"ze东",
								  u8"泽d",
								  u8"zedong",
								  u8"毛太祖",
								  u8"毛相",
								  u8"主席画像",
								  u8"改革历程",
								  u8"朱镕基",
								  u8"朱容基",
								  u8"朱镕鸡",
								  u8"朱容鸡",
								  u8"朱云来",
								  u8"李鹏",
								  u8"李peng",
								  u8"里鹏",
								  u8"李月月鸟",
								  u8"李小鹏",
								  u8"李小琳",
								  u8"华主席",
								  u8"华国",
								  u8"国锋",
								  u8"国峰",
								  u8"锋同志",
								  u8"白春礼",
								  u8"薄熙来",
								  u8"薄一波",
								  u8"蔡赴朝",
								  u8"蔡武",
								  u8"曹刚川",
								  u8"常万全",
								  u8"陈炳德",
								  u8"陈德铭",
								  u8"陈建国",
								  u8"陈良宇",
								  u8"陈绍基",
								  u8"陈同海",
								  u8"陈至立",
								  u8"戴秉国",
								  u8"丁一平",
								  u8"董建华",
								  u8"杜德印",
								  u8"杜世成",
								  u8"傅锐",
								  u8"郭伯雄",
								  u8"郭金龙",
								  u8"贺国强",
								  u8"胡春华",
								  u8"耀邦",
								  u8"华建敏",
								  u8"黄华华",
								  u8"黄丽满",
								  u8"黄兴国",
								  u8"回良玉",
								  u8"贾庆林",
								  u8"贾廷安",
								  u8"靖志远",
								  u8"李长春",
								  u8"李春城",
								  u8"李建国",
								  u8"李克强",
								  u8"李岚清",
								  u8"李沛瑶",
								  u8"李荣融",
								  u8"李瑞环",
								  u8"李铁映",
								  u8"李先念",
								  u8"李学举",
								  u8"李源潮",
								  u8"栗智",
								  u8"梁光烈",
								  u8"廖锡龙",
								  u8"林树森",
								  u8"林炎志",
								  u8"林左鸣",
								  u8"令计划",
								  u8"柳斌杰",
								  u8"刘奇葆",
								  u8"刘少奇",
								  u8"刘延东",
								  u8"刘云山",
								  u8"刘志军",
								  u8"龙新民",
								  u8"路甬祥",
								  u8"罗箭",
								  u8"吕祖善",
								  u8"马飚",
								  u8"马恺",
								  u8"孟建柱",
								  u8"欧广源",
								  u8"强卫",
								  u8"沈跃跃",
								  u8"宋平顺",
								  u8"粟戎生",
								  u8"苏树林",
								  u8"孙家正",
								  u8"铁凝",
								  u8"屠光绍",
								  u8"王东明",
								  u8"汪东兴",
								  u8"王鸿举",
								  u8"王沪宁",
								  u8"王乐泉",
								  u8"王洛林",
								  u8"王岐山",
								  u8"王胜俊",
								  u8"王太华",
								  u8"王学军",
								  u8"王兆国",
								  u8"王振华",
								  u8"吴邦国",
								  u8"吴定富",
								  u8"吴官正",
								  u8"无官正",
								  u8"吴胜利",
								  u8"吴仪",
								  u8"奚国华",
								  u8"习仲勋",
								  u8"徐才厚",
								  u8"许其亮",
								  u8"徐绍史",
								  u8"杨洁篪",
								  u8"叶剑英",
								  u8"由喜贵",
								  u8"于幼军",
								  u8"俞正声",
								  u8"袁纯清",
								  u8"曾培炎",
								  u8"曾庆红",
								  u8"曾宪梓",
								  u8"曾荫权",
								  u8"张德江",
								  u8"张定发",
								  u8"张高丽",
								  u8"张立昌",
								  u8"张荣坤",
								  u8"张志国",
								  u8"赵洪祝",
								  u8"紫阳",
								  u8"周生贤",
								  u8"周永康",
								  u8"朱海仑",
								  u8"中南海",
								  u8"大陆当局",
								  u8"中国当局",
								  u8"北京当局",
								  u8"共产党",
								  u8"党产共",
								  u8"共贪党",
								  u8"阿共",
								  u8"产党共",
								  u8"公产党",
								  u8"工产党",
								  u8"共c党",
								  u8"共x党",
								  u8"共铲",
								  u8"供产",
								  u8"共惨",
								  u8"供铲党",
								  u8"供铲谠",
								  u8"供铲裆",
								  u8"共残党",
								  u8"共残主义",
								  u8"共产主义的幽灵",
								  u8"拱铲",
								  u8"老共",
								  u8"中共",
								  u8"中珙",
								  u8"中gong",
								  u8"gc党",
								  u8"贡挡",
								  u8"gong党",
								  u8"g产",
								  u8"狗产蛋",
								  u8"共残裆",
								  u8"恶党",
								  u8"邪党",
								  u8"共产专制",
								  u8"共产王朝",
								  u8"裆中央",
								  u8"土共",
								  u8"土g",
								  u8"共狗",
								  u8"g匪",
								  u8"共匪",
								  u8"仇共",
								  u8"政府",
								  u8"症腐",
								  u8"政腐",
								  u8"政付",
								  u8"正府",
								  u8"政俯",
								  u8"政f",
								  u8"zhengfu",
								  u8"政zhi",
								  u8"挡中央",
								  u8"档中央",
								  u8"中央领导",
								  u8"中国zf",
								  u8"中央zf",
								  u8"国wu院",
								  u8"中华帝国",
								  u8"gong和",
								  u8"大陆官方",
								  u8"北京政权",
								  u8"江泽民",
								  u8"胡锦涛",
								  u8"温家宝",
								  u8"习近平",
								  u8"习仲勋",
								  u8"贺国强",
								  u8"贺子珍",
								  u8"周永康",
								  u8"李长春",
								  u8"李德生",
								  u8"王岐山",
								  u8"姚依林",
								  u8"回良玉",
								  u8"李源潮",
								  u8"李干成",
								  u8"戴秉国",
								  u8"黄镇",
								  u8"刘延东",
								  u8"刘瑞龙",
								  u8"俞正声",
								  u8"黄敬",
								  u8"薄熙",
								  u8"薄一波",
								  u8"周小川",
								  u8"周建南",
								  u8"温云松",
								  u8"徐明",
								  u8"江泽慧",
								  u8"江绵恒",
								  u8"江绵康",
								  u8"李小鹏",
								  u8"李鹏",
								  u8"李小琳",
								  u8"朱云来",
								  u8"朱容基",
								  u8"法轮功",
								  u8"李洪志",
								  u8"新疆骚乱",
								  u8"出售雷管",
								  u8"炸药",
								  u8"炸药",
								  u8"出售炸药",
								  u8"出售炸药 电话",
								  u8"制作火药配方",
								  u8"炸药出售 ",
								  u8"出售雷管炸药",
								  u8"火药配方",
								  u8"在家里做原子弹",
								  u8"硝酸甘油炸弹制作",
								  u8"TNT 炸弹的制作",
								  u8"硝铵炸药配方",
								  u8"硝酸甘油制作",
								  u8"炸弹配方",
								  u8"简易炸药",
								  u8"火药制作简易炸弹",
								  u8"炸弹制作",
								  u8"硝酸甘油",
								  u8"硝酸甘油炸弹制作方法",
								  u8"手把手教你做炸弹",
								  u8"恐怖分子傻瓜手册",
								  u8"氢弹手工制作方法",
								  u8"起爆器",
								  u8"自制燃烧弹",
								  u8"燃烧弹",
								  u8"弹药制造方法",
								  u8"液体炸药配方",
								  u8"炸药配方与制作",
								  u8"自制炸药配方",
								  u8"tnt炸药成分",
								  u8"炸药的制作",
								  u8"tnt炸药配方",
								  u8"各种炸药配方大全",
								  u8"炸药的成分",
								  u8"燃烧弹的制作方 ",
								  u8"黑火药的配方",
								  u8"黑火药的制作",
								  u8"黑火药比例",
								  u8"燃烧弹 制作",
								  u8"黑火药配方比例",
								  u8"黑火药的成分",
								  u8"原子弹配方",
								  u8"原子弹DIY教程",
								  u8"C4炸药",
								  u8"雷管出售",
								  u8"制作硝化甘油",
								  u8"DIY核弹",
								  u8"核弹制作方法",
								  u8"原子弹制作手册",
								  u8"简易原子弹教程",
								  u8"黑火药配方",
								  u8"自制炸药方法",
								  u8"教你怎么用土办法做武器",
								  u8"莫洛托夫鸡尾酒的配方",
								  u8"zz炸弹的制作方法",
								  u8"C4炸药的成分",
								  u8"炸药制造生产工艺",
								  u8"原子弹制造完全手册",
								  u8"「红烧兔子」大餐",
								  u8"汽车炸弹制作",
								  u8"人体炸弹制作流程",
								  u8"炸弹制作从入门",
								  u8"鸡尾酒炸弹制作",
								  u8"集束炸弹制作",
								  u8"硝酸铵的简便制取方法",
								  u8"莫洛托夫燃烧弹",
								  u8"塑胶炸弹",
								  u8"用火药混合塑料",
								  u8"高性能爆炸物质混合",
								  u8"原子弹 制作方法",
								  u8"手教你制作原子弹",
								  u8"轻型巡航导弹的制作",
								  u8"黑索金的制造过程",
								  u8"Zha药制造进程",
								  u8"购买自爆材料",
								  u8"可塑炸弹制作方法",
								  u8"C4塑胶炸药",
								  u8"水浴法制TNT",
								  u8"简易核弹头的制造",
								  u8"高爆炸药的基本配方",
								  u8"如何制硝化甘油",
								  u8"燃烧剂的简易制备",
								  u8"简易大威力炸弹",
								  u8"炸弹制作入门",
								  u8"硝酸甘油炸药制作",
								  u8"基本配方是硝酸甘",
								  u8"燃烧剂的简易制作",
								  u8"制作性能优良的燃烧弹",
								  u8"酸甘油炸药",
								  u8"简易导弹制作",
								  u8"核弹头的制造",
								  u8"可塑炸弹制作",
								  u8"炸药硝酸甘油制法",
								  u8"原子弹制作方法",
								  u8"火药的配方",
								  u8"手教你做原子弹",
								  u8"怎么用土办法做武器",
								  u8"手把手教你制作原子弹",
								  u8"燃烧弹的制作方法",
								  u8"怎么自制燃烧弹",
								  u8"TNT炸弹制作",
								  u8"高爆炸药基本配方",
								  u8"爆破和杀伤燃烧弹",
								  u8"炸弹制作从入门到精",
								  u8"写的炸弹制作教程",
								  u8"原子弹制作 方法",
								  u8"教你制作原子弹",
								  u8"下硝化甘油的制作方法",
								  u8"DIY原子弹",
								  u8"原子弹制作",
								  u8"炸药及其制造方法",
								  u8"ATOM弹制造",
								  u8"三分钟教你原子弹",
								  u8"火药原理",
								  u8"简易炸弹",
								  u8"简易燃烧弹",
								  u8"土炸药成份",
								  u8"你怎么用土办法做武器",
								  u8"燃烧弹制作",
								  u8"燃烧瓶的制作方法",
								  u8"炸弹制作手册 硫酸",
								  u8"原子弹制作方法DIY",
								  u8"汽油三分之一白糖混合",
								  u8"炸药的制作配方",
								  u8"燃烧弹的制作方",
								  u8"炸药的配方",
								  u8"如何自制燃烧弹",
								  u8"标准炸弹教程",
								  u8"黑火药和硝酸钾农药混合",
								  u8"恐怖分子自己制造炸弹",
								  u8"手柄制作炸弹引爆器",
								  u8"制作炸弹首先要",
								  u8"燃烧瓶制作",
								  u8"出售炸药QQ",
								  u8"出售气枪",
								  u8"出售气枪 QQ",
								  u8"出售气枪 电话",
								  u8"出售手枪",
								  u8"出售手枪 QQ",
								  u8"出售手枪 电话",
								  u8"出售猎枪",
								  u8"出售猎枪 QQ",
								  u8"出售猎枪 电话",
								  u8"出售汽枪",
								  u8"出售步枪",
								  u8"出售狙击枪",
								  u8"出售手狗",
								  u8"出售气狗",
								  u8"出售猎狗",
								  u8"【手枪出售】联系电话",
								  u8"64式手枪出售",
								  u8"92式手枪出售",
								  u8"出售:高仿真枪",
								  u8"出售54式手枪",
								  u8"出售M1911",
								  u8"出售各类猎狗",
								  u8"出售国产军用54手枪",
								  u8"出售火枪 –游戏",
								  u8"出售进口金属枪模",
								  u8"出售仿真枪",
								  u8"出售麻醉枪免收任何定金",
								  u8"出售手枪=货到付款",
								  u8"出售外国枪模",
								  u8"出售袖珍手枪",
								  u8"娥眉气枪出售",
								  u8"防身手枪QQ",
								  u8"防身武器手枪",
								  u8"仿真枪QQ",
								  u8"钢珠枪小口径步枪",
								  u8"高压狗QQ",
								  u8"高压气枪,气枪子弹",
								  u8"高压气枪|电狗专卖",
								  u8"高压气枪|气枪货到付款",
								  u8"高压气枪专卖店",
								  u8"各类军用枪",
								  u8"各类军用枪支",
								  u8"各式气枪出售",
								  u8"工字牌气枪出售专卖",
								  u8"气枪",
								  u8"工字汽狗麻醉枪",
								  u8"供应军用手枪",
								  u8"供应汽枪",
								  u8"购买枪支QQ",
								  u8"国产汽狗专卖|气枪货",
								  u8"集团出售手枪",
								  u8"集团出售手枪气枪",
								  u8"进口气枪,气枪子弹",
								  u8"军用54出售",
								  u8"军用77出售",
								  u8"军用手枪钢珠枪",
								  u8"里有汽枪出售",
								  u8"猎枪出售/枪",
								  u8"猎枪仿真枪QQ",
								  u8"猎枪麻醉枪钢珠枪",
								  u8"猎枪哪里有卖",
								  u8"猎枪手枪买卖",
								  u8"猎枪销售手枪销售气枪",
								  u8"猎枪专卖店",
								  u8"猎枪子弹底火专卖",
								  u8"麻醉枪出售可验货",
								  u8"麻醉枪猎枪军用",
								  u8"买卖54狗",
								  u8"买卖64狗",
								  u8"买卖警察枪支",
								  u8"买卖军狗",
								  u8"买卖军用枪支",
								  u8"卖64手枪",
								  u8"卖92手枪",
								  u8"卖钢珠枪QQ",
								  u8"哪里能买到自制手枪  ",
								  u8"哪里有钢珠枪",
								  u8"哪里有连发猎枪买卖",
								  u8"哪里有卖双管猎",
								  u8"哪里有气枪卖",
								  u8"哪里有枪出售",
								  u8"哪里有真枪出售",
								  u8"哪里有真枪卖买",
								  u8"哪能买到真枪",
								  u8"期出售手枪气枪猎",
								  u8"气狗 高压气枪",
								  u8"气狗出售 进口气狗",
								  u8"气枪专卖网",
								  u8"气枪/出售/专卖网",
								  u8"气枪/猎枪/钢珠枪",
								  u8"气枪仿真枪出售",
								  u8"气枪购买|气枪组装",
								  u8"气枪货到付款,汽狗出售",
								  u8"气枪价格 仿真气枪",
								  u8"气枪价格，气枪专卖店",
								  u8"气枪看货加QQ",
								  u8"气枪联系QQ",
								  u8"气枪猎枪麻醉枪",
								  u8"气枪迷魂药",
								  u8"汽狗高压汽枪",
								  u8"铅弹 上海工字气枪",
								  u8"全金属仿真枪专卖",
								  u8"热武器QQ",
								  u8"三箭气枪出售",
								  u8"手枪气* 枪猎",
								  u8"手槍买卖自制手枪",
								  u8"售枪购买手枪",
								  u8"售手枪QQ",
								  u8"司马仿真枪模型QQ",
								  u8"淘宝气枪货到付款",
								  u8"提供气狗专卖店",
								  u8"提供专业及时的气狗",
								  u8"秃鹰气枪货到付款",
								  u8"锡峰气枪出售",
								  u8"仙鹤气枪出售",
								  u8"销售/专卖/买卖77式手枪",
								  u8"销售电手狗",
								  u8"销售气手狗",
								  u8"销售小口径步枪",
								  u8"销售运动步枪",
								  u8"小口径秃鹰沙漠",
								  u8"有军用枪支出售",
								  u8"长期出 售手枪",
								  u8"折叠狗QQ",
								  u8"转让 猎枪",
								  u8"子弹货到付款",
								  u8"自制手枪哪里买  ",
								  u8"自制手枪哪里有卖",
								  u8"阻击枪/汽枪/高压气枪",
								  u8"组装手枪  ",
								  u8"最新销售汽狗",
								  u8"出售警用手铐|仿真枪",
								  u8"猎枪|真枪",
								  u8"手枪|阻击枪",
								  u8"弓弩专营",
								  u8"弓弩专卖网货到付款",
								  u8"弓弩专卖网",
								  u8"弓弩专卖货到付款",
								  u8"弓弩专卖店网",
								  u8"弓弩专卖店",
								  u8"弓弩专卖",
								  u8"弓弩制作",
								  u8"弓弩直销",
								  u8"弓弩有限公司",
								  u8"弓弩营销中心",
								  u8"弓弩一百专卖店",
								  u8"弓弩销售",
								  u8"弓弩网 ",
								  u8"弓弩网",
								  u8"弓弩图纸",
								  u8"弓弩特许经销",
								  u8"弓弩狩猎网",
								  u8"自制手弩",
								  u8"追风弓弩麻醉箭专卖",
								  u8"专业弓弩网",
								  u8"中国战神军用弓弩",
								  u8"中国弩弓专卖",
								  u8"中国弓弩专卖网",
								  u8"中国弓弩直销",
								  u8"中国弓弩网",
								  u8"中国弓弩狩猎网",
								  u8"中国弓驽网",
								  u8"制作简易弓弩 ",
								  u8"郑州弓弩专卖",
								  u8"赵氏弓弩专卖网",
								  u8"赵氏弓弩专卖店",
								  u8"赵氏弓弩专卖",
								  u8"赵氏弓弩销售",
								  u8"小型弓弩专卖店",
								  u8"小猎人弓弩网",
								  u8"狩猎器材弓弩专卖",
								  u8"狩猎器材弓弩",
								  u8"狩猎弓弩专卖网",
								  u8"狩猎弓弩专卖",
								  u8"狩猎弓弩麻醉箭",
								  u8"手枪式折叠三用弩",
								  u8"三利达弓弩专卖网",
								  u8"三利达弓弩直营",
								  u8"三利达弓弩配件",
								  u8"三步倒药箭批发",
								  u8"三步倒弩箭专卖",
								  u8"三步倒麻醉弩箭销售",
								  u8"三步倒麻醉箭专卖",
								  u8"三步倒麻醉箭",
								  u8"三步倒捕狗药",
								  u8"军用弓弩专卖网",
								  u8"军用弓弩专卖店",
								  u8"军用弓弩批发",
								  u8"军用弓弩公司",
								  u8"供应三利达弓弩麻醉箭",
								  u8"供应三步倒麻醉箭",
								  u8"供应秦氏弓弩",
								  u8"供应弩用麻醉箭",
								  u8"供应弩捕狗箭",
								  u8"供应麻醉箭三步倒",
								  u8"供应麻醉箭批发",
								  u8"供应麻醉箭",
								  u8"供应军用弩折叠弩",
								  u8"供应军用弓弩专卖",
								  u8"供应精品弓弩",
								  u8"供应弓弩麻醉箭",
								  u8"供应弓弩",
								  u8"供应钢珠弓弩",
								  u8"弓弩商城专卖",
								  u8"弓弩商城",
								  u8"弓弩亲兄弟货到付款",
								  u8"弓弩批发",
								  u8"弓弩免定金货到付款",
								  u8"弓弩麻醉箭",
								  u8"弓弩麻醉镖",
								  u8"弓弩论坛 ",
								  u8"钢珠弓弩专卖网",
								  u8"钢珠弓弩专卖店",
								  u8"打狗弓弩三步倒",
								  u8"麻醉弓弩专卖店",
								  u8"出售军刀",
								  u8"出售军刺",
								  u8"出售弹簧刀",
								  u8"出售三棱刀",
								  u8"出售跳刀",
								  u8"军刀网",
								  u8"南方军刀网",
								  u8"户外军刀网",
								  u8"三棱军刺专卖",
								  u8"出售开山刀军刺",
								  u8"西点军刀网",
								  u8"军刀专 卖",
								  u8"戈博军刀",
								  u8"阿兰德龙户外",
								  u8"出售军品军刀",
								  u8"勃朗宁军刀",
								  u8"军刀军品网",
								  u8"阿兰得龙野营刀具网",
								  u8"出售军刺军刀",
								  u8"警用刀具出售",
								  u8"折刀专卖网",
								  u8"阳江军品军刀网",
								  u8"野营刀专卖",
								  u8"砍刀精品折刀专卖",
								  u8"匕首蝴蝶甩刀专卖",
								  u8"军刀专卖军刺",
								  u8"军刀专卖刀具批发",
								  u8"军刀图片砍刀",
								  u8"军刀网军刀专卖",
								  u8"军刀价格军用刀具",
								  u8"军品军刺网",
								  u8"军刀军刺甩棍",
								  u8"阳江刀具批发网",
								  u8"北方先锋军刀",
								  u8"正品军刺出售",
								  u8"野营军刀出售",
								  u8"开山刀砍刀出售",
								  u8"仿品军刺出售",
								  u8"军刀直刀专卖",
								  u8"手工猎刀专卖",
								  u8"自动跳刀专卖",
								  u8"军刀电棍销售",
								  u8"军刀甩棍销售",
								  u8"美国军刀出售",
								  u8"极端武力折刀",
								  u8"防卫棍刀户外刀具",
								  u8"阿兰德龙野营刀",
								  u8"仿品军刺网",
								  u8"野营砍刀户外军刀",
								  u8"手工猎刀户外刀具",
								  u8"中国户外刀具网",
								  u8"西点军品军刀网",
								  u8"野营开山刀军刺",
								  u8"三利达弓弩军刀",
								  u8"尼泊尔军刀出售",
								  u8"防卫野营砍刀出售",
								  u8"防卫著名军刀出售",
								  u8"防卫棍刀出售",
								  u8"防卫甩棍出售",
								  u8"防卫电棍出售",
								  u8"军刺野营砍刀出售",
								  u8"著名精品折刀出售",
								  u8"战术军刀出售",
								  u8"刺刀专卖网",
								  u8"户外军刀出售",
								  u8"阳江刀具直销网",
								  u8"冷钢刀具直销网",
								  u8"防卫刀具直销网",
								  u8"极端武力直销网",
								  u8"刀具直销网",
								  u8"军刀直销网",
								  u8"直刀匕首直销网",
								  u8"军刀匕首直销网",
								  u8"折刀砍刀军品网",
								  u8"野营刀具军品网",
								  u8"阳江刀具军品网",
								  u8"冷钢刀具军品网",
								  u8"防卫刀具军品网",
								  u8"极端武力军品网",
								  u8"军用刀具军品网",
								  u8"军刀直刀军品网",
								  u8"折刀砍刀专卖",
								  u8"野营刀具专卖",
								  u8"阳江刀具专卖",
								  u8"冷钢刀具专卖",
								  u8"防卫刀具专卖",
								  u8"出售美军现役军刀",
								  u8"兼职",
								  u8"招聘",
								  u8"网络",
								  u8"QQ",
								  u8"招聘",
								  u8"有意者",
								  u8"到货",
								  u8"本店",
								  u8"代购",
								  u8"扣扣",
								  u8"客服",
								  u8"微店",
								  u8"兼职",
								  u8"兼值",
								  u8"淘宝",
								  u8"小姐",
								  u8"妓女",
								  u8"包夜",
								  u8"3P",
								  u8"LY",
								  u8"JS",
								  u8"狼友",
								  u8"技师",
								  u8"推油",
								  u8"胸推",
								  u8"BT",
								  u8"毒龙",
								  u8"口爆",
								  u8"兼职",
								  u8"楼凤",
								  u8"足交",
								  u8"口暴",
								  u8"口交",
								  u8"全套",
								  u8"SM",
								  u8"桑拿",
								  u8"吞精",
								  u8"咪咪",
								  u8"婊子",
								  u8"乳方",
								  u8"操逼",
								  u8"全职",
								  u8"性伴侣",
								  u8"网购",
								  u8"网络工作",
								  u8"代理",
								  u8"专业代理",
								  u8"帮忙点一下",
								  u8"帮忙点下",
								  u8"请点击进入",
								  u8"详情请进入",
								  u8"私人侦探",
								  u8"私家侦探",
								  u8"针孔摄象",
								  u8"调查婚外情",
								  u8"信用卡提现",
								  u8"无抵押贷款",
								  u8"广告代理",
								  u8"原音铃声",
								  u8"借腹生子",
								  u8"找个妈妈",
								  u8"找个爸爸",
								  u8"代孕妈妈",
								  u8"代生孩子",
								  u8"代开发票",
								  u8"腾讯客服电话",
								  u8"销售热线",
								  u8"免费订购热线",
								  u8"低价出售",
								  u8"款到发货",
								  u8"回复可见",
								  u8"连锁加盟",
								  u8"加盟连锁",
								  u8"免费二级域名",
								  u8"免费使用",
								  u8"免费索取",
								  u8"蚁力神",
								  u8"婴儿汤",
								  u8"售肾",
								  u8"刻章办",
								  u8"买小车",
								  u8"套牌车",
								  u8"玛雅网",
								  u8"电脑传讯",
								  u8"视频来源",
								  u8"下载速度",
								  u8"高清在线",
								  u8"全集在线",
								  u8"在线播放",
								  u8"txt下载",
								  u8"六位qq",
								  u8"6位qq",
								  u8"位的qq",
								  u8"个qb",
								  u8"送qb",
								  u8"用刀横向切腹",
								  u8"完全自杀手册",
								  u8"四海帮",
								  u8"足球投注",
								  u8"地下钱庄",
								  u8"中国复兴党",
								  u8"阿波罗网",
								  u8"曾道人",
								  u8"六合彩",
								  u8"改卷内幕",
								  u8"替考试",
								  u8"隐形耳机",
								  u8"出售答案",
								  u8"考中答案",
								  u8"答an",
								  u8"da案",
								  u8"资金周转",
								  u8"救市",
								  u8"股市圈钱",
								  u8"崩盘",
								  u8"资金短缺",
								  u8"证监会",
								  u8"质押贷款",
								  u8"小额贷款",
								  u8"周小川",
								  u8"刘明康",
								  u8"尚福林",
								  u8"孔丹",
								  u8"分期u8" };
			string specialCharactersLibrary[212]{ u8"︿",
							   u8"）÷（１－",
							   u8"）、",
							   u8"＋ξ",
							   u8"＋＋",
							   u8"－β",
							   u8"－［＊］－",
							   u8"＜±",
							   u8"＜Δ",
							   u8"＜λ",
							   u8"＜φ",
							   u8"＜＜",
							   u8"＝″",
							   u8"＝☆",
							   u8"＝（",
							   u8"＝－",
							   u8"＝［",
							   u8"＝｛",
							   u8"＞λ",
							   u8"ＬＩ",
							   u8"Ｒ．Ｌ．",
							   u8"ＺＸＦＩＴＬ",
							   u8"［①①］",
							   u8"［①②］",
							   u8"［①③］",
							   u8"［①④］",
							   u8"［①⑤］",
							   u8"［①⑥］",
							   u8"［①⑦］",
							   u8"［①⑧］",
							   u8"［①⑨］",
							   u8"［①Ａ］",
							   u8"［①Ｂ］",
							   u8"［①Ｃ］",
							   u8"［①Ｄ］",
							   u8"［①Ｅ］",
							   u8"［①］",
							   u8"［①ａ］",
							   u8"［①ｃ］",
							   u8"［①ｄ］",
							   u8"［①ｅ］",
							   u8"［①ｆ］",
							   u8"［①ｇ］",
							   u8"［①ｈ］",
							   u8"［①ｉ］",
							   u8"［①ｏ］",
							   u8"［②",
							   u8"［②①］",
							   u8"［②②］",
							   u8"［②③］",
							   u8"［②④",
							   u8"［②⑤］",
							   u8"［②⑥］",
							   u8"［②⑦］",
							   u8"［②⑧］",
							   u8"［②⑩］",
							   u8"［②Ｂ］",
							   u8"［②Ｇ］",
							   u8"［②］",
							   u8"［②ａ］",
							   u8"［②ｂ］",
							   u8"［②ｃ］",
							   u8"［②ｄ］",
							   u8"［②ｅ］",
							   u8"［②ｆ］",
							   u8"［②ｇ］",
							   u8"［②ｈ］",
							   u8"［②ｉ］",
							   u8"［②ｊ］",
							   u8"［③①］",
							   u8"［③⑩］",
							   u8"［③Ｆ］",
							   u8"［③］",
							   u8"［③ａ］",
							   u8"［③ｂ］",
							   u8"［③ｃ］",
							   u8"［③ｄ］",
							   u8"［③ｅ］",
							   u8"［③ｇ］",
							   u8"［③ｈ］",
							   u8"［④］",
							   u8"［④ａ］",
							   u8"［④ｂ］",
							   u8"［④ｃ］",
							   u8"［④ｄ］",
							   u8"［④ｅ］",
							   u8"［⑤］",
							   u8"［⑤］］",
							   u8"［⑤ａ］",
							   u8"［⑤ｂ］",
							   u8"［⑤ｄ］",
							   u8"［⑤ｅ］",
							   u8"［⑤ｆ］",
							   u8"［⑥］",
							   u8"［⑦］",
							   u8"［⑧］",
							   u8"［⑨］",
							   u8"［⑩］",
							   u8"］∧′＝［",
							   u8"］［",
							   u8"＿",
							   u8"ａ］",
							   u8"ｂ］",
							   u8"ｃ］",
							   u8"ｅ］",
							   u8"ｆ］",
							   u8"ｎｇ昉",
							   u8"｛－",
							   u8"｝＞",
							   u8"～±",
							   u8"～＋",
							   u8"…………………………………………………③",
							   u8"′∈",
							   u8"′｜",
							   u8"∈［",
							   u8"∪φ∈",
							   u8"②ｃ",
							   u8"③］",
							   u8"》），",
							   u8"〕〔",

			};
			for (int i = 1; i < traditionalChineseLibrary->size() + 1; ++i) {
				if (str.find(traditionalChineseLibrary[i]) != string::npos) {
					return true;
				}
			}
			for (int i = 1; i < simplifiedChineseLibrary->size() + 1; ++i) {
				if (str.find(simplifiedChineseLibrary[i]) != string::npos) {
					return true;
				}
			}
			for (int i = 1; i < specialCharactersLibrary->size() + 1; ++i) {
				if (str.find(specialCharactersLibrary[i]) != string::npos) {
					return true;
				}
			}
			return false;
		}
	};
}

#endif
