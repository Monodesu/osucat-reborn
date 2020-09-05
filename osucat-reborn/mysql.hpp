#pragma once
#ifndef MYSQL_HPP
#define MYSQL_HPP

#include <mysql.h>
char SQL_USER[32], SQL_HOST[32], SQL_PWD[32], SQL_DATABASE[32];
int SQL_PORT;

using json = nlohmann::json;
using namespace osucat;

class Database {
public:
	/*
	 * 数据库对象
	 */
	Database() {
		mysql_init(&this->conn);
	}

	/*
	 * 连接到数据库
	 * 在使用完毕后必须调用Close()函数
	 */
	void Connect() {
		if (mysql_real_connect(&this->conn, SQL_HOST, SQL_USER, SQL_PWD, SQL_DATABASE, SQL_PORT, NULL, 0) == NULL) {
			// throw OsuCat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
		}
		if (int t = mysql_set_character_set(&conn, "utf8") != NULL) {
			mysql_close(&this->conn);
			// throw OsuCat::database_exception(mysql_error(&this->conn), t);
			throw osucat::database_exception(mysql_error(&this->conn), t);
		}
	}

	void Execute(const std::string& sql) {
		if (0 != mysql_query(&this->conn, sql.c_str())) {
			// throw OsuCat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
		}
	}

	void Commit() {
		mysql_commit(&this->conn);
	}

#pragma region SqlNative
	/*
	 * 执行Update语句
	 * 可能抛出的错误: std::exception
	 */
	void Update(const std::string& updateSql) {
		std::string query = updateSql;
		query.erase(0, query.find_first_not_of(" "));
		query.erase(query.find_last_not_of(" ") + 1);
		std::string sub = query.substr(0, 6);
		std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
		if (sub.compare("update")) {
			throw osucat::database_exception("This Query is not an Update Query!", 1047);
		}
		if (0 != mysql_query(&this->conn, updateSql.c_str())) {
			cout << mysql_errno(&this->conn) << endl;
			throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
		}
		mysql_commit(&this->conn);
	}

	/*
	 * 执行Insert语句
	 * 可能抛出的错误: std::exception
	 */
	void Insert(const std::string& insertSql) {
		std::string query = insertSql;
		query.erase(0, query.find_first_not_of(" "));
		query.erase(query.find_last_not_of(" ") + 1);
		std::string sub = query.substr(0, 6);
		std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
		if (sub.compare("insert")) {
			throw osucat::database_exception("This Query is not an Insert Query!", 1047);
		}
		if (0 != mysql_query(&this->conn, insertSql.c_str())) {
			throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
		}
		mysql_commit(&this->conn);
	}

	/*
	 * 执行Delete语句
	 * 可能抛出的错误: std::exception
	 */
	void Delete(const std::string& deleteSql) {
		std::string query = deleteSql;
		query.erase(0, query.find_first_not_of(" "));
		query.erase(query.find_last_not_of(" ") + 1);
		std::string sub = query.substr(0, 6);
		std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
		if (sub.compare("delete")) {
			throw osucat::database_exception("This Query is not an Delete Query!", 1047);
		}
		if (0 != mysql_query(&this->conn, deleteSql.c_str())) {
			cout << mysql_errno(&this->conn) << endl;
			throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
		}
		mysql_commit(&this->conn);
	}

	/*
	 * 执行Select语句
	 * 返回值: json_object数组
	 * 每一个object的key为字段，value为值的字符串表示
	 * 一个object为一行符合条件的数据
	 * 可能抛出的错误: std::exception, std::exception
	 */
	json Select(const std::string& selectSql) {
		std::string query = selectSql;
		query.erase(0, query.find_first_not_of(" "));
		query.erase(query.find_last_not_of(" ") + 1);
		std::string sub = query.substr(0, 6);
		std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
		if (sub.compare("select")) {
			throw osucat::database_exception("This Query is not an Selete Query!", 1047);
		}

		MYSQL_RES* result;
		MYSQL_ROW row;
		json ret;
		if (mysql_query(&this->conn, selectSql.c_str()) == 0) {
			result = mysql_store_result(&this->conn);
			if (NULL != result) {
				ret = nlohmann::json::array();

				unsigned int cFields = mysql_num_fields(result);
				MYSQL_FIELD* field;
				vector<string> fields(cFields);
				for (unsigned int i = 0; i < cFields; ++i) {
					field = mysql_fetch_field(result);
					fields[i] = field->name;
				}

				while (row = mysql_fetch_row(result)) {
					int i = 0;
					json aResult;
					for (unsigned int i = 0; i < cFields; ++i) {
						std::string value = row[i];
						aResult[fields[i]] = value;
					}
					ret.push_back(aResult);
				}
			}
			else {
				throw osucat::database_exception(mysql_error(&this->conn), mysql_errno(&this->conn));
			}
			mysql_free_result(result);
			if (0 == ret.size()) {
				throw osucat::database_exception("Empty Set!", 1065);
			}
			return ret;
		}
		else {
			throw std::exception(mysql_error(&this->conn), mysql_errno(&this->conn));
		}
	}
#pragma endregion

