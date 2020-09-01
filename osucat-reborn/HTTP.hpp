#pragma once
#ifndef OSUCAT_HTTP_HPP
#define OSUCAT_HTTP_HPP

#define PP_PLUS_URL "https://syrin.me/pp+/u/"
#define PP_PLUS_API "https://syrin.me/pp+/api/user/"
#define HTTPHEADER L"osucat/<0.3.1> (Windows NT 10.0; Win32) WinHttp/5.1"

using namespace std;
using namespace osucat;
using namespace nlohmann;

class NetConnection {
public:
	static std::string HttpsGet(const std::string& url) {
		std::string ret("");

		std::wstring wUrl = osucat::utils::s2ws(url);
		URL_COMPONENTS urlComp;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.size(), 0, &urlComp))
			throw osucat::NetWork_Exception("[Network] Error in WinHttpCrackUrl", GetLastError());

		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		bool bResults = false;
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
		hSession = WinHttpOpen(
			HTTPHEADER, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			std::wstring hostname(urlComp.lpszHostName);
			hostname = hostname.substr(0, hostname.find_first_of(L'/'));
			hostname = hostname.substr(0, hostname.find_first_of(L':'));
			hConnect = WinHttpConnect(hSession, hostname.c_str(), urlComp.nPort, 0);
		}

		if (hConnect) {
			hRequest = WinHttpOpenRequest(hConnect,
				L"GET",
				urlComp.lpszUrlPath,
				NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				WINHTTP_FLAG_SECURE);
		}
		if (hRequest) {
			bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		}
		if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
		if (bResults) {
			do {
				LPSTR inBuf;
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpQueryDataAvailable",
						GetLastError());
				inBuf = new char[dwSize + 1];
				if (!inBuf) throw std::exception("[Network] Out of Memory!", 1001);
				ZeroMemory(inBuf, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)inBuf, dwSize, &dwDownloaded))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpReadData!", GetLastError());
				ret += inBuf;
			} while (dwSize > 0);
		}
		if (!bResults) throw osucat::NetWork_Exception("[Network] Error has occurred!", GetLastError());

		if (hSession) WinHttpCloseHandle(hSession);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hRequest) WinHttpCloseHandle(hRequest);
		return ret;
	}

	static void DownloadFile(const std::string& url, const std::string& path) {
		std::ofstream file(path, std::ios::binary);
		std::wstring wUrl = osucat::utils::s2ws(url);
		URL_COMPONENTS urlComp;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.size(), 0, &urlComp))
			throw osucat::NetWork_Exception("[Network] Error in WinHttpCrackUrl", GetLastError());

		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		bool bResults = false;
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
		hSession = WinHttpOpen(L"Sayobot with WinHttp API",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			0);
		if (hSession) {
			std::wstring hostname(urlComp.lpszHostName);
			hostname = hostname.substr(0, hostname.find_first_of(L'/'));
			hostname = hostname.substr(0, hostname.find_first_of(L':'));
			hConnect = WinHttpConnect(hSession, hostname.c_str(), urlComp.nPort, 0);
		}

		if (hConnect) {
			hRequest = WinHttpOpenRequest(hConnect,
				L"GET",
				urlComp.lpszUrlPath,
				NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				WINHTTP_FLAG_SECURE);
		}
		if (hRequest) {
			bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		}
		if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
		if (bResults) {
			do {
				LPSTR inBuf;
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpQueryDataAvailable",
						GetLastError());
				inBuf = new char[dwSize + 1];
				if (!inBuf) throw std::exception("[Network] Out of Memory!", 1001);
				ZeroMemory(inBuf, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)inBuf, dwSize, &dwDownloaded))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpReadData!", GetLastError());
				file.write(inBuf, dwDownloaded);
			} while (dwSize > 0);
		}
		if (!bResults) throw osucat::NetWork_Exception("[Network] Error has occurred!", GetLastError());

		if (hSession) WinHttpCloseHandle(hSession);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hRequest) WinHttpCloseHandle(hRequest);
		file.close();
	}

	static std::string HttpsPost(const std::string& url, const json& Data) {
#ifdef WIN32
		std::string ret("");

		std::string data_str = Data.dump();
		std::wstring wUrl = osucat::utils::s2ws(url);
		URL_COMPONENTS urlComp;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.size(), 0, &urlComp))
			throw osucat::NetWork_Exception("[Network] Error in WinHttpCrackUrl", GetLastError());

		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		bool bResults = false;
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
		hSession = WinHttpOpen(
			HTTPHEADER, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			std::wstring hostname(urlComp.lpszHostName);
			hostname = hostname.substr(0, hostname.find_first_of(L'/'));
			hostname = hostname.substr(0, hostname.find_first_of(L':'));
			hConnect = WinHttpConnect(hSession, hostname.c_str(), urlComp.nPort, 0);
		}

		if (hConnect) {
			hRequest = WinHttpOpenRequest(hConnect,
				L"POST",
				urlComp.lpszUrlPath,
				L"HTTPS/1.1",
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				WINHTTP_FLAG_SECURE);
		}

		if (hRequest) {
			WinHttpAddRequestHeaders(hRequest,
				L"Content-Type: application/json;charset=utf-8",
				-1L,
				WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
			bResults = WinHttpSendRequest(hRequest,
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				(LPVOID)data_str.c_str(),
				data_str.length(),
				data_str.length(),
				0);
		}

		if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
		if (bResults) {
			do {
				LPSTR inBuf;
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpQueryDataAvailable",
						GetLastError());
				inBuf = new char[dwSize + 1];
				if (!inBuf) throw osucat::NetWork_Exception("[Network] Out of Memory!", 1001);
				ZeroMemory(inBuf, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)inBuf, dwSize, &dwDownloaded))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpReadData!", GetLastError());
				ret += inBuf;
			} while (dwSize > 0);
		}
		if (!bResults) throw osucat::NetWork_Exception("[Network] Error has occurred!", GetLastError());

		if (hSession) WinHttpCloseHandle(hSession);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hRequest) WinHttpCloseHandle(hRequest);
		return ret;

