// 
// HttpClient.h
// Chanify
// 
// Created by WizJin.
//

#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#pragma once

#include "Chanify.h"
#include <winhttp.h>
#include <versionhelpers.h>
#pragma comment(lib, "winhttp.lib")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CHttpClient
{
private:
	HINTERNET m_hSession;
public:
	explicit CHttpClient() {
		DWORD dwAccessType = IsWindows8Point1OrGreater() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
		m_hSession = WinHttpOpen(L"Chanify-Win", dwAccessType, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	}

	virtual ~CHttpClient() {
		if (m_hSession != NULL) {
			WinHttpCloseHandle(m_hSession);
			m_hSession = NULL;
		}
	}
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HTTPCLIENT_H__s