	void UserSet(int64_t userid, int64_t qq) {
		try {
			char query[1024];
			sprintf_s(query, 1024, "insert into info (uid, qq, v2_shownbadges, v2_signature) values (%I64d, %I64d, \"25565,25565,25565\", \"powered by osucat\")", userid, qq);
			this->Insert(query);
		}
		catch (osucat::database_exception) {
			//忽略
		}
	}

	void AddUserData(const osu_api_v1::user_info* info, const string& timeStr) {
		int64_t osu_uid = info->user_id;
		int64_t osu_count300 = info->n300;
		int64_t osu_count100 = info->n100;
		int64_t osu_count50 = info->n50;
		int64_t osu_playcount = info->playcount;
		int64_t osu_ranked_score = info->ranked_score;
		int64_t osu_total_score = info->total_score;
		int64_t osu_pp_rank = info->global_rank;
		double osu_level = info->level;
		double osu_pp_raw = info->pp;
		double osu_accuracy = info->accuracy;
		int64_t osu_count_rank_ss = info->count_ss;
		int64_t osu_count_rank_ssh = info->count_ssh;
		int64_t osu_count_rank_s = info->count_s;
		int64_t osu_count_rank_sh = info->count_sh;
		int64_t osu_count_rank_a = info->count_a;
		int64_t osu_total_seconds_played = info->playtime;
		int64_t osu_pp_country_rank = info->country_rank;
		char buffer[8192];
		sprintf_s(buffer,
			8192,
			"insert into info_record values (%lld,%lld,%lld,%lld,%lld,"
			"%lld,%lld,%.2f,%.2f,%.2f,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%d,\"%s\")",
			osu_uid,
			osu_count300,
			osu_count100,
			osu_count50,
			osu_playcount,
			osu_ranked_score,
			osu_total_score,
			osu_level,
			osu_pp_raw,
			osu_accuracy,
			osu_count_rank_ss,
			osu_count_rank_ssh,
			osu_count_rank_s,
			osu_count_rank_sh,
			osu_count_rank_a,
			osu_total_seconds_played,
			osu_pp_country_rank,
			osu_pp_rank,
			info->mode,
			timeStr.c_str());
		this->Insert(buffer);
	}