#endif
	}

	static std::string HttpGet(const std::string& url) {
		std::string ret("");

		std::wstring wUrl = osucat::utils::s2ws(url);
		URL_COMPONENTS urlComp;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.size(), 0, &urlComp))
			throw osucat::NetWork_Exception("[Network] Error in WinHttpCrackUrl", GetLastError());

		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		bool bResults = false;
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
		hSession = WinHttpOpen(
			HTTPHEADER, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			std::wstring hostname(urlComp.lpszHostName);
			hostname = hostname.substr(0, hostname.find_first_of(L'/'));
			hostname = hostname.substr(0, hostname.find_first_of(L':'));
			hConnect = WinHttpConnect(hSession, hostname.c_str(), urlComp.nPort, 0);
		}

		if (hConnect) {
			hRequest = WinHttpOpenRequest(hConnect,
				L"GET",
				urlComp.lpszUrlPath,
				NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				0);
		}
		if (hRequest) {
			bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		}
		if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
		if (bResults) {
			do {
				LPSTR inBuf;
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpQueryDataAvailable",
						GetLastError());
				inBuf = new char[dwSize + 1];
				if (!inBuf) throw std::exception("[Network] Out of Memory!", 1001);
				ZeroMemory(inBuf, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)inBuf, dwSize, &dwDownloaded))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpReadData!", GetLastError());
				ret += inBuf;
			} while (dwSize > 0);
		}
		if (!bResults) throw osucat::NetWork_Exception("[Network] Error has occurred!", GetLastError());

		if (hSession) WinHttpCloseHandle(hSession);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hRequest) WinHttpCloseHandle(hRequest);
		return ret;
	}

	static std::string HttpPost(const std::string& url, const json& Data) {
#ifdef WIN32
		std::string ret("");

		std::string data_str = Data.dump();
		std::wstring wUrl = osucat::utils::s2ws(url);
		URL_COMPONENTS urlComp;
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.size(), 0, &urlComp))
			throw osucat::NetWork_Exception("[Network] Error in WinHttpCrackUrl", GetLastError());

		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		bool bResults = false;
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
		hSession = WinHttpOpen(
			HTTPHEADER, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			std::wstring hostname(urlComp.lpszHostName);
			hostname = hostname.substr(0, hostname.find_first_of(L'/'));
			hostname = hostname.substr(0, hostname.find_first_of(L':'));
			hConnect = WinHttpConnect(hSession, hostname.c_str(), urlComp.nPort, 0);
		}

		if (hConnect) {
			hRequest = WinHttpOpenRequest(hConnect,
				L"POST",
				urlComp.lpszUrlPath,
				L"HTTP/1.1",
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				0);
		}

		if (hRequest) {
			WinHttpAddRequestHeaders(hRequest,
				L"Content-Type: application/json;charset=utf-8",
				-1L,
				WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
			bResults = WinHttpSendRequest(hRequest,
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				(LPVOID)data_str.c_str(),
				data_str.length(),
				data_str.length(),
				0);
		}

		if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
		if (bResults) {
			do {
				LPSTR inBuf;
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpQueryDataAvailable",
						GetLastError());
				inBuf = new char[dwSize + 1];
				if (!inBuf) throw osucat::NetWork_Exception("[Network] Out of Memory!", 1001);
				ZeroMemory(inBuf, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)inBuf, dwSize, &dwDownloaded))
					throw osucat::NetWork_Exception("[Network] Error in WinHttpReadData!", GetLastError());
				ret += inBuf;
			} while (dwSize > 0);
		}
		if (!bResults) throw osucat::NetWork_Exception("[Network] Error has occurred!", GetLastError());

		if (hSession) WinHttpCloseHandle(hSession);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hRequest) WinHttpCloseHandle(hRequest);
		return ret;

