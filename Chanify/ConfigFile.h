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
	std::wstring	m_name;
	std::wstring	m_endpoint;
	std::wstring	m_token;
	std::wstring	m_interruptionLevel;
	UINT			m_sound;

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
		m_name = readString(path, L"name");
		m_endpoint = readString(path, L"endpoint");
		m_token = readString(path, L"token");
		m_sound = readInteger(path, L"sound");
		m_interruptionLevel = readString(path, L"interruption-level");
	}

	inline const std::wstring& GetEndpoint(void) const { return m_endpoint; }
	inline const std::wstring& GetToken(void) const { return m_token; }
	inline const std::wstring& GetInterruptionLevel(void) const { return m_interruptionLevel; }
	inline UINT GetSound(void) const { return m_sound; }
};


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __CONFIGFILE_H__