	/*0=success -1=no record*/
	int GetUserData(int64_t user_id, int days, osu_api_v1::user_info* info) {
		char query[1024];
		time_t now = time(NULL);
		char timeStr[30] = { 0 };
		json result;
		tm* tm_time = localtime(&now);
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_time);
		if (days == 0) {
			try {
				sprintf_s(query,
					1024,
					"select * from info_record where uid=%lld and gamemode=%d and lastupdate=\"%s\"",
					user_id,
					(int)info->mode,
					timeStr);
				result = this->Select(query);
			}
			catch (osucat::database_exception& e) {
				now = now - 86400;
				tm* tm_time = localtime(&now);
				strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_time);
				sprintf_s(query,
					1024,
					"select * from info_record where uid=%lld and gamemode=%d and lastupdate=\"%s\"",
					user_id,
					(int)info->mode,
					timeStr);
				result = this->Select(query);
			}
		}
		else {
			try {
				now = now - days * 86400;
				tm* tm_time = localtime(&now);
				strftime(timeStr, sizeof(timeStr), "%Y-%m-%d 04:00:00", tm_time);
				sprintf_s(query,
					1024,
					"select * from info_record where uid=%lld and gamemode=%d and lastupdate=\"%s\"",
					user_id,
					(int)info->mode,
					timeStr);
				result = this->Select(query);
			}
			catch (osucat::database_exception& e) {
				return -1;
			}
		}
		info->user_id = std::stoll(result[0]["uid"].get<std::string>());
		info->n300 = std::stoll(result[0]["n300"].get<std::string>());
		info->n100 = std::stoll(result[0]["n100"].get<std::string>());
		info->n50 = std::stoll(result[0]["n50"].get<std::string>());
		info->playcount = std::stoll(result[0]["PC"].get<std::string>());
		info->total_score = std::stoll(result[0]["TS"].get<std::string>());
		info->ranked_score = std::stoll(result[0]["RS"].get<std::string>());
		info->country_rank = std::stoll(result[0]["CR"].get<std::string>());
		info->global_rank = std::stoll(result[0]["GR"].get<std::string>());
		info->playtime = std::stoll(result[0]["PT"].get<std::string>());
		info->pp = std::stof(result[0]["PP"].get<std::string>());
		info->level = std::stof(result[0]["LV"].get<std::string>());
		info->accuracy = std::stod(result[0]["ACC"].get<std::string>());
		info->count_ssh = std::stoi(result[0]["XH"].get<std::string>());
		info->count_ss = std::stoi(result[0]["X"].get<std::string>());
		info->count_sh = std::stoi(result[0]["S"].get<std::string>());
		info->count_s = std::stoi(result[0]["SH"].get<std::string>());
		info->count_a = std::stoi(result[0]["A"].get<std::string>());
		return 0;
	}

	int SetUserMainMode(int64_t qq, int mode) {
		char buffer[1024];
		sprintf_s(buffer, 1024, "update info set mainmode=%d where qq=%lld", mode, qq);
		try {
			this->Update(buffer);
			return 1;
		}
		catch (osucat::database_exception) {
			return 0;
		}
	}

	int64_t GetQQ(int64_t uid) {
		char query[1024];
		sprintf_s(query, 1024, "select qq from info where uid=%lld", uid);
		try {
			json result = this->Select(query);
			return std::stoll(result[0]["qq"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return 0;
		}
	}

	vector<int> GetBadgeList(int64_t uid) {
		char query[1024];
		vector<int> ret;
		sprintf_s(query, 1024, "select badge from badge where uid=%lld", uid);
		try {
			badgeSystem::main bgs;
			json result = this->Select(query);
			ret = bgs.badgeParser(result[0]["badge"].get<string>());
		}
		catch (osucat::database_exception) {
		}
		catch (json::exception& ex) {
		}
		return ret;
	}

	void addbadge(int64_t uid, string str) {
		string query;
		vector<int> tmp = this->GetBadgeList(uid);
		if (tmp.size() == 0) {
			string query = "insert into badge (uid, badge) values (" + to_string(uid) + ", \"" + str + "\")";
			this->Insert(query);
		}
		else {
			string query = "update badge set badge=\"" + str + "\" where uid=" + to_string(uid);
			this->Update(query);
		}
	}

	int64_t GetUserID(int64_t qq) {
		char query[512];
		sprintf_s(query, 512, "select uid from info where qq=%lld", qq);
		try {
			json result = this->Select(query);
			return std::stoll(result[0]["uid"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return 0;
		}
	}

	int GetUserDefaultGameMode(int64_t uid) {
		char query[1024];
		sprintf_s(query, 1024, "select mainmode from info where uid=%lld", uid);
		try {
			json result = this->Select(query);
			return std::stoi(result[0]["mainmode"].get<std::string>());
		}
		catch (osucat::database_exception& e) {
			return 0;
		}
	}

	float GetUserPreviousPP(int64_t uid) {
		char query[1024];
		sprintf_s(query, 1024, "select pp from pplus where uid=%I64d", uid);
		try {
			json result = this->Select(query);
			return std::stof(result[0]["pp"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return 0.0;
		}
	}

	void GetUserPreviousPPlus(int64_t uid, osu_api_v1::pplus_info* pi) {
		char query[1024];
		sprintf_s(query, 1024, "select * from pplus where uid=%lld", uid);
		try {
			json result = this->Select(query);
			pi->acc = stoi(result[0]["acc"].get<std::string>());
			pi->flow = stoi(result[0]["flow"].get<std::string>());
			pi->spd = stoi(result[0]["spd"].get<std::string>());
			pi->sta = stoi(result[0]["sta"].get<std::string>());
			pi->pre = stoi(result[0]["pre"].get<std::string>());
			pi->jump = stoi(result[0]["jump"].get<std::string>());
		}
		catch (osucat::database_exception) {
			pi->acc = 0;
			pi->flow = 0;
			pi->spd = 0;
			pi->sta = 0;
			pi->pre = 0;
			pi->jump = 0;
		}
	}

	void UpdatePPlus(int64_t uid, float pp, osu_api_v1::pplus_info pi) {
		if (this->GetUserPreviousPP(uid) == 0.0) {
			string query = "insert into pplus (uid, pp, jump, flow, pre, acc, spd, sta) values (" + to_string(uid)
				+ ", " + to_string(pp) + ", " + to_string(pi.jump) + ", " + to_string(pi.flow) + ", "
				+ to_string(pi.pre) + ", " + to_string(pi.acc) + ", " + to_string(pi.spd) + ", "
				+ to_string(pi.sta) + ")";
			this->Insert(query);
		}
		else {
			string query = "update pplus set pp=" + to_string(pp) + ",jump=" + to_string(pi.jump)
				+ ",flow=" + to_string(pi.flow) + ",pre=" + to_string(pi.pre) + ",acc=" + to_string(pi.acc)
				+ ",spd=" + to_string(pi.spd) + ",sta=" + to_string(pi.sta) + " where uid=" + to_string(uid);
			this->Update(query);
		}
	}

	void UpdatePPRecord(int64_t qq, int64_t bid) {
		if (this->GetPPRecord(qq) == EOF) {
			string query = "insert into pprecord (qq, bid) values (" + to_string(qq) + ", " + to_string(bid) + ")";
			this->Insert(query);
		}
		else {
			string query = "update pprecord set bid=" + to_string(bid) + " where qq=" + to_string(qq);
			this->Update(query);
		}
	}

	int64_t GetPPRecord(int64_t qq) {
		try {
			json result = this->Select("select bid from pprecord where qq=" + to_string(qq));
			return std::stoll(result[0]["bid"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return EOF;
		}
	}

	int Getcallcount() {
		try {
			json result = this->Select("select count from count");
			return std::stoi(result[0]["count"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return EOF;
		}
	}

	vector<int64_t> GetUserSet() {
		vector<int64_t> ret;
		string query = "SELECT uid FROM info";
		json result = (this->Select(query));
		for (int i = 0; i < result.size(); ++i) {
			ret.push_back(stoll(result[i]["uid"].get<string>()));
		}
		return ret;
	}

	void addcallcount() {
		try {
			json result = this->Select("select count from count");
			string query = "update count set count=" + to_string(stoi(result[0]["count"].get<std::string>()) + 1);
			this->Update(query);
		}
		catch (osucat::database_exception) {
			return;
		}
	}

	bool add_blacklist(int64_t qq) {
		try {
			this->Insert("INSERT INTO blacklist (qq,is_blocked) values(" + to_string(qq) + ",1)");
			return true;
		}
		catch (osucat::database_exception) {
			return false;
		}
	}

	int is_Blocked(int64_t qq) {
		try {
			json result = this->Select("select is_blocked from blacklist where qq=" + to_string(qq));
			return std::stoi(result[0]["is_blocked"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return 0;
		}
	}

	/*
	int cmd
	1 = bp
	2 = rctpp
	3 = recent
	4 = entertainment
	5 = repeater
	*/
	int isGroupEnable(int64_t group_id, int cmd) {
		try {
			string _cmd;
			switch (cmd) {
			case 1:
				_cmd = "bp";
				break;
			case 2:
				_cmd = "rctpp";
				break;
			case 3:
				_cmd = "recent";
				break;
			case 4:
				_cmd = "entertainment";
				break;
			case 5:
				_cmd = "repeater";
				break;
			}
			json result = this->Select("select " + _cmd + " from group_settings where group_id=" + to_string(group_id));
			switch (cmd) {
			case 1:
				return std::stoi(result[0]["bp"].get<std::string>());
			case 2:
				return std::stoi(result[0]["rctpp"].get<std::string>());
			case 3:
				return std::stoi(result[0]["recent"].get<std::string>());
			case 4:
				return std::stoi(result[0]["entertainment"].get<std::string>());
			case 5:
				return std::stoi(result[0]["repeater"].get<std::string>());
			}

		}
		catch (osucat::database_exception) {
			this->setGroupSettings(group_id);
			return 1;
		}
	}

	/*
	int cmd
	1 = bp
	2 = rctpp
	3 = recent
	4 = entertainment
	5 = repeater
	*/
	void changeGroupSettings(int64_t group_id, int cmd, bool on_off) {
		try {
			string _cmd;
			switch (cmd) {
			case 1:
				_cmd = "bp";
				break;
			case 2:
				_cmd = "rctpp";
				break;
			case 3:
				_cmd = "recent";
				break;
			case 4:
				_cmd = "entertainment";
				break;
			case 5:
				_cmd = "repeater";
				break;
			}
			if (on_off == true) {
				_cmd += "=1";
			}
			else {
				_cmd += "=0";
			}
			string query = "update group_settings set " + _cmd + " where group_id=" + to_string(group_id);
			this->Update(query);
		}
		catch (osucat::database_exception) {
			cout << "修改群组设定失败" << endl;
		}
	}

	void setGroupSettings(int64_t group_id) {
		try {
			string query = "insert into group_settings (group_id) values (" + to_string(group_id) + ")";
			this->Insert(query);
		}
		catch (osucat::database_exception) {
			cout << "初始化群组设定失败" << endl;
		}
	}

	void setshownBadges(int64_t uid, int badgeid) {
		string query = "update info set shownbadgeID=" + to_string(badgeid) + " where uid=" + to_string(uid);
		this->Update(query);
	}

	int getshownbadges(int64_t uid) {
		string query;
		query = "select shownbadgeID from info where uid=" + to_string(uid);
		try {
			json result = this->Select(query);
			int value = std::stol(result[0]["shownbadgeID"].get<std::string>());
			if (value != 25565) {
				return value;
			}
			else {
				return EOF;
			}
		}
		catch (osucat::database_exception) {
			return EOF;
		}

	}

	int getunsetstatus(int64_t uid) {
		string query;
		query = "select verifying_unset from info where uid=" + to_string(uid);
		try {
			json result = this->Select(query);
			return std::stol(result[0]["verifying_unset"].get<std::string>());
		}
		catch (osucat::database_exception) {
			return 0;
		}
	}

	/*
	status
	true = 1 , false = 0
	*/
	void changeunsetstatus(int64_t uid, bool status) {
		int tmp;
		status == true ? tmp = 1 : tmp = 0;
		string query = "update info set verifying_unset=" + to_string(tmp) + " where uid=" + to_string(uid);
		this->Update(query);
	}

	void deleteuser(int64_t uid) {
		string query1 = "DELETE from info_record where uid = " + to_string(uid),
			query2 = "DELETE from info where uid = " + to_string(uid);
		this->Delete(query1);
		this->Delete(query2);
	}

	/*
	int isdaily
	1 = isdaily plus+5
	2 = plus
	3 = minus
	*/
	void setBottleRemaining(int isdaily, int64_t qq, int dailyoldvalue = 0) {
		if (isdaily == 1) {
			string query = "update bottlerecord set remaining=" + to_string(dailyoldvalue + 5) + ",lastrewardtime=" + to_string(time(NULL)) + " where qq=" + to_string(qq);
			this->Update(query);
			return;
		}
		if (isdaily == 2) {
			int oldr = this->getUserBottleRemaining(qq);
			string query = "update bottlerecord set remaining=" + to_string(oldr + 1) + " where qq=" + to_string(qq);
			this->Update(query);
			return;
		}
		if (isdaily == 3) {
			int oldr = this->getUserBottleRemaining(qq);
			string query = "update bottlerecord set remaining=" + to_string(oldr - 1) + " where qq=" + to_string(qq);
			this->Update(query);
			return;
		}
	}

	int getUserBottleRemaining(int64_t qq) {
		try {
			string query = "SELECT * FROM bottlerecord WHERE qq=" + to_string(qq);
			json j = this->Select(query);
			int64_t a = stoll(j[0]["lastrewardtime"].get<std::string>());
			int b = stoi(j[0]["remaining"].get<std::string>());
			char rtn[128];
			time_t tick = a;
			struct tm tm = { 0 };
			tm = *localtime(&tick);
			strftime(rtn, 128, "%d", &tm);
			string tmp1 = rtn;
			tick = time(NULL);
			tm = *localtime(&tick);
			strftime(rtn, 128, "%d", &tm);
			string tmp2 = rtn;
			if (tmp1 != tmp2) {
				this->setBottleRemaining(1, qq, b);
				j = this->Select(query);
				return stoi(j[0]["remaining"].get<std::string>());
			}
			else {
				return stoi(j[0]["remaining"].get<std::string>());
			}
		}
		catch (osucat::database_exception) {
			string query = "INSERT INTO bottlerecord (qq,remaining,lastrewardtime) VALUES (" + to_string(qq) + ",5," + to_string(time(NULL)) + ")";
			this->Insert(query);
			return 5;
		}
	}
	void Close() {
		if (this->conn.net.vio != NULL) mysql_close(&this->conn);
	}
	// 析构函数会判断数据库是否关闭，如果没有关闭就关闭数据库连接
	~Database() {
		Close();
	}

private:
	MYSQL conn;
};
#endif