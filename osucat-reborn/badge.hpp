#pragma once
#ifndef OSUCAT_BADGE_HPP
#define OSUCAT_BADGE_HPP
#define BADGENOTFOUND 65536

namespace osucat::badgeSystem {
	class main {
	public:
		

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