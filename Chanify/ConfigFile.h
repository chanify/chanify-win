// 
// ConfigFile.h
// Chanify
// 
// Created by WizJin.
//

#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#pragma once

#include "Chanify.h"
#include "Utils.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CConfigFile
{
private:
	inline static std::wstring readString(const std::wstring path, const std::wstring& key) {
		std::wstring res;
		WCHAR data[4096] = { 0 };
		GetPrivateProfileStringW(L"client", key.c_str(), NULL, data, _countof(data), path.c_str());
		if (data[0] != 0) {
			res = data;
		}
		return res;
	}
	inline static UINT readInteger(const std::wstring path, const std::wstring& key) {
		return GetPrivateProfileIntW(L"client", key.c_str(), 0, path.c_str());
	}
public:
	explicit CConfigFile() {
		std::wstring path = CUtils::Shared()->GetAppFilePath(L"Chanify.ini");
		wprintf(L"name: %s\n", readString(path, L"name").c_str());
		wprintf(L"endpoint: %s\n", readString(path, L"endpoint").c_str());
		wprintf(L"sound: %d\n", readInteger(path, L"sound"));
		wprintf(L"token: %s\n", readString(path, L"token").c_str());
	}
};


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __CONFIGFILE_H__