#endif
	}

	static std::string GetBannerLink(int64_t uid) {
		string source, temp, cover_url;
		try {
			source = NetConnection::HttpsGet("https://osu.ppy.sh/users/" + to_string(uid));
			temp = source.substr(source.find(R"(<script id="json-user" type="application/json">)") + 50);
			temp = temp.substr(0, temp.find_last_of("}") + 1);
			json j;
			j = json::parse(temp);
			cover_url = j["cover_url"].get<string>();
			// https://osu.ppy.sh/images/headers/profile-covers/c1.jpg
		}
		catch (osucat::NetWork_Exception) {
			cover_url =
				"https://osu.ppy.sh/images/headers/profile-covers/c" + to_string(osucat::utils::randomNum(1, 8)) + ".jpg";
		}
		catch (std::exception) {
			cover_url =
				"https://osu.ppy.sh/images/headers/profile-covers/c" + to_string(osucat::utils::randomNum(1, 8)) + ".jpg";
		}
		return cover_url;
	}

	static vector<long> getUserPlusData(int64_t uid) {
		char url[512];
		std::string response;
		vector<long> ppd;
		sprintf_s(url, PP_PLUS_API "%I64d", uid);
		response = HttpsGet(url);
		if (!response.empty()) {
			json data = json::parse(response)["user_data"];
			// cout << data.dump() << endl;
			// cout <<
			// "<><><><><><><><><><><><><>\n<><><><><><><><><><><><><>\n<><><><><><><><><><><><><>\n<><><><><><><><><><><><><>\n<><><><><><><><><><><><><>\n<><><><><><><><><><><><><>"
			// << endl; cout << "aim(jump) : "<<data["JumpAimTotal"].get<double>() << endl; cout << "aim(flow) : " <<
			// data["FlowAimTotal"].get<double>() << endl; cout << "precision : " <<
			// data["PrecisionTotal"].get<double>()
			// << endl; cout << "speed : " << data["SpeedTotal"].get<double>() << endl; cout << "stamina : " <<
			// data["StaminaTotal"].get<double>() << endl; cout << "accuracy : " << data["AccuracyTotal"].get<double>()
			// << endl;
			ppd.push_back(data["AccuracyTotal"].get<double>());
			ppd.push_back(data["FlowAimTotal"].get<double>());
			ppd.push_back(data["JumpAimTotal"].get<double>());
			ppd.push_back(data["PrecisionTotal"].get<double>());
			ppd.push_back(data["SpeedTotal"].get<double>());
			ppd.push_back(data["StaminaTotal"].get<double>());
		}
		//??
		return ppd;
	}
};
#endif