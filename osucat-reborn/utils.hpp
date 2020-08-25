#pragma once

#include <algorithm> 
#include <cctype>
#include <locale>
#include <ctime>

namespace osucat::utils {


	static std::string unixTime2Str(int timetick)
	{
		char rtn[128];
		time_t tick;
		struct tm tm = { 0 };
		tick = timetick;
		tm = *localtime(&tick);
		strftime(rtn, 128, "%Y-%m-%d %H:%M:%S", &tm);
		return rtn;
	}
	static inline void string_replace(std::string& str, const std::string& old_val, const std::string& new_val) {
		// see https://stackoverflow.com/a/29752943

		std::string res;
		res.reserve(str.length()); // avoids a few memory allocations

		std::string::size_type last_pos = 0;
		std::string::size_type pos;
		while (std::string::npos != (pos = str.find(old_val, last_pos))) {
			res.append(str, last_pos, pos - last_pos);
			res += new_val;
			last_pos = pos + old_val.length();
		}

		// care for the rest after last occurrence
		res += str.substr(last_pos);
		str.swap(res);
	}
	/*
	about trim see : https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
	*/
	static inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	}
	static inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}
	static inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}
	inline std::string ws2s(const std::wstring& ws) {
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(ws);
	}
	inline std::wstring s2ws(const std::string& s) {
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(s);
	}
	static std::string IntegerSplit(int64_t number, const char inpent = ',') {
		bool flag = number < 0 ? true : false;
		number = number < 0 ? 0 - number : number;
		std::string ret = "";
		int counter = 0;
		while (number > 0) {
			int bit = number % 10;
			ret = std::to_string(bit) + ret;
			++counter;
			if (counter == 3) {
				ret = inpent + ret;
				counter = 0;
			}
			number /= 10;
		}
		std::string rtn = flag ? '-' + ret : ret;
		if (rtn[0] == ',') {
			rtn = rtn.substr(1);
		}
		return rtn;
	}
	static std::string Duration2String(uint64_t duration) {
		uint64_t day, hour, minute, second;
		day = duration / 86400;
		duration %= 86400;
		hour = duration / 3600;
		duration %= 3600;
		minute = duration / 60;
		second = duration % 60;

		char buffer[128];
		sprintf_s(buffer, 128, "%I64dd %I64dh %I64dm %I64ds", day, hour, minute, second);
		return buffer;
	}
	static std::string Duration2StrWithoutDAY(uint64_t duration) {
		uint64_t hour, minute, second;
		hour = duration / 3600;
		duration %= 3600;
		minute = duration / 60;
		second = duration % 60;
		char buffer[512];
		sprintf_s(buffer, 512, u8"%lld小时%lld分钟%lld秒", hour, minute, second);
		return buffer;
	}
	static std::string timeStampToDate(time_t rawtime) {
		struct tm dt = { 0 };
		char buffer[30];
		localtime_s(&dt, &rawtime);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &dt);
		return std::string(buffer);
	}
	static bool fileExist(const std::string& path) {
		// struct stat buffer;
		// return (stat(path.c_str(), &buffer) == 0);
		std::fstream file;
		file.open(path, std::ios::in);
		if (!file) {
			return false;
		}
		else {
			return true;
		}
		return false;
	}
	static int64_t randomNum(int minNum, int maxNum) {
		srand((unsigned)GetTickCount());
		return rand() % (maxNum - minNum + 1) + minNum;
	}
	static std::vector<std::string> string_split(const std::string& original_str, char split_character) {
		std::string tmp;
		std::vector<std::string> rtn;
		for (size_t i = 0; i < original_str.size(); ++i) {
			char c = original_str[i];
			if (c == split_character) {
				rtn.push_back(tmp);
				tmp.clear();
			}
			else {
				tmp += c;
			}
			if (i == original_str.size() - 1) rtn.push_back(tmp);
		}
		return rtn;
	}
	static std::string rand_str() {
		srand((unsigned int)time(NULL));
		char randstr[21];
		int len = 20;
		int i;
		for (i = 0; i < len; ++i) {
			switch ((rand() % 5)) {
			case 1:
				randstr[i] = 'A' + rand() % 25;
				break;
			case 2:
				randstr[i] = 'a' + rand() % 25;
				break;
			default:
				randstr[i] = '0' + rand() % 10;
				break;
			}
		}
		randstr[i] = '\0';
		return randstr;
	}
	static bool isNum(std::string str) {
		std::stringstream sin(str);
		double d;
		char c;
		if (!(sin >> d)) return false;
		if (sin >> c) return false;
		return true;
	}
	static bool copyFile(std::string sourcePath, std::string destPath) {
		try {
			std::ifstream source(sourcePath, std::ios::binary);
			std::ofstream dest(destPath, std::ios::binary);
			dest << source.rdbuf();
			source.close();
			dest.close();
			return true;
		}
		catch (std::exception) {
			return false;
		}
	}
	static std::string getCountryNameByCode(std::string code) {
		std::string countryName = code;
		while (true) {
			if (code == "US") {
				countryName = "United State";
				break;
			}
			if (code == "GB") {
				countryName = "United Kingdom";
				break;
			}
			if (code == "DE") {
				countryName = "Germany";
				break;
			}
			if (code == "CA") {
				countryName = "Canada";
				break;
			}
			if (code == "JP") {
				countryName = "Japan";
				break;
			}
			if (code == "ES") {
				countryName = "Spain";
				break;
			}
			if (code == "FR") {
				countryName = "France";
				break;
			}
			if (code == "IT") {
				countryName = "Italy";
				break;
			}
			if (code == "CN") {
				countryName = "China";
				break;
			}
			if (code == "AT") {
				countryName = "Austria";
				break;
			}
			if (code == "AU") {
				countryName = "Australian";
				break;
			}
			if (code == "CH") {
				countryName = "Switzerland";
				break;
			}
			if (code == "EG") {
				countryName = "Egypt";
				break;
			}
			if (code == "FI") {
				countryName = "Finland";
				break;
			}
			if (code == "IN") {
				countryName = "India";
				break;
			}
			if (code == "IE") {
				countryName = "Ireland";
				break;
			}
			if (code == "ID") {
				countryName = "Indonesia";
				break;
			}
			if (code == "KH") {
				countryName = "Cambodia";
				break;
			}
			if (code == "KR") {
				countryName = "Korea";
				break;
			}
			if (code == "MX") {
				countryName = "Mexico";
				break;
			}
			if (code == "NL") {
				countryName = "Holland";
				break;
			}
			if (code == "NO") {
				countryName = "Norway";
				break;
			}
			if (code == "MM") {
				countryName = "Burma";
				break;
			}
			if (code == "PH") {
				countryName = "Philippines";
				break;
			}
			if (code == "BE") {
				countryName = "Belgium";
				break;
			}
			if (code == "GY") {
				countryName = "Guyana";
				break;
			}
			if (code == "SE") {
				countryName = "Sweden";
				break;
			}
			if (code == "PY") {
				countryName = "Paraguay";
				break;
			}
			if (code == "UA") {
				countryName = "Ukraine";
				break;
			}
			if (code == "PE") {
				countryName = "Peru";
				break;
			}
			if (code == "AE") {
				countryName = "United Arab Emirates";
				break;
			}
			if (code == "VN") {
				countryName = "Vietnam";
				break;
			}
			if (code == "SO") {
				countryName = "Somalia";
				break;
			}
			if (code == "LA") {
				countryName = "Laos";
				break;
			}
			if (code == "KW") {
				countryName = "Kuwait";
				break;
			}
			if (code == "MY") {
				countryName = "Malaysia";
				break;
			}
			if (code == "DK") {
				countryName = "Denmark";
				break;
			}
			if (code == "PK") {
				countryName = "Pakistan";
				break;
			}
			if (code == "TH") {
				countryName = "Thailand";
				break;
			}
			if (code == "HU") {
				countryName = "HUNGARY";
				break;
			}
			if (code == "LV") {
				countryName = "Latvia";
				break;
			}
			if (code == "RU") {
				countryName = "Russia";
				break;
			}
			if (code == "CZ") {
				countryName = "Czech Republic";
				break;
			}
			if (code == "PL") {
				countryName = "Poland";
				break;
			}
			if (code == "BY") {
				countryName = "Belarus";
				break;
			}
			if (code == "LT") {
				countryName = "Lithuania";
				break;
			}
			if (code == "LU") {
				countryName = "Luxembourg";
				break;
			}
			if (code == "AD") {
				countryName = "Andorra";
				break;
			}
			if (code == "AG") {
				countryName = "Antigua and Barbuda";
				break;
			}
			if (code == "RO") {
				countryName = "Romania";
				break;
			}
			if (code == "AM") {
				countryName = "Armenia";
				break;
			}
			if (code == "CV") {
				countryName = "Cape Verde";
				break;
			}
			if (code == "DZ") {
				countryName = "Algeria";
				break;
			}
			if (code == "RT") {
				countryName = "Rota (Northern Mariana Islands)";
				break;
			}
			if (code == "TL") {
				countryName = "Timor-Leste";
				break;
			}
			if (code == "MU") {
				countryName = "Mauritius";
				break;
			}
			if (code == "ZM") {
				countryName = "Zambia";
				break;
			}
			if (code == "BL") {
				countryName = "St. Barthelemy";
				break;
			}
			if (code == "SX") {
				countryName = "St. Maarten, St. Martin";
				break;
			}
			if (code == "BH") {
				countryName = "Bahrain";
				break;
			}
			if (code == "GM") {
				countryName = "Gambia";
				break;
			}
			if (code == "MV") {
				countryName = "Maldives";
				break;
			}
			if (code == "CK") {
				countryName = "Cook Islands";
				break;
			}
			if (code == "RE") {
				countryName = "Reunion Island";
				break;
			}
			if (code == "IQ") {
				countryName = "Iraq";
				break;
			}
			if (code == "GD") {
				countryName = "Grenada";
				break;
			}
			if (code == "FM") {
				countryName = "Micronesia, Federated States of";
				break;
			}
			if (code == "VE") {
				countryName = "Venezuela";
				break;
			}
			if (code == "NC") {
				countryName = "New Caledonia";
				break;
			}
			if (code == "NP") {
				countryName = "Nepal";
				break;
			}
			if (code == "MD") {
				countryName = "Moldova";
				break;
			}
			if (code == "VA") {
				countryName = "Vatican City (Italy)";
				break;
			}
			if (code == "GP") {
				countryName = "Guadeloupe";
				break;
			}
			if (code == "GU") {
				countryName = "Guam";
				break;
			}
			if (code == "NA") {
				countryName = "Namibia";
				break;
			}
			if (code == "QA") {
				countryName = "Qatar";
				break;
			}
			if (code == "YT") {
				countryName = "Mayotte";
				break;
			}
			if (code == "ME") {
				countryName = "Montenegro";
				break;
			}
			if (code == "BT") {
				countryName = "Bhutan";
				break;
			}
			if (code == "UZ") {
				countryName = "Uzbekistan";
				break;
			}
			if (code == "WF") {
				countryName = "Wallis & Futuna Islands";
				break;
			}
			if (code == "JO") {
				countryName = "Jordan";
				break;
			}
			if (code == "MK") {
				countryName = "Macedonia (FYROM)";
				break;
			}
			if (code == "BF") {
				countryName = "Burkina Faso";
				break;
			}
			if (code == "NE") {
				countryName = "Niger";
				break;
			}
			if (code == "WL") {
				countryName = "Wales (United Kingdom)";
				break;
			}
			if (code == "GE") {
				countryName = "Georgia";
				break;
			}
			if (code == "IS") {
				countryName = "Iceland";
				break;
			}
			if (code == "ER") {
				countryName = "Eritrea";
				break;
			}
			if (code == "KM") {
				countryName = "Comoros";
				break;
			}
			if (code == "SN") {
				countryName = "Senegal";
				break;
			}
			if (code == "EE") {
				countryName = "Estonia";
				break;
			}
			if (code == "MO") {
				countryName = "Macau";
				break;
			}
			if (code == "SI") {
				countryName = "Slovenia";
				break;
			}
			if (code == "UG") {
				countryName = "Uganda";
				break;
			}
			if (code == "ZW") {
				countryName = "Zimbabwe";
				break;
			}
			if (code == "TD") {
				countryName = "Chad";
				break;
			}
			if (code == "GL") {
				countryName = "Greenland";
				break;
			}
			if (code == "AF") {
				countryName = "Afghanistan";
				break;
			}
			if (code == "CD") {
				countryName = "Congo, Democratic Republic of";
				break;
			}
			if (code == "MN") {
				countryName = "Mongolia";
				break;
			}
			if (code == "MR") {
				countryName = "Mauritania";
				break;
			}
			if (code == "CO") {
				countryName = "Colombia";
				break;
			}
			if (code == "TT") {
				countryName = "Trinidad & Tobago";
				break;
			}
			if (code == "SF") {
				countryName = "Scotland (United Kingdom)";
				break;
			}
			if (code == "CF") {
				countryName = "Central African Republic";
				break;
			}
			if (code == "GI") {
				countryName = "Gibraltar";
				break;
			}
			if (code == "KY") {
				countryName = "Cayman Islands";
				break;
			}
			if (code == "NI") {
				countryName = "Nicaragua";
				break;
			}
			if (code == "TG") {
				countryName = "Togo";
				break;
			}
			if (code == "GQ") {
				countryName = "Equatorial Guinea";
				break;
			}
			if (code == "KN") {
				countryName = "St. Kitts and Nevis";
				break;
			}
			if (code == "ZZ") {
				countryName = "Tortola (British Virgin Islands)";
				break;
			}
			if (code == "TZ") {
				countryName = "Tanzania, United Republic of";
				break;
			}
			if (code == "KE") {
				countryName = "Kenya";
				break;
			}
			if (code == "MH") {
				countryName = "Marshall Islands";
				break;
			}
			if (code == "KZ") {
				countryName = "Kazakhstan";
				break;
			}
			if (code == "SA") {
				countryName = "Saudi Arabia";
				break;
			}
			if (code == "BN") {
				countryName = "Brunei Darussalam";
				break;
			}
			if (code == "SY") {
				countryName = "Syrian Arab Republic";
				break;
			}
			if (code == "HN") {
				countryName = "Honduras";
				break;
			}
			if (code == "PG") {
				countryName = "Papua New Guinea";
				break;
			}
			if (code == "HT") {
				countryName = "Haiti";
				break;
			}
			if (code == "SM") {
				countryName = "San Marino";
				break;
			}
			if (code == "MW") {
				countryName = "Malawi";
				break;
			}
			if (code == "CM") {
				countryName = "Cameroon";
				break;
			}
			if (code == "MG") {
				countryName = "Madagascar";
				break;
			}
			if (code == "PT") {
				countryName = "Portugal";
				break;
			}
			if (code == "CR") {
				countryName = "Costa Rica";
				break;
			}
			if (code == "BM") {
				countryName = "Bermuda";
				break;
			}
			if (code == "SR") {
				countryName = "Suriname";
				break;
			}
			if (code == "SB") {
				countryName = "Solomon Islands";
				break;
			}
			if (code == "TA") {
				countryName = "Tahiti (French Polynesia)";
				break;
			}
			if (code == "CW") {
				countryName = "Curacao";
				break;
			}
			if (code == "TW") {
				countryName = "Taiwan";
				break;
			}
			if (code == "SL") {
				countryName = "Sierra Leone";
				break;
			}
			if (code == "TU") {
				countryName = "Truk (Micronesia, Federated States of)";
				break;
			}
			if (code == "UI") {
				countryName = "Union Islands (St. Vincent & the Grenadines)";
				break;
			}
			if (code == "VL") {
				countryName = "St. Thomas (U.S. Virgin Islands)";
				break;
			}
			if (code == "NF") {
				countryName = "Norfolk Island (Australia)";
				break;
			}
			if (code == "GT") {
				countryName = "Guatemala";
				break;
			}
			if (code == "BG") {
				countryName = "Bulgaria";
				break;
			}
			if (code == "PO") {
				countryName = "Ponape (Micronesia, Federated States of)";
				break;
			}
			if (code == "AW") {
				countryName = "Aruba";
				break;
			}
			if (code == "EC") {
				countryName = "Ecuador";
				break;
			}
			if (code == "RS") {
				countryName = "Serbia";
				break;
			}
			if (code == "BS") {
				countryName = "Bahamas";
				break;
			}
			if (code == "BO") {
				countryName = "Bolivia";
				break;
			}
			if (code == "PW") {
				countryName = "Palau";
				break;
			}
			if (code == "KG") {
				countryName = "Kirghizia (Kyrgyzstan)";
				break;
			}
			if (code == "TC") {
				countryName = "Turks & Caicos Islands";
				break;
			}
			if (code == "LC") {
				countryName = "St. Lucia";
				break;
			}
			if (code == "AZ") {
				countryName = "Azerbaijan";
				break;
			}
			if (code == "MT") {
				countryName = "Malta";
				break;
			}
			if (code == "BI") {
				countryName = "Burundi";
				break;
			}
			if (code == "AO") {
				countryName = "Angola";
				break;
			}
			if (code == "CY") {
				countryName = "Cyprus";
				break;
			}
			if (code == "BR") {
				countryName = "Brazil";
				break;
			}
			if (code == "WS") {
				countryName = "Samoa Western";
				break;
			}
			if (code == "AX") {
				countryName = "Aland Island (Finland)";
				break;
			}
			if (code == "LR") {
				countryName = "Liberia";
				break;
			}
			if (code == "FO") {
				countryName = "Faroe Islands";
				break;
			}
			if (code == "UV") {
				countryName = "St. John (U.S. Virgin Islands)";
				break;
			}
			if (code == "SG") {
				countryName = "Singapore";
				break;
			}
			if (code == "NG") {
				countryName = "Nigeria";
				break;
			}
			if (code == "PA") {
				countryName = "Panama";
				break;
			}
			if (code == "DJ") {
				countryName = "Djibouti";
				break;
			}
			if (code == "SZ") {
				countryName = "Swaziland";
				break;
			}
			if (code == "DO") {
				countryName = "Dominican Republic";
				break;
			}
			if (code == "KO") {
				countryName = "Kosrae (Micronesia, Federated States of)";
				break;
			}
			if (code == "MZ") {
				countryName = "Mozambique";
				break;
			}
			if (code == "AS") {
				countryName = "American Samoa";
				break;
			}
			if (code == "UY") {
				countryName = "Uruguay";
				break;
			}
			if (code == "KT") {
				countryName = "republic of ivory coast";
				break;
			}
			if (code == "AL") {
				countryName = "Albania";
				break;
			}
			if (code == "CG") {
				countryName = "Congo";
				break;
			}
			if (code == "AI") {
				countryName = "Anguilla";
				break;
			}
			if (code == "RW") {
				countryName = "Rwanda";
				break;
			}
			if (code == "GR") {
				countryName = "Greece";
				break;
			}
			if (code == "BW") {
				countryName = "Botswana";
				break;
			}
			if (code == "HR") {
				countryName = "Croatia";
				break;
			}
			if (code == "SC") {
				countryName = "Seychelles";
				break;
			}
			if (code == "NZ") {
				countryName = "New Zealand";
				break;
			}
			if (code == "PF") {
				countryName = "French Polynesia";
				break;
			}
			if (code == "ML") {
				countryName = "Mali";
				break;
			}
			if (code == "VU") {
				countryName = "Vanuatu";
				break;
			}
			if (code == "MP") {
				countryName = "Northern Mariana Islands";
				break;
			}
			if (code == "TR") {
				countryName = "Turkey";
				break;
			}
			if (code == "BA") {
				countryName = "Bosnia and Herzegovina";
				break;
			}
			if (code == "LY") {
				countryName = "Libyan Arab Jamahiriya";
				break;
			}
			if (code == "SV") {
				countryName = "El Salvador";
				break;
			}
			if (code == "TN") {
				countryName = "Tunisia";
				break;
			}
			if (code == "SW") {
				countryName = "St. Christopher (St. Kitts)";
				break;
			}
			if (code == "AR") {
				countryName = "Argentina";
				break;
			}
			if (code == "YE") {
				countryName = "Yemen, Republic of";
				break;
			}
			if (code == "TJ") {
				countryName = "Tajikistan";
				break;
			}
			if (code == "GA") {
				countryName = "Gabon";
				break;
			}
			if (code == "FJ") {
				countryName = "Fiji";
				break;
			}
			if (code == "GN") {
				countryName = "Guinea";
				break;
			}
			if (code == "GH") {
				countryName = "Ghana";
				break;
			}
			if (code == "BD") {
				countryName = "Bangladesh";
				break;
			}
			if (code == "TV") {
				countryName = "Tuvalu";
				break;
			}
			if (code == "YA") {
				countryName = "Yap (Micronesia, Federated States of)";
				break;
			}
			if (code == "LI") {
				countryName = "Liechtenstein";
				break;
			}
			if (code == "SP") {
				countryName = "Saipan (Northern Mariana Islands)";
				break;
			}
			if (code == "LB") {
				countryName = "Lebanon";
				break;
			}
			if (code == "MQ") {
				countryName = "Martinique";
				break;
			}
			if (code == "ZA") {
				countryName = "South Africa";
				break;
			}
			if (code == "BJ") {
				countryName = "Benin";
				break;
			}
			if (code == "OM") {
				countryName = "Oman";
				break;
			}
			if (code == "TO") {
				countryName = "Tonga";
				break;
			}
			if (code == "SK") {
				countryName = "Slovakia";
				break;
			}
			if (code == "BB") {
				countryName = "Barbados";
				break;
			}
			if (code == "MA") {
				countryName = "Morocco";
				break;
			}
			if (code == "IL") {
				countryName = "Israel";
				break;
			}
			if (code == "LK") {
				countryName = "Sri Lanka";
				break;
			}
			if (code == "BQ") {
				countryName = "Bonaire, St. Eustatius, Saba";
				break;
			}
			if (code == "PR") {
				countryName = "Puerto Rico";
				break;
			}
			if (code == "MS") {
				countryName = "Montserrat";
				break;
			}
			if (code == "TM") {
				countryName = "Turkmenistan";
				break;
			}
			if (code == "KI") {
				countryName = "Kiribati";
				break;
			}
			if (code == "GW") {
				countryName = "Guinea-Bissau";
				break;
			}
			if (code == "IC") {
				countryName = "Canary Islands (Spain)";
				break;
			}
			if (code == "GF") {
				countryName = "French Guiana";
				break;
			}
			if (code == "LS") {
				countryName = "Lesotho";
				break;
			}
			if (code == "JE") {
				countryName = "Jersey (Channel Islands)";
				break;
			}
			if (code == "DM") {
				countryName = "Dominica";
				break;
			}
			if (code == "ET") {
				countryName = "Ethiopia";
				break;
			}
			if (code == "BZ") {
				countryName = "Belize";
				break;
			}
			if (code == "GG") {
				countryName = "Guernsey (Channel Islands)";
				break;
			}
			if (code == "JM") {
				countryName = "Jamaica";
				break;
			}
			if (code == "NB") {
				countryName = "Northern Ireland (United Kingdom)";
				break;
			}
			if (code == "CL") {
				countryName = "Chile";
				break;
			}
			if (code == "SD") {
				countryName = "Sudan";
				break;
			}
			if (code == "CI") {
				countryName = "ivory coast";
				break;
			}
			if (code == "AN") {
				countryName = "NETHERLANDS ANTILLES";
				break;
			}
			if (code == "CC") {
				countryName = "COCOS(KEELING) ISLANDS";
				break;
			}
			if (code == "CU") {
				countryName = "CUBA";
				break;
			}
			if (code == "MC") {
				countryName = "Monaco";
				break;
			}
			if (code == "VC") {
				countryName = "St. Vincent & the Grenadines";
				break;
			}
			if (code == "HK") {
				countryName = "HONGKONG";
				break;
			}
			break;
		}
		return countryName;
	};
	//容器vector中元素的去重
	static std::vector<int> unique_element_in_vector(std::vector<int> v) {
		std::vector<int>::iterator vector_iterator;
		sort(v.begin(), v.end());
		vector_iterator = unique(v.begin(), v.end());
		if (vector_iterator != v.end()) {
			v.erase(vector_iterator, v.end());
		}
		return v;
	}
	//两个vector求交集
	static std::vector<int> vectors_intersection(std::vector<int> v1, std::vector<int> v2) {
		std::vector<int> v;
		sort(v1.begin(), v1.end());
		sort(v2.begin(), v2.end());
		set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v)); //求交集
		return v;
	}
	//两个vector求并集
	static std::vector<int> vectors_set_union(std::vector<int> v1, std::vector<int> v2) {
		std::vector<int> v;
		sort(v1.begin(), v1.end());
		sort(v2.begin(), v2.end());
		set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v)); //求交集
		return v;
	}
	//判断vector的某一元素是否存在
	static bool is_element_in_vector(std::vector<std::string> v, std::string element) {
		std::vector<std::string>::iterator it;
		it = std::find(v.begin(), v.end(), element);
		if (it != v.end()) {
			return true;
		}
		else {
			return false;
		}
	}
	static void fakestrptime(const char* timeStr, const char* format, tm* ttm) {
		//see also http://stackoverflow.com/questions/9575131/why-is-mktime-changing-the-year-day-of-my-tm-struct

		tm tm_;
		int year, month, day, hour, minute, second;
		sscanf(timeStr, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
		tm_.tm_year = year - 1900;
		tm_.tm_mon = month - 1;
		tm_.tm_mday = day;
		tm_.tm_hour = hour;
		tm_.tm_min = minute;
		tm_.tm_sec = second;
		tm_.tm_isdst = 0;

		time_t t_ = mktime(&tm_); //已经减了8个时区
		ttm->tm_sec = t_; //秒时间
	}    
	inline std::string escape(const std::string& str, const bool escape_comma = true) {

		std::string res = str;
		string_replace(res, "&", "&amp;");
		string_replace(res, "[", "&#91;");
		string_replace(res, "]", "&#93;");
		if (escape_comma) string_replace(res, ",", "&#44;");
		return res;
	}
	// 对字符串做 CQ 码去转义
	inline std::string unescape(const std::string& str) {
		std::string res = str;
		string_replace(res, "&#44;", ",");
		string_replace(res, "&#91;", "[");
		string_replace(res, "&#93;", "]");
		string_replace(res, "&amp;", "&");
		return res;
	}
	//这功能有缺陷
	string GetMiddleText(string regularStr, string frontStr, string behindStr) {
		string str;
		try {
			str = regularStr.substr(regularStr.find(frontStr) + 1);
			str = str.substr(0, str.find(behindStr));
			return str;
		}
		catch (std::exception) {
			return "";
		}
	}
};


