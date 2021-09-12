// 
// Utils.h
// Chanify
// 
// Created by WizJin.
//

#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

#include "Chanify.h"
#include <time.h>
#include <string>
#include <memory>
#pragma comment(lib, "version.lib")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CUtils
{
private:
	HINSTANCE		m_hInstance;
	std::wstring	m_version;
	std::wstring	m_szModulePath;
public:
	explicit CUtils(HINSTANCE hInstance) {
		srand((unsigned)time(NULL));
		m_hInstance = hInstance;
		m_szModulePath = _wpgmptr;
	}

	virtual ~CUtils() {

	}

	inline HINSTANCE GetInstance(void) const { return m_hInstance; }

	inline std::wstring GetVersion(void) {
		if (m_version.empty()) {
			DWORD ignore;
			DWORD vinfoSize = GetFileVersionInfoSizeW(m_szModulePath.c_str(), &ignore);
			if (vinfoSize > 0) {
				void* vinfo = malloc(vinfoSize);
				if (vinfo != nullptr) {
					if (GetFileVersionInfoW(m_szModulePath.c_str(), 0, vinfoSize, vinfo) != 0) {
						struct Translation {
							int16_t Language;
							int16_t Codepage;
						} *translations = NULL;
						uint32_t translationLength = 0;
						if (VerQueryValueW(vinfo, L"\\VarFileInfo\\Translation", (LPVOID*)&translations, &translationLength)) {
							for (size_t i = 0; i < (translationLength / sizeof(struct Translation)); ++i) {
								WCHAR key[MAX_PATH] = { 0 };
								swprintf_s(key, _countof(key), _T("\\StringFileInfo\\%04x%04x\\FileVersion"), translations[i].Language, translations[i].Codepage);
								const WCHAR* ffInfo = nullptr;
								uint32_t ffiLength = 0;
								if (VerQueryValueW(vinfo, key, (LPVOID*)&ffInfo, &ffiLength)) {
									uint32_t version[4] = { 0 };
									uint32_t* v = version;
									swscanf_s(ffInfo, _T("%d.%d.%d.%d"), &v[0], &v[1], &v[2], &v[3]);
									WCHAR verstr[256] = { 0 };
									swprintf_s(verstr, _countof(verstr), L"v%d.%d.%d", v[0], v[1], v[2]);
									m_version = verstr;
									break;
								}
							}
						}
					}
					free(vinfo);
				}
			}
		}
		return m_version;
	}

	inline std::wstring GetAppFilePath(const std::wstring& name) {
		std::wstring path = name;
		auto p = m_szModulePath.rfind('\\');
		if (p != std::wstring::npos) {
			path = m_szModulePath.substr(0, p + 1) + name;
		}
		return path;
	}

	static std::shared_ptr<CUtils>& Shared(void) {
		static std::shared_ptr<CUtils> instance;
		return instance;
	}

	static void Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance) {
		auto ptr = std::make_shared<CUtils>(hInstance);
		Shared().swap(ptr);
	}

public:
	inline static std::wstring str2lower(const std::wstring& str) {
		std::wstring res;
		for (auto c : str) {
			res += std::tolower(c);
		}
		return res;
	}

	inline static std::wstring A2W(const std::string& str) {
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring res(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &res[0], size_needed);
		return res;
	}
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __UTILS_H__
