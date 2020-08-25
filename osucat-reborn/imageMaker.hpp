#pragma once

#include <random>
#include "Hexagram.hpp"

#ifndef IMAGEMAKER_HPP
#define IMAGEMAKER_HPP

using namespace Magick;
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

	string infoPic_v1(const UserPanelData& data, const user_info& previoustUserInfo, bool isBonded = false) {
		char path_temp[512];
		char temp[1024];
		Image Info(Geometry(1200, 857), "none"); //初始化图像
		Image imtemp(Geometry(1200, 350), Color("none"));
		DrawableList dl; //初始化队列
		Info.density(Point(300, 300)); //图像分辨率
		TypeMetric metric;

		bool isCoverExist = osucat::utils::fileExist("./work/v1_cover/" + to_string(data.user_info.user_id) + ".jpg");
		sprintf_s(path_temp, 512, "./work/v1_cover/%lld.jpg", data.user_info.user_id);
		// cover
		if (isCoverExist == false) {
			string coverLink = NetConnection::GetBannerLink(data.user_info.user_id);
			if (coverLink.empty()) {
				char defaultCover[512];
				sprintf_s(defaultCover,
					"https://osu.ppy.sh/images/headers/profile-covers/c%I64d.jpg",
					osucat::utils::randomNum(1, 8));
				coverLink = defaultCover;
			}
			Image cover(coverLink);
			Image coverRoundrect(Geometry(1200, 350), Color("none"));
			cover.resize(Geometry(100000, 350));
			cover.crop(Geometry(1200, 350));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 1200, 350, 20, 20));
			coverRoundrect.draw(dl);
			coverRoundrect.composite(cover, 0, 0, InCompositeOp);
			coverRoundrect.quality(100);
			coverRoundrect.write("./work/v1_cover/" + to_string(data.user_info.user_id) + ".jpg");
			//logging::info_success(BOT_NAME, "用户Banner保存完成");
		}
		Image Cover("./work/v1_cover/" + to_string(data.user_info.user_id) + ".jpg");
		Info.composite(Cover, 0, 0, OverCompositeOp);
		// custom panel
		if (osucat::utils::fileExist("./work/v1_infopanel/" + to_string(data.user_info.user_id) + ".png") == true) {
			Image Panel("./work/v1_infopanel/" + to_string(data.user_info.user_id) + ".png");
			Info.composite(Panel, 0, 0, OverCompositeOp);
		}
		else {
			Image Panel("./work/default-info-v1.png");
			Info.composite(Panel, 0, 0, OverCompositeOp);
		}
		// avatar
		bool isAvatarExist = osucat::utils::fileExist("./work/avatar/" + to_string(data.user_info.user_id) + ".png");
		if (isAvatarExist == false) {
			default_random_engine rand((unsigned int)time(NULL));
			uniform_int_distribution<int> u(100000, 900000);
			sprintf_s(temp, "https://a.ppy.sh/%I64d?%d.jpg", data.user_info.user_id, u(rand));
			string name = "./work/avatar/" + std::to_string(data.user_info.user_id) + ".png";
			NetConnection::DownloadFile(temp, name);
			Image avatar(name);
			avatar.write("./work/avatar/" + to_string(data.user_info.user_id) + ".png");
			//logging::info_success(BOT_NAME, "用户Avatar保存完成");
		}
		try {
			Image avttemp(Geometry(190, 190), Color("none"));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 190, 190, 40, 40));
			avttemp.draw(dl);
			Image ava("./work/avatar/" + to_string(data.user_info.user_id) + ".png");
			ava.resize(Geometry(190, 190));
			avttemp.composite(ava, 0, 0, InCompositeOp);
			Info.composite(avttemp, 39, 55, OverCompositeOp);
		}
		catch (Magick::Exception) {
			DeleteFileA(("./work/avatar/" + to_string(data.user_info.user_id) + ".png").c_str());
			Image avatardef("./work/default_avatar.png");
			avatardef.resize(Geometry(190, 190));
			Image avttemp(Geometry(190, 190), Color("none"));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 190, 190, 40, 40));
			avttemp.draw(dl);
			avttemp.composite(avatardef, 0, 0, InCompositeOp);
			Info.composite(avttemp, 39, 55, OverCompositeOp);
		}

		// obj
		Image Flags("./work/Flags/" + data.user_info.country + ".png");
		Flags.resize(Geometry(57, 10000));
		Info.composite(Flags, 272, 212, OverCompositeOp);
		Image ModeIcon("./work/mode_icon/" + to_string(data.user_info.mode) + ".png");
		ModeIcon.resize(Geometry(64, 64));
		Info.composite(ModeIcon, 1125, 10, OverCompositeOp);

		dl.clear();
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawableFont("./work/fonts/Exo2-SemiBold.otf"));
		dl.push_back(DrawableFillColor("white"));
		dl.push_back(DrawablePointSize(60));
		dl.push_back(DrawableText(268, 120, data.user_info.username));
		dl.push_back(DrawablePointSize(20));
		if (previoustUserInfo.country_rank != data.user_info.country_rank) {
			if (previoustUserInfo.country_rank - data.user_info.country_rank > 0) {
				dl.push_back(DrawableText(335,
					248,
					"#" + osucat::utils::IntegerSplit(data.user_info.country_rank) + "(+"
					+ to_string(previoustUserInfo.country_rank - data.user_info.country_rank)
					+ ")"));
			}
			else {
				string crtemp = "#" + osucat::utils::IntegerSplit(data.user_info.country_rank) + "("
					+ to_string(previoustUserInfo.country_rank - data.user_info.country_rank) + ")";
				// utils::string_replace(crtemp, "-", "↓");
				dl.push_back(DrawableText(335, 248, crtemp));
			}
		}
		else {
			dl.push_back(DrawableText(335, 248, "#" + osucat::utils::IntegerSplit(data.user_info.country_rank)));
		}
		dl.push_back(DrawableFont("./work/fonts/Exo2-Regular.otf"));
		dl.push_back(DrawablePointSize(40));
		dl.push_back(DrawableText(40, 405, osucat::utils::IntegerSplit(data.user_info.global_rank)));
		dl.push_back(DrawableFont("./work/fonts/Alibaba-PuHuiTi-Regular.ttf"));
		if (isBonded == true) {
			if (previoustUserInfo.global_rank != data.user_info.global_rank) {
				dl.push_back(DrawablePointSize(14));
				if (previoustUserInfo.global_rank - data.user_info.global_rank > 0) {
					dl.push_back(DrawableText(
						40,
						425,
						"↑ " + osucat::utils::IntegerSplit(previoustUserInfo.global_rank - data.user_info.global_rank)));
				}
				else {
					string grntmp = osucat::utils::IntegerSplit(previoustUserInfo.global_rank - data.user_info.global_rank);
					utils::string_replace(grntmp, "-", "");
					grntmp[0] == ',' ? grntmp = grntmp.substr(1) : grntmp = grntmp;
					dl.push_back(DrawableText(40, 425, "↓ " + grntmp));
				}
			}
			else {
				dl.push_back(DrawablePointSize(14));
				dl.push_back(DrawableText(40, 425, "↑ -"));
			}
		}
		dl.push_back(DrawableFont("./work/fonts/Exo2-Regular.otf"));
		sprintf_s(temp, "%.2f", data.user_info.pp);
		dl.push_back(DrawablePointSize(40));
		dl.push_back(DrawableText(246, 405, temp));
		dl.push_back(DrawableFont("./work/fonts/Alibaba-PuHuiTi-Regular.ttf"));
		if (isBonded == true) {
			if (previoustUserInfo.pp != data.user_info.pp) {
				dl.push_back(DrawablePointSize(14));
				if (previoustUserInfo.pp - data.user_info.pp > 0.00) {
					sprintf_s(temp, "↓ %.2f", previoustUserInfo.pp - data.user_info.pp);
					dl.push_back(DrawableText(246, 425, temp));
				}
				else {
					sprintf_s(
						temp,
						"↑ %.2f",
						(previoustUserInfo.pp - data.user_info.pp) - (previoustUserInfo.pp - data.user_info.pp) * 2);
					dl.push_back(DrawableText(246, 425, temp));
				}
			}
			else {
				dl.push_back(DrawablePointSize(14));
				dl.push_back(DrawableText(246, 425, "↑ -"));
			}
		}
		dl.push_back(DrawableFont("./work/fonts/Exo2-Regular.otf"));
		dl.push_back(DrawablePointSize(30));
		dl.push_back(DrawableGravity(GravityType::CenterGravity));
		dl.push_back(DrawableText(-520, 95, to_string(data.user_info.count_ssh)));
		dl.push_back(DrawableText(-409, 95, to_string(data.user_info.count_ss)));
		dl.push_back(DrawableText(-299, 95, to_string(data.user_info.count_sh)));
		dl.push_back(DrawableText(-188, 95, to_string(data.user_info.count_s)));
		dl.push_back(DrawableText(-78, 95, to_string(data.user_info.count_a)));
		dl.push_back(DrawableFont("./work/fonts/Exo2-SemiBold.otf"));
		sprintf_s(temp, "%d", (int)data.user_info.level);
		dl.push_back(DrawablePointSize(34));
		dl.push_back(DrawableText(516, -64, temp));
		dl.push_back(DrawablePointSize(36));
		dl.push_back(DrawableGravity(GravityType::EastGravity));
		dl.push_back(DrawableFont("./work/fonts/Exo2-Medium.otf"));
		dl.push_back(DrawableText(20, 177, osucat::utils::IntegerSplit(data.user_info.ranked_score)));

		char acctmp1[64], acctmp2[64];
		sprintf_s(acctmp1, 64, "%.2f", previoustUserInfo.accuracy);
		sprintf_s(acctmp2, 64, "%.2f", data.user_info.accuracy);
		double preacc, nowacc;
		preacc = stod(to_string(acctmp1));
		nowacc = stod(to_string(acctmp2));
		if (preacc != nowacc) {
			if (preacc - nowacc > 0.00) {
				//+
				sprintf_s(temp, "%.2f%%(-%.2f%%)", data.user_info.accuracy, preacc - nowacc);
				dl.push_back(DrawableText(20, 217, temp));
			}
			else {
				//-
				sprintf_s(temp, "%.2f%%(+%.2f%%)", data.user_info.accuracy, abs(preacc - nowacc));
				dl.push_back(DrawableText(20, 217, temp));
			}
		}
		else {
			sprintf_s(temp, "%.2f%%", data.user_info.accuracy);
			dl.push_back(DrawableText(20, 217, temp));
		}
		if (previoustUserInfo.playcount != data.user_info.playcount) {
			dl.push_back(DrawableText(20,
				257,
				osucat::utils::IntegerSplit(data.user_info.playcount) + "(+"
				+ osucat::utils::IntegerSplit(data.user_info.playcount - previoustUserInfo.playcount)
				+ ")"));
		}
		else {
			dl.push_back(DrawableText(20, 257, osucat::utils::IntegerSplit(data.user_info.playcount)));
		}
		dl.push_back(DrawableText(20, 297, osucat::utils::IntegerSplit(data.user_info.total_score)));
		if (previoustUserInfo.n100 + previoustUserInfo.n300 + previoustUserInfo.n50
			!= data.user_info.n100 + data.user_info.n300 + data.user_info.n50) {
			dl.push_back(DrawableText(
				20,
				337,
				osucat::utils::IntegerSplit(data.user_info.n100 + data.user_info.n300 + data.user_info.n50) + "(+"
				+ osucat::utils::IntegerSplit((data.user_info.n100 + data.user_info.n300 + data.user_info.n50)
					- (previoustUserInfo.n100 + previoustUserInfo.n300 + previoustUserInfo.n50))
				+ ")"));
		}
		else {
			dl.push_back(DrawableText(
				20, 337, osucat::utils::IntegerSplit(data.user_info.n100 + data.user_info.n300 + data.user_info.n50)));
		}
		dl.push_back(DrawableText(20, 377, osucat::utils::Duration2String(data.user_info.playtime)));
		dl.push_back(DrawableFont("./work/fonts/Exo2-SemiBold.otf"));
		dl.push_back(DrawablePointSize(20));
		sprintf_s(temp, "%.2f", data.user_info.level - (int)data.user_info.level);
		sprintf_s(temp, "%d%%", (int)(stof(temp) * 100));
		dl.push_back(DrawableText(136, -40, temp));
		Info.draw(dl);
		// progress bar 此处顺序不能改，不要动这里。
		Image levelRoundrect(Geometry(4 * stoi(temp) + 4, 12), Color("none"));
		dl.clear();
		dl.push_back(DrawableFillColor("#FF66AB"));
		dl.push_back(DrawableRoundRectangle(0, 0, 4 * stoi(temp), 7, 3, 3));
		levelRoundrect.draw(dl);
		Info.composite(levelRoundrect, 662, 370, OverCompositeOp);
		//多模式mode图标绘制
		if (data.user_info.mode == 0) {
			// 获取pp+信息
			vector<long> pp_plus;
			// 剥离对pp+的依赖
			//if (data.user_info.updatepplus == true) {
			//    try {
			//        pp_plus = NetConnection::getUserPlusData(data.user_info.user_id);
			//        Database db;
			//        db.Connect();
			//        pplus_info pi;
			//        pi.acc = pp_plus[0];
			//        pi.flow = pp_plus[1];
			//        pi.jump = pp_plus[2];
			//        pi.pre = pp_plus[3];
			//        pi.spd = pp_plus[4];
			//        pi.sta = pp_plus[5];
			//        db.UpdatePPlus(data.user_info.user_id, data.user_info.pp, pi);
			//    } catch (std::exception) {
			//        pp_plus.push_back(0);
			//        pp_plus.push_back(0);
			//        pp_plus.push_back(0);
			//        pp_plus.push_back(0);
			//        pp_plus.push_back(0);
			//        pp_plus.push_back(0);
			//    }
			//} else {
			pp_plus.push_back(data.pplus_info.acc);
			pp_plus.push_back(data.pplus_info.flow);
			pp_plus.push_back(data.pplus_info.jump);
			pp_plus.push_back(data.pplus_info.pre);
			pp_plus.push_back(data.pplus_info.spd);
			pp_plus.push_back(data.pplus_info.sta);
			/*}*/
			// pp+ / 六芒星图
			Hexagram::HexagramInfo hi;
			hi.abilityFillColor = "rgba(253, 148, 62, 0.5)";
			hi.abilityLineColor = "#fd943e";
			hi.nodeMaxValue = 10000;
			hi.node_count = 6;
			hi.size = 200;
			hi.sideLength = 200;
			hi.mode = 1;
			hi.StrokeWidth = 2;
			hi.nodesize = 2.5f;
			// acc ,flow, jump, pre, speed, sta
			const vector<int> _x_offset = { 372, 330, 122, 52, 128, 317 }; // pp+数据的x轴坐标
			int pptemp = 0;
			dl.clear();
			dl.push_back(DrawableGravity(GravityType::NorthWestGravity));
			dl.push_back(DrawablePointSize(18));
			dl.push_back(DrawableFillColor("#FFCC33"));
			dl.push_back(DrawableFont("./work/fonts/Exo2-Regular.otf"));
			for (auto p : pp_plus) {
				sprintf_s(temp, 512, "(%d)", p);
				dl.push_back(DrawableText(_x_offset[pptemp++], pptemp % 3 ? (pptemp < 3 ? 620 : 809) : 714, temp));
			}

			const vector<double> multi = { 14.1, 69.7, 1.92, 19.8, 0.588, 3.06 };
			const vector<double> exp = { 0.769, 0.596, 0.953, 0.8, 1.175, 0.993 };
			Hexagram::draw_v1(pp_plus, multi, exp, &Info, 132, 626, hi);
			Image ppdataPanel("./work/pp+-v1.png");
			Info.composite(ppdataPanel, 0, 0, OverCompositeOp);
			Info.draw(dl);
		}
		else {
			Image ppdataPanel("./work/nopp+info-v1.png");
			Info.composite(ppdataPanel, 0, 0, OverCompositeOp);
		}
		dl.clear();
		dl.push_back(DrawableFont("./work/fonts/Exo2-Regular.otf"));
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawablePointSize(20));
		dl.push_back(DrawableFillColor("white"));
		time_t nowtime;
		nowtime = time(NULL);
		struct tm local = { 0 };
		localtime_s(&local, &nowtime);
		char buf[80];
		strftime(buf, 80, "%Y-%m-%d %H:%M:%S", &local);
		sprintf_s(temp, "update:   %s", to_string(buf).c_str());
		dl.push_back(DrawableText(10, 21, temp));

		// days_brfore
		if (previoustUserInfo.days_before > 0) {
			sprintf_s(temp, 512, "对比自%d天前", previoustUserInfo.days_before);
			dl.push_back(DrawableFont("./work/fonts/Alibaba-PuHuiTi-Regular.ttf"));
			dl.push_back(DrawableText(300, 21, temp));
		}

		Info.draw(dl);
		dl.clear();
		// draw badge
		if (data.badgeid != EOF) {
			Image Badge("./work/badges/" + to_string(data.badgeid) + ".png");
			Badge.resize(Geometry(86, 40));
			dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
			Info.composite(Badge, 272, 152, OverCompositeOp);
			Info.draw(dl);
		}
		Info.quality(100);
		char file[512];
		sprintf_s(file, "%s%s.jpg", osucat::utils::rand_str().c_str(), to_string(data.user_info.user_id).c_str());
		Info.write(to_string(OC_ROOT_PATH) + "\\data\\images\\osucat\\" + file);
		return file;
	}

	string infoPic_v2(const UserPanelData& data, const user_info& previoustUserInfo, bool isBonded = false) {
		char path_temp[512];
		char temp[1024];
		Image Info(Geometry(2400, 1714), "none");
		DrawableList dl; //初始化队列
		Info.density(Point(300, 300));
		TypeMetric metric;
		// background
		if (osucat::utils::fileExist("./work/v2_background/" + to_string(data.user_info.user_id) + ".png") == true) {
			Image bg("./work/v2_background/" + to_string(data.user_info.user_id) + ".png");
			Info.composite(bg, 0, 0, OverCompositeOp);
		}
		else {
			Image bg("./work/v2_background/default_backgrounds/default.png");
			// Panel.resize(Geometry(1200, 857));
			Info.composite(bg, 0, 0, OverCompositeOp);
		}
		// custom panel
		if (osucat::utils::fileExist("./work/v2_infopanel/" + to_string(data.user_info.user_id) + ".png") == true) {
			Image Panel("./work/v2_infopanel/" + to_string(data.user_info.user_id) + ".png");
			Info.composite(Panel, 0, 0, OverCompositeOp);
		}
		else {
			Image Panel("./work/default-info-v2.png");
			// Panel.resize(Geometry(1200, 857));
			Info.composite(Panel, 0, 0, OverCompositeOp);
		}
		// avatar
		bool isAvatarExist = osucat::utils::fileExist("./work/avatar/" + to_string(data.user_info.user_id) + ".png");
		if (isAvatarExist == false) {
			default_random_engine rand((unsigned int)time(NULL));
			uniform_int_distribution<int> u(100000, 900000);
			sprintf_s(temp, "https://a.ppy.sh/%I64d?%d.jpg", data.user_info.user_id, u(rand));
			string name = "./work/avatar/" + std::to_string(data.user_info.user_id) + ".png";
			NetConnection::DownloadFile(temp, name);
			Image avatar(name);
			avatar.write("./work/avatar/" + to_string(data.user_info.user_id) + ".png");
		}
		try {
			Image avttemp(Geometry(410, 410), Color("none"));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 410, 410, 60, 60));
			avttemp.draw(dl);
			Image ava("./work/avatar/" + to_string(data.user_info.user_id) + ".png");
			ava.resize(Geometry(410, 410));
			avttemp.composite(ava, 0, 0, InCompositeOp);
			Info.composite(avttemp, 89, 106, OverCompositeOp);
		}
		catch (Magick::Exception) {
			DeleteFileA(("./work/avatar/" + to_string(data.user_info.user_id) + ".png").c_str());
			Image avatardef("./work/avatar/default.png");
			avatardef.resize(Geometry(415, 415));
			Image avttemp(Geometry(410, 410), Color("none"));
			dl.clear();
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableRoundRectangle(0, 0, 430, 430, 60, 60));
			avttemp.draw(dl);
			avttemp.composite(avatardef, 0, 0, InCompositeOp);
			Info.composite(avttemp, 89, 106, OverCompositeOp);
		}
		// level
		dl.clear();
		dl.push_back(DrawableGravity(GravityType::CenterGravity));
		dl.push_back(DrawableFont("./work/fonts/Torus Regular.otf"));
		sprintf_s(temp, "%d", (int)data.user_info.level);
		dl.push_back(DrawablePointSize(42));
		dl.push_back(DrawableFillColor("white"));
		dl.push_back(DrawableText(-604, -652, temp));
		Info.draw(dl);
		dl.clear();
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		sprintf_s(temp, "%.2f", data.user_info.level - (int)data.user_info.level);
		sprintf_s(temp, "%d%%", (int)(stof(temp) * 100));
		dl.push_back(DrawableFont("./work/fonts/Torus Regular.otf"));
		dl.push_back(DrawablePointSize(20));
		dl.push_back(DrawableFillColor("white"));
		dl.push_back(DrawableText(1170, 289, temp));
		Info.draw(dl);
		dl.clear();
		Image levelRoundrect(Geometry(6 * stoi(temp) + 6, 14), Color("none"));
		Image levelBackGround(Geometry(606, 14), Color("none"));
		dl.push_back(DrawableFillColor("#FF92AC"));
		dl.push_back(DrawableRoundRectangle(0, 0, 6 * stoi(temp), 12, 6, 6));
		levelRoundrect.draw(dl);
		dl.clear();
		dl.push_back(DrawableFillColor("rgba(0, 0, 0, 0.32)"));
		dl.push_back(DrawableRoundRectangle(0, 0, 600, 12, 6, 6));
		levelBackGround.draw(dl);
		Info.composite(levelBackGround, 560, 277, OverCompositeOp);
		Info.composite(levelRoundrect, 560, 277, OverCompositeOp);
		dl.clear();
		// username
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawableFont("./work/fonts/Torus Regular.otf"));
		dl.push_back(DrawableFillColor("white"));
		dl.push_back(DrawablePointSize(60));
		dl.push_back(DrawableText(666, 208, data.user_info.username));
		Info.draw(dl);
		dl.clear();
		// ranks
		dl.push_back(DrawableGravity(GravityType::CenterGravity));
		dl.push_back(DrawableFont("./work/fonts/Torus Regular.otf"));
		dl.push_back(DrawableFillColor("white"));
		dl.push_back(DrawablePointSize(60));
		dl.push_back(DrawableText(-1016, -70, to_string(data.user_info.count_ssh)));
		dl.push_back(DrawableText(-765, -70, to_string(data.user_info.count_ss)));
		dl.push_back(DrawableText(-515, -70, to_string(data.user_info.count_sh)));
		dl.push_back(DrawableText(-246, -70, to_string(data.user_info.count_s)));
		dl.push_back(DrawableText(10, -70, to_string(data.user_info.count_a)));
		Info.draw(dl);
		dl.clear();




		// Hexagram
		vector<long> pp_plus;
		// 剥离对pp+的依赖
		/*if (data.user_info.updatepplus == true) {
			try {
				pp_plus = NetConnection::getUserPlusData(data.user_info.user_id);
				Database db;
				db.Connect();
				pplus_info pi;
				pi.acc = pp_plus[0];
				pi.flow = pp_plus[1];
				pi.jump = pp_plus[2];
				pi.pre = pp_plus[3];
				pi.spd = pp_plus[4];
				pi.sta = pp_plus[5];
				db.UpdatePPlus(data.user_info.user_id, data.user_info.pp, pi);
			} catch (std::exception) {
				pp_plus.push_back(0);
				pp_plus.push_back(0);
				pp_plus.push_back(0);
				pp_plus.push_back(0);
				pp_plus.push_back(0);
				pp_plus.push_back(0);
			}
		} else {*/
		pp_plus.push_back(data.pplus_info.acc);
		pp_plus.push_back(data.pplus_info.flow);
		pp_plus.push_back(data.pplus_info.jump);
		pp_plus.push_back(data.pplus_info.pre);
		pp_plus.push_back(data.pplus_info.spd);
		pp_plus.push_back(data.pplus_info.sta);
		//}
		Hexagram::HexagramInfo hi;
		hi.abilityFillColor = "rgba(255, 123, 172, 0.36)";
		//hi.abilityLineColor = "rgba(156, 126, 137, 0.98)";

		hi.abilityLineColor = "rgba(205, 157, 173, 0.95)";
		hi.nodeMaxValue = 10000;
		hi.node_count = 6;
		hi.size = 1000;
		hi.sideLength = 200;
		hi.mode = 3;
		hi.StrokeWidth = 8;
		hi.nodesize = 12;
		// acc ,flow, jump, pre, speed, sta
		const vector<int> _x_offset = { 812, 678, 220, 85, 220, 676 }; // pp+数据的x轴坐标
		int pptemp = 0;
		dl.clear();
		dl.push_back(DrawableGravity(GravityType::NorthWestGravity));
		dl.push_back(DrawablePointSize(18));
		dl.push_back(DrawableFillColor("#FFCC33"));
		dl.push_back(DrawableFont("./work/fonts/Exo2-Regular.otf"));
		for (auto p : pp_plus) {
			sprintf_s(temp, 512, "(%d)", p);
			dl.push_back(DrawableText(_x_offset[pptemp++], pptemp % 3 ? (pptemp < 3 ? 1050 : 1541) : 1295, temp));
		}

		const vector<double> multi = { 14.1, 69.7, 1.92, 19.8, 0.588, 3.06 };
		const vector<double> exp = { 0.769, 0.596, 0.953, 0.8, 1.175, 0.993 };
		Image hexagram(Geometry(1100, 1100), "none");
		Hexagram::draw_v1(pp_plus, multi, exp, &hexagram, 50, 50, hi);
		hexagram.resize(Geometry(664, 592));
		Info.composite(hexagram, 236, 1000, OverCompositeOp);
		Info.draw(dl);

















		Info.quality(100);

		char file[512];
		sprintf_s(file, "%s%s-v2.jpg", osucat::utils::rand_str().c_str(), to_string(data.user_info.user_id).c_str());
		Info.write(to_string(OC_ROOT_PATH) + "\\data\\images\\osucat\\" + file);
		return file;
	}

	string scorePic(const ScorePanelData& data) {
		Image score(Geometry(1950 - 2, 1088), "none"); //初始化图像
		DrawableList dl; //初始化队列
		score.density(Point(300, 300)); //图像分辨率
		TypeMetric metric;

		//获取背景图文件名
		char ctemp[512];
		string sub = to_string(data.beatmap_info.beatmapset_id);
		// https://unicom.sayobot.cn:25225/beatmaps/%lld/%s/files/
		sprintf_s(ctemp,
			"https://txy1.sayobot.cn/beatmaps/%lld/%s/files/",
			data.beatmap_info.beatmapset_id / 10000,
			data.beatmap_info.beatmapset_id / 10000 ? sub.substr(sub.size() - 4, 4).c_str() : sub.c_str());

		string temp = NetConnection::HttpsGet(ctemp);
		if (temp.length() > 5) {
			// const set<string> image_exts = {".jpg", ".jpeg", ".png", ".bmp"};
			string name = "";
			// size_t bgsize = 0;
			// json t = json::parse(temp);
			char beatmap_url[512];
			sprintf_s(beatmap_url, OSU_FILE_URL "%lld", data.score_info.beatmap_id);
			NetConnection::DownloadFile(
				beatmap_url, R"(./work/background/)" + std::to_string(data.beatmap_info.beatmap_id) + ".osu");
			ifstream beatmap(R"(./work/background/)" + std::to_string(data.beatmap_info.beatmap_id) + ".osu");
			string s;
			while (getline(beatmap, s)) {
				// cout << s << endl;
				if (s.find(".jpg") != s.npos) {
					name = s.substr(s.find_first_of('"') + 1, s.find_last_of('"') - s.find_first_of('"') - 1);
					break;
				}
				if (s.find(".jpeg") != s.npos) {
					name = s.substr(s.find_first_of('"') + 1, s.find_last_of('"') - s.find_first_of('"') - 1);
					break;
				}
				if (s.find(".png") != s.npos) {
					name = s.substr(s.find_first_of('"') + 1, s.find_last_of('"') - s.find_first_of('"') - 1);
					break;
				}
				if (s.find(".bmp") != s.npos) {
					name = s.substr(s.find_first_of('"') + 1, s.find_last_of('"') - s.find_first_of('"') - 1);
					break;
				}
			}

			// for (auto it : t) {
			//    if (it["type"].get<string>().compare("file")) continue;
			//    string q = it["name"].get<string>();
			//    size_t p = it["size"].get<size_t>();
			//    if (image_exts.count(q.substr(q.find_last_of('.'))) != 0 && p > bgsize) {
			//        name = q;
			//        bgsize = p;
			//    }
			//}
			if (!name.empty()) {
				strcat_s(ctemp, name.c_str());
				name = R"(./work/background/)" + std::to_string(data.beatmap_info.beatmap_id)
					+ name.substr(name.find_last_of('.'));
				if (!osucat::utils::fileExist(name)) {
					NetConnection::DownloadFile(ctemp, name);
				}
				try {
					Image bg(name);
					Image smallbg(name);
					Image roundrect(Geometry(433, 296), Color("none"));
					// beatmap info shadow
					dl.clear();
					dl.push_back(DrawableFillColor("white"));
					dl.push_back(DrawableRoundRectangle(0, 0, 433.0, 296.0, 20.0, 20.0));
					roundrect.draw(dl);
					//修改图片大小以适配左上角小图
					if ((double)smallbg.size().width() / smallbg.size().height() > (433.0 / 296.0)) {
						smallbg.resize(Geometry(296 / smallbg.size().height() * smallbg.size().width(), 296));
						smallbg.crop(Geometry(433, 296, (smallbg.size().width() - 433) / 2, 0));
					}
					else {
						smallbg.resize(Geometry(433, 433 / smallbg.size().width() * smallbg.size().height()));
						smallbg.crop(Geometry(433, 296, 0, (smallbg.size().height() - 296) / 2));
					}
					roundrect.composite(smallbg, 0, 0, InCompositeOp);
					//修改主图大小
					if ((double)bg.size().width() / bg.size().height() > (1952.0 / 1088.0)) {
						bg.resize(Geometry(1088.0 / bg.size().height() * bg.size().width(), 1088.0));
						bg.crop(Geometry(1952.0, 1088.0, (bg.size().width() - 1952.0) / 2, 0));
					}
					else {
						bg.resize(Geometry(1952.0, 1952.0 / bg.size().width() * bg.size().height()));
						bg.crop(Geometry(1952.0, 1088.0, 0, (bg.size().height() - 1088.0) / 2));
					}
					bg.gaussianBlur(5, 15);
					dl.clear();
					dl.push_back(DrawableFillColor("rgba(0,0,0,0.31)"));
					dl.push_back(DrawableRectangle(0, 0, 2000.0, 1100.0));
					bg.draw(dl);
					score.composite(bg, 0, 0, OverCompositeOp);
					score.composite(Image("./work/default-score-v2.png"), 0, 0, OverCompositeOp);
					score.composite(roundrect, 27, 34, OverCompositeOp);
				}
				catch (Magick::Exception) {
					score.composite(Image("./work/default-score-v2.png"), 0, 0, OverCompositeOp);
					DeleteFileA(name.c_str());
				}
			}
			else {
				//留空或者放置默认图片
				score.composite(Image("./work/default-score-v2.png"), 0, 0, OverCompositeOp);
			}

			// score
			score.font("./work/fonts/Torus Regular.otf");
			score.fillColor("white");
			dl.clear(); //清空队列
			score.fontPointsize(40);
			dl.push_back(DrawableTextAlignment(AlignType::CenterAlign));
			dl.push_back(DrawableText(975, 743, osucat::utils::IntegerSplit(data.score_info.score)));
			score.draw(dl);
			// title
			dl.clear();
			score.fontPointsize(60.66);
			temp.clear();
			for (auto c : data.pp_info.data.title) {
				temp += c;
				score.fontTypeMetrics(temp, &metric);
				if (metric.textWidth() > 725) {
					temp += "...";
					break;
				}
			}
			dl.push_back(DrawableFillColor("black"));
			dl.push_back(DrawableText(499, 105, temp));
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableText(499, 100, temp));
			score.draw(dl);
			// beatmap_info
			dl.clear();
			score.fontPointsize(40);
			temp.clear();
			for (auto c : data.pp_info.data.artist) {
				temp += c;
				score.fontTypeMetrics(temp, &metric);
				if (metric.textWidth() > 205) {
					temp += "...";
					break;
				}
			}
			dl.push_back(DrawableFillColor("black"));
			dl.push_back(DrawableText(519, 168, temp));
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableText(519, 165, temp));
			temp.clear();
			for (auto c : data.pp_info.data.creator) {
				temp += c;
				score.fontTypeMetrics(temp, &metric);
				if (metric.textWidth() > 145) {
					temp += "...";
					break;
				}
			}
			dl.push_back(DrawableFillColor("black"));
			dl.push_back(DrawableText(795, 168, temp));
			dl.push_back(DrawableText(1008, 168, std::to_string(data.score_info.beatmap_id)));
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableText(795, 165, temp));
			dl.push_back(DrawableText(1008, 165, std::to_string(data.score_info.beatmap_id)));
			score.draw(dl);
			// total pp
			dl.clear();
			score.fontPointsize(61);
			dl.push_back(DrawableTextAlignment(AlignType::RightAlign));
			dl.push_back(DrawableFillColor("#cf93ae"));
			score.fontTypeMetrics("pp", &metric);
			dl.push_back(DrawableText(1899, 493, "pp"));
			dl.push_back(DrawableFillColor("#fc65a9"));
			data.pp_info.data.total_pp.has_value() ? dl.push_back(
				DrawableText(1899 - metric.textWidth(), 493, std::to_string((int)data.pp_info.data.total_pp.value())))
				: dl.push_back(DrawableText(1899 - metric.textWidth(), 493, "-"));
			score.draw(dl);
			// object score
			if (data.mode == mode::std) {
				dl.clear();
				score.fontPointsize(53.09);
				dl.push_back(DrawableTextAlignment(AlignType::CenterAlign));
				dl.push_back(DrawableFillColor("black"));
				dl.push_back(DrawableText(793, 850, std::to_string(data.score_info.n300)));
				dl.push_back(DrawableText(793, 978, std::to_string(data.score_info.n100)));
				dl.push_back(DrawableText(1154, 850, std::to_string(data.score_info.n50)));
				dl.push_back(DrawableText(1154, 978, std::to_string(data.score_info.nmiss)));
				dl.push_back(DrawableFillColor("white"));
				dl.push_back(DrawableText(793, 847, std::to_string(data.score_info.n300)));
				dl.push_back(DrawableText(793, 975, std::to_string(data.score_info.n100)));
				dl.push_back(DrawableText(1154, 847, std::to_string(data.score_info.n50)));
				dl.push_back(DrawableText(1154, 975, std::to_string(data.score_info.nmiss)));
				score.draw(dl);
			}
			else {
				dl.clear();
				score.fontPointsize(53.09);
				dl.push_back(DrawableTextAlignment(AlignType::CenterAlign));
				dl.push_back(DrawableFillColor("black"));
				dl.push_back(DrawableText(
					793, 850, std::to_string(data.score_info.n300) + "/" + std::to_string(data.score_info.ngeki)));
				dl.push_back(DrawableText(
					793, 978, std::to_string(data.score_info.n100) + "/" + std::to_string(data.score_info.nkatu)));
				dl.push_back(DrawableText(1154, 850, std::to_string(data.score_info.n50)));
				dl.push_back(DrawableText(1154, 978, std::to_string(data.score_info.nmiss)));
				dl.push_back(DrawableFillColor("white"));
				dl.push_back(DrawableText(
					793, 847, std::to_string(data.score_info.n300) + "/" + std::to_string(data.score_info.ngeki)));
				dl.push_back(DrawableText(
					793, 975, std::to_string(data.score_info.n100) + "/" + std::to_string(data.score_info.nkatu)));
				dl.push_back(DrawableText(1154, 847, std::to_string(data.score_info.n50)));
				dl.push_back(DrawableText(1154, 975, std::to_string(data.score_info.nmiss)));
				score.draw(dl);
			}
			// ar,od info
			dl.clear();
			score.fontPointsize(24.25);
			int second = data.pp_info.data.length / 1000 % 60;
			int minute = data.pp_info.data.length / 1000 / 60;
			int maxsecond = data.pp_info.data.maxlength / 1000 % 60;
			int maxminute = data.pp_info.data.maxlength / 1000 / 60;
			char l[20];
			dl.push_back(DrawableFillColor("black"));
			sprintf_s(l, "%.2f", data.pp_info.data.bpm);
			dl.push_back(DrawableText(1457, 124, l));
			sprintf_s(l, "%.1f", data.pp_info.data.ar);
			dl.push_back(DrawableText(1457, 212, l));
			sprintf_s(l, "%.1f", data.pp_info.data.cs);
			dl.push_back(DrawableText(1457, 307, l));
			data.mode == mode::std ? sprintf_s(l, "%d:%02d / %d:%02d", minute, second, maxminute, maxsecond)
				: sprintf_s(l, " - ");
			dl.push_back(DrawableText(1741, 124, l));
			sprintf_s(l, "%.1f", data.pp_info.data.od);
			dl.push_back(DrawableText(1741, 212, l));
			sprintf_s(l, "%.1f", data.pp_info.data.hp);
			dl.push_back(DrawableText(1741, 307, l));
			dl.push_back(DrawableFillColor("#f1ce59"));
			sprintf_s(l, "%.2f", data.pp_info.data.bpm);
			dl.push_back(DrawableText(1457, 121, l));
			sprintf_s(l, "%.1f", data.pp_info.data.ar);
			dl.push_back(DrawableText(1457, 209, l));
			sprintf_s(l, "%.1f", data.pp_info.data.cs);
			dl.push_back(DrawableText(1457, 304, l));
			data.mode == mode::std ? sprintf_s(l, "%d:%02d / %d:%02d", minute, second, maxminute, maxsecond)
				: sprintf_s(l, " - ");
			dl.push_back(DrawableText(1741, 121, l));
			sprintf_s(l, "%.1f", data.pp_info.data.od);
			dl.push_back(DrawableText(1741, 209, l));
			sprintf_s(l, "%.1f", data.pp_info.data.hp);
			dl.push_back(DrawableText(1741, 304, l));
			score.draw(dl);
			// stars, version
			dl.clear();
			score.fontPointsize(26.37);
			dl.push_back(DrawableFillColor("black"));
			sprintf_s(l, "Stars: %.2f", data.pp_info.data.total_star);
			temp.clear();
			for (auto c : data.beatmap_info.version) {
				temp += c;
				score.fontTypeMetrics(temp, &metric);
				if (metric.textWidth() > 140) {
					temp += "...";
					break;
				}
			}
			dl.push_back(DrawableText(584, 314, temp));
			dl.push_back(DrawableText(584, 286, l));
			dl.push_back(DrawableFillColor("#f1ce59"));
			dl.push_back(DrawableText(584, 283, l));
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableText(584, 311, temp));
			score.draw(dl);

			// avatar
			bool isAvatarExist = osucat::utils::fileExist("./work/avatar/" + to_string(data.score_info.user_id) + ".png");

			name = "./work/avatar/" + std::to_string(data.score_info.user_id) + ".png";
			if (isAvatarExist == false) {
				NetConnection::DownloadFile(ctemp, name);
				default_random_engine rand((unsigned int)time(NULL));
				uniform_int_distribution<int> u(100000, 900000);
				//@@
				sprintf_s(ctemp, 512, "https://a.ppy.sh/%I64d?%d", data.score_info.user_id, u(rand));
				string name = "./work/avatar/" + std::to_string(data.score_info.user_id) + ".png";
				NetConnection::DownloadFile(ctemp, name);
				Image avatar(name);
				avatar.resize(Geometry(190, 190));
				avatar.write("./work/avatar/" + to_string(data.score_info.user_id) + ".png");
			}
			try {
				Image avatar(name);
				avatar.resize(Geometry(75, 75));
				Image imtemp(Geometry(80, 80), Color("none"));
				imtemp.fillColor(Color("white"));
				imtemp.draw(DrawableCircle(40, 40, 2.5, 40));
				imtemp.composite(avatar, 3, 3, InCompositeOp);
				imtemp.fillColor("none");
				imtemp.strokeWidth(1.0);
				imtemp.strokeColor("white");
				imtemp.draw(DrawableCircle(40, 40, 2.5, 40));
				score.composite(imtemp, 40, 425, OverCompositeOp);
			}
			catch (Magick::Exception) {
				DeleteFileA(("./work/avatar/" + to_string(data.user_info.user_id) + ".png").c_str());
				Image avatar("./work/default_avatar.png");
				avatar.resize(Geometry(75, 75));
				Image imtemp(Geometry(80, 80), Color("none"));
				imtemp.fillColor(Color("white"));
				imtemp.draw(DrawableCircle(40, 40, 2.5, 40));
				imtemp.composite(avatar, 3, 3, InCompositeOp);
				imtemp.fillColor("none");
				imtemp.strokeWidth(1.0);
				imtemp.strokeColor("white");
				imtemp.draw(DrawableCircle(40, 40, 2.5, 40));
				score.composite(imtemp, 40, 425, OverCompositeOp);
			}

			// time
			score.fontPointsize(27.61);
			tm tmt = { 0 };
			gmtime_s(&tmt, (time_t*)&data.score_info.achieved_timestamp);
			strftime(ctemp, 512, "%Y/%m/%d %H:%M", &tmt);
			dl.clear();
			dl.push_back(DrawableFillColor("black"));
			dl.push_back(DrawableText(145, 499, ctemp));
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableText(145, 496, ctemp));
			score.draw(dl);
			// diff circle
			// green, blue, yellow, red, purple, black
			// [0,2), [2,3), [3,4), [4,5), [5,7), [7,?)

			std::vector<std::string> ringFile;
			if (data.mode == mode::std) {
				ringFile.push_back("std-easy.png");
				ringFile.push_back("std-normal.png");
				ringFile.push_back("std-hard.png");
				ringFile.push_back("std-insane.png");
				ringFile.push_back("std-expert.png");
				ringFile.push_back("std-expertplus.png");
			};
			if (data.mode == mode::taiko) {
				ringFile.push_back("taiko-easy.png");
				ringFile.push_back("taiko-normal.png");
				ringFile.push_back("taiko-hard.png");
				ringFile.push_back("taiko-insane.png");
				ringFile.push_back("taiko-expert.png");
				ringFile.push_back("taiko-expertplus.png");
			};
			if (data.mode == mode::ctb) {
				ringFile.push_back("ctb-easy.png");
				ringFile.push_back("ctb-normal.png");
				ringFile.push_back("ctb-hard.png");
				ringFile.push_back("ctb-insane.png");
				ringFile.push_back("ctb-expert.png");
				ringFile.push_back("ctb-expertplus.png");
			};
			if (data.mode == mode::mania) {
				ringFile.push_back("mania-easy.png");
				ringFile.push_back("mania-normal.png");
				ringFile.push_back("mania-hard.png");
				ringFile.push_back("mania-insane.png");
				ringFile.push_back("mania-expert.png");
				ringFile.push_back("mania-expertplus.png");
			};
			if (data.pp_info.data.total_star < 2) {
				temp = ringFile[0];
			}
			else if (data.pp_info.data.total_star < 2.7) {
				temp = ringFile[1];
			}
			else if (data.pp_info.data.total_star < 4) {
				temp = ringFile[2];
			}
			else if (data.pp_info.data.total_star < 5.3) {
				temp = ringFile[3];
			}
			else if (data.pp_info.data.total_star < 6.5) {
				temp = ringFile[4];
			}
			else {
				temp = ringFile[5];
			}
			Image difficon("./work/icons/" + temp);
			difficon.resize(Geometry(65, 65));
			score.composite(difficon, 507, 257, OverCompositeOp);
			if (data.mode == mode::std) {
				// acc/pp
				dl.clear();
				score.fontPointsize(33.5);
				for (int i = 0; i < 5; ++i) {
					temp = std::to_string(data.confirmed_acc[i]);
					score.fontTypeMetrics(temp, &metric);
					dl.push_back(DrawableFillColor("#fc65a9"));
					dl.push_back(DrawableText(50 + 139 * i, 633, temp));
					dl.push_back(DrawableFillColor("#cf93ae"));
					dl.push_back(DrawableText(50 + 139 * i + metric.textWidth(), 633, "pp"));
				}
				score.draw(dl);
				// if fc
				dl.clear();
				score.fontPointsize(24.5);
				temp = to_string(data.fc);
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(99, 557, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(99 + metric.textWidth(), 557, "pp"));
				score.draw(dl);
				// aim, speed
				dl.clear();
				score.fontPointsize(33.5);
				temp = to_string((int)data.pp_info.data.aim_pp);
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(1532, 633, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(1532 + metric.textWidth(), 633, "pp"));
				temp = to_string((int)data.pp_info.data.speed_pp);
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(1672, 633, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(1672 + metric.textWidth(), 633, "pp"));
				temp = to_string((int)data.pp_info.data.acc_pp);
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(1812, 633, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(1812 + metric.textWidth(), 633, "pp"));
				score.draw(dl);
			}
			else {
				// acc/pp
				dl.clear();
				score.fontPointsize(33.5);
				for (int i = 0; i < 5; ++i) {
					temp = "-";
					score.fontTypeMetrics(temp, &metric);
					dl.push_back(DrawableFillColor("#fc65a9"));
					dl.push_back(DrawableText(50 + 139 * i, 633, temp));
					dl.push_back(DrawableFillColor("#cf93ae"));
					dl.push_back(DrawableText(50 + 139 * i + metric.textWidth(), 633, "pp"));
				}
				score.draw(dl);
				// if fc
				dl.clear();
				score.fontPointsize(24.5);
				temp = "-";
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(99, 557, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(99 + metric.textWidth(), 557, "pp"));
				score.draw(dl);
				// aim, speed
				dl.clear();
				score.fontPointsize(33.5);
				temp = "-";
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(1532, 633, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(1532 + metric.textWidth(), 633, "pp"));
				temp = "-";
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(1672, 633, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(1672 + metric.textWidth(), 633, "pp"));
				temp = "-";
				score.fontTypeMetrics(temp, &metric);
				dl.push_back(DrawableFillColor("#fc65a9"));
				dl.push_back(DrawableText(1812, 633, temp));
				dl.push_back(DrawableFillColor("#cf93ae"));
				dl.push_back(DrawableText(1812 + metric.textWidth(), 633, "pp"));
				score.draw(dl);
			}

			// acc
			dl.clear();
			score.fontPointsize(53.56);
			dl.push_back(DrawableTextAlignment(AlignType::CenterAlign));
			sprintf_s(ctemp, 512, "%.2f%%", data.pp_info.data.accuracy);
			dl.push_back(DrawableFillColor("black"));
			dl.push_back(DrawableText(360, 961, ctemp));
			// ColorHSL hsl("#ffbd1f");
			ColorHSL hsl("#87ff6a");
			hsl.hue(ctemp == "100.00%"
				? hsl.hue()
				: hsl.hue() * (data.pp_info.data.accuracy < 60 ? 0 : (data.pp_info.data.accuracy - 60) / 40.0));
			dl.push_back(DrawableFillColor(hsl));
			dl.push_back(DrawableText(360, 958, ctemp));
			// combo
			if (data.mode == mode::std || data.mode == mode::ctb) {
				dl.push_back(DrawableFillColor("black"));
				dl.push_back(DrawableText(1598, 961, "/"));
				dl.push_back(DrawableFillColor("white"));
				dl.push_back(DrawableText(1598, 958, "/"));
				score.fontTypeMetrics("/", &metric);
				sprintf_s(ctemp, 512, "%dx", data.pp_info.data.combo);
				dl.push_back(DrawableTextAlignment(AlignType::RightAlign));
				dl.push_back(DrawableFillColor("black"));
				dl.push_back(DrawableText(1598 - metric.textWidth() / 2.0, 961, ctemp));
				hsl = ColorHSL("#87ff6a");
				// hsl.hue(hsl.hue() * data.pp_info.data.combo / data.pp_info.data.maxcombo);
				hsl.hue(data.pp_info.data.combo == data.pp_info.data.maxcombo
					? hsl.hue()
					: hsl.hue() * data.pp_info.data.combo / data.pp_info.data.maxcombo);

				dl.push_back(DrawableFillColor(hsl));
				dl.push_back(DrawableText(1598 - metric.textWidth() / 2.0, 958, ctemp));
				sprintf_s(ctemp, 512, "%dx", data.pp_info.data.maxcombo);
				dl.push_back(DrawableTextAlignment(AlignType::LeftAlign));
				dl.push_back(DrawableFillColor("black"));
				dl.push_back(DrawableText(1598 + metric.textWidth() / 2.0, 961, ctemp));
				dl.push_back(DrawableFillColor("#87ff6a"));
				dl.push_back(DrawableText(1598 + metric.textWidth() / 2.0, 958, ctemp));
			}
			else {
				sprintf_s(ctemp, 512, "%dx", data.pp_info.data.combo);
				dl.push_back(DrawableTextAlignment(AlignType::CenterAlign));
				dl.push_back(DrawableFillColor("black"));
				dl.push_back(DrawableText(1598, 961, ctemp));
				dl.push_back(DrawableFillColor("#87ff6a"));
				dl.push_back(DrawableText(1598, 958, ctemp));
			}
			score.draw(dl);
			// username
			score.font("./work/fonts/Torus SemiBold.otf");
			score.fontPointsize(36.0);
			dl.clear();
			// get username
			user_info UI = { 0 };
			api::GetUser(data.score_info.user_id, mode::std, &UI);
			// draw useranme
			dl.push_back(DrawableFillColor("black"));
			dl.push_back(DrawableText(145, 461, UI.username));
			dl.push_back(DrawableFillColor("white"));
			dl.push_back(DrawableText(145, 458, UI.username));
			score.draw(dl);
			switch (data.beatmap_info.approved) {
			case beatmap_status::ranked:
				score.composite(Image("./work/icons/ranked.png"), 415, 16, OverCompositeOp);
				break;
			case beatmap_status::approved:
				score.composite(Image("./work/icons/approved.png"), 415, 16, OverCompositeOp);
				break;
			case beatmap_status::loved:
				score.composite(Image("./work/icons/loved.png"), 415, 16, OverCompositeOp);
				break;
			default:
				break;
			}
			sprintf_s(ctemp, 512, "./work/ranking/ranking-%s.png", data.score_info.rank.c_str());
			score.composite(Image(ctemp), 913, 874, OverCompositeOp);

			// mods
			if (data.score_info.mods != 0) {
				string modsname = api::modParser(data.score_info.mods);
				int32_t modcounts = modsname.length() / 2;
				for (int32_t i = 1; i <= modcounts; ++i) {
					if (i == 1) {
						Image MI("./work/mods/" + modsname.substr(0, 2) + ".png");
						MI.resize(Geometry(200, 61));
						score.composite(MI, 440, 440, OverCompositeOp);
					}
					else if (i <= modcounts) {
						Image MI("./work/mods/" + modsname.substr(i == 2 ? 2 : (i - 1) * 2, 2) + ".png");
						MI.resize(Geometry(200, 61));
						score.composite(
							// MI, i == 2 ? 440 + 200 : 440 + (i - 1) * 200, 440, OverCompositeOp);
							MI,
							i == 2 ? 440 + 160 : 440 + (i - 1) * 160,
							440,
							OverCompositeOp);
					}
				}
				// 440x420
			}

			score.quality(100);

			char file[512];
			sprintf_s(file, "%s%s.jpg", osucat::utils::rand_str().c_str(), to_string(data.score_info.user_id).c_str());
			score.write(to_string(OC_ROOT_PATH) + "\\data\\images\\osucat\\" + file);
			return file;
		}
		else {
			throw osucat::NetWork_Exception::exception("访问sayoapi时发生了错误。", GetLastError());
		}
	} // namespace ImageMaker

	string ppvsimg(UserPanelData& u1d, UserPanelData& u2d) {
		char path_temp[512];
		char temp[1024];
		Image ppvspanel(R"(.\work\ppvs.png)");
		DrawableList dl; //初始化队列
		TypeMetric metric;

#pragma region hexagram
		vector<long> pp_plus;
		vector<long> pp_plus2;
		try {
			if (u2d.user_info.updatepplus == true) {
				pp_plus = NetConnection::getUserPlusData(u2d.user_info.user_id);
				Database db;
				db.Connect();
				pplus_info pi;
				pi.acc = pp_plus[0];
				pi.flow = pp_plus[1];
				pi.jump = pp_plus[2];
				pi.pre = pp_plus[3];
				pi.spd = pp_plus[4];
				pi.sta = pp_plus[5];
				db.UpdatePPlus(u2d.user_info.user_id, u2d.user_info.pp, pi);
			}
			else {
				pp_plus.push_back(u2d.pplus_info.acc);
				pp_plus.push_back(u2d.pplus_info.flow);
				pp_plus.push_back(u2d.pplus_info.jump);
				pp_plus.push_back(u2d.pplus_info.pre);
				pp_plus.push_back(u2d.pplus_info.spd);
				pp_plus.push_back(u2d.pplus_info.sta);
			}

		}
		catch (std::exception) {
			pp_plus.push_back(0);
			pp_plus.push_back(0);
			pp_plus.push_back(0);
			pp_plus.push_back(0);
			pp_plus.push_back(0);
			pp_plus.push_back(0);
		}
		Hexagram::HexagramInfo hi;
		hi.abilityLineColor = "#FF7BAC";
		hi.node_count = 6;
		hi.size = 1134;
		hi.sideLength = 791;
		hi.mode = 2;
		hi.StrokeWidth = 6;
		hi.nodesize = 10.0f;
		const vector<double> multi = { 14.1, 69.7, 1.92, 19.8, 0.588, 3.06 };
		const vector<double> exp = { 0.769, 0.596, 0.953, 0.8, 1.175, 0.993 };
		try {
			if (u1d.user_info.updatepplus == true) {
				pp_plus2 = NetConnection::getUserPlusData(u1d.user_info.user_id);
				Database db;
				db.Connect();
				pplus_info pi;
				pi.acc = pp_plus2[0];
				pi.flow = pp_plus2[1];
				pi.jump = pp_plus2[2];
				pi.pre = pp_plus2[3];
				pi.spd = pp_plus2[4];
				pi.sta = pp_plus2[5];
				db.UpdatePPlus(u1d.user_info.user_id, u1d.user_info.pp, pi);
			}
			else {
				pp_plus2.push_back(u1d.pplus_info.acc);
				pp_plus2.push_back(u1d.pplus_info.flow);
				pp_plus2.push_back(u1d.pplus_info.jump);
				pp_plus2.push_back(u1d.pplus_info.pre);
				pp_plus2.push_back(u1d.pplus_info.spd);
				pp_plus2.push_back(u1d.pplus_info.sta);
			}
		}
		catch (std::exception& ex) {
			pp_plus2.push_back(0);
			pp_plus2.push_back(0);
			pp_plus2.push_back(0);
			pp_plus2.push_back(0);
			pp_plus2.push_back(0);
			pp_plus2.push_back(0);
		}
		// pp+ / 六芒星图
		Hexagram::HexagramInfo hi2;
		hi2.abilityLineColor = "#29ABE2";
		hi2.node_count = 6;
		hi2.size = 1134;
		hi2.sideLength = 791;
		hi2.mode = 2;
		hi2.StrokeWidth = 6;
		hi2.nodesize = 10.0f;
#pragma endregion

		hi.nodeMaxValue = 12000;
		hi2.nodeMaxValue = 12000;
		if (u1d.user_info.pp < u2d.user_info.pp) {
			hi.abilityFillColor = "rgba(255, 123, 172, 0.4)";
			hi2.abilityFillColor = "rgba(41, 171, 226, 0.4)";
			Hexagram::draw_v1(pp_plus, multi, exp, &ppvspanel, 0, -120, hi);
			Hexagram::draw_v1(pp_plus2, multi, exp, &ppvspanel, 0, -120, hi2);
		}
		else {
			hi.abilityFillColor = "rgba(255, 123, 172, 0.4)";
			hi2.abilityFillColor = "rgba(41, 171, 226, 0.4)";
			Hexagram::draw_v1(pp_plus2, multi, exp, &ppvspanel, 0, -120, hi2);
			Hexagram::draw_v1(pp_plus, multi, exp, &ppvspanel, 0, -120, hi);
		}

		const vector<int> _y_offset = { 1052, 1136, 1220, 1304, 1389, 1471 }; // pp+数据的y轴坐标
		int pptemp = 0;
		dl.clear();
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawablePointSize(32));
		dl.push_back(DrawableFillColor("#999999"));
		dl.push_back(DrawableFont("./work/fonts/AvenirLTStd-Medium.otf"));

		sprintf_s(temp, 512, "%d", pp_plus[2]);
		dl.push_back(DrawableText(664, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus[1]);
		dl.push_back(DrawableText(664, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus[3]);
		dl.push_back(DrawableText(664, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus[4]);
		dl.push_back(DrawableText(664, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus[5]);
		dl.push_back(DrawableText(664, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus[0]);
		dl.push_back(DrawableText(664, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%.2f", u2d.user_info.pp);
		dl.push_back(DrawableText(664, 966, temp));
		ppvspanel.draw(dl);

		pptemp = 0;
		dl.clear();
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawablePointSize(32));
		dl.push_back(DrawableFillColor("#999999"));
		dl.push_back(DrawableFont("./work/fonts/AvenirLTStd-Medium.otf"));

		sprintf_s(temp, 512, "%d", pp_plus2[2]);
		dl.push_back(DrawableText(424, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus2[1]);
		dl.push_back(DrawableText(424, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus2[3]);
		dl.push_back(DrawableText(424, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus2[4]);
		dl.push_back(DrawableText(424, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus2[5]);
		dl.push_back(DrawableText(424, _y_offset[pptemp++], temp));
		sprintf_s(temp, 512, "%d", pp_plus2[0]);
		dl.push_back(DrawableText(424, _y_offset[pptemp++], temp));

		sprintf_s(temp, 512, "%.2f", u1d.user_info.pp);
		dl.push_back(DrawableText(424, 966, temp));
		ppvspanel.draw(dl);
		dl.clear();

		int diffpoint = 960;
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawablePointSize(32));
		dl.push_back(DrawableFillColor("#ffcd22"));
		dl.push_back(DrawableFont("./work/fonts/AvenirLTStd-Medium.otf"));
		sprintf_s(temp, 512, "%.2f", u1d.user_info.pp - u2d.user_info.pp);
		dl.push_back(DrawableText(diffpoint, 966, temp));
		sprintf_s(temp, 512, "%d", pp_plus2[2] - pp_plus[2]);
		dl.push_back(DrawableText(diffpoint, 1052, temp));
		sprintf_s(temp, 512, "%d", pp_plus2[1] - pp_plus[1]);
		dl.push_back(DrawableText(diffpoint, 1136, temp));
		sprintf_s(temp, 512, "%d", pp_plus2[3] - pp_plus[3]);
		dl.push_back(DrawableText(diffpoint, 1220, temp));
		sprintf_s(temp, 512, "%d", pp_plus2[4] - pp_plus[4]);
		dl.push_back(DrawableText(diffpoint, 1304, temp));
		sprintf_s(temp, 512, "%d", pp_plus2[5] - pp_plus[5]);
		dl.push_back(DrawableText(diffpoint, 1389, temp));
		sprintf_s(temp, 512, "%d", pp_plus2[0] - pp_plus[0]);
		dl.push_back(DrawableText(diffpoint, 1471, temp));
		ppvspanel.draw(dl);

		dl.clear();
		dl.push_back(DrawableGravity(GravityType::UndefinedGravity));
		dl.push_back(DrawablePointSize(36));
		dl.push_back(DrawableFillColor("#999999"));
		dl.push_back(DrawableFont("./work/fonts/AvenirLTStd-Medium.otf"));
		dl.push_back(DrawableText(264, 874, u1d.user_info.username));
		dl.push_back(DrawableText(808, 874, u2d.user_info.username));
		ppvspanel.draw(dl);
		ppvspanel.composite(Image("./work/ppvs_title.png"), 0, 0, OverCompositeOp);
		ppvspanel.quality(100);

		char file[512];
		sprintf_s(file, "%s%s.jpg", osucat::utils::rand_str().c_str(), to_string(u1d.user_info.user_id).c_str());
		ppvspanel.write(to_string(OC_ROOT_PATH) + "\\data\\images\\osucat\\" + file);
		return file;
	}
}
#endif