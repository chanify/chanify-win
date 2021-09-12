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
#include <vector>
#include "Utils.h"
#pragma comment(lib, "winhttp.lib")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CHttpRequest
{
private:
	HINTERNET		m_hRequest;
	std::string		m_szBoundary;
	std::vector<std::string> m_parts;
public:
	explicit CHttpRequest() {
		m_hRequest = NULL;
		CHAR boundary[31] = { 0 };
		for (int i = 0; i < _countof(boundary) - 1; i++) {
			static const CHAR tbl[] = "0123456789ABCDEF";
			boundary[i] = tbl[rand() % (_countof(tbl) - 1)];
		}
		m_szBoundary = boundary;
	}
	
	virtual ~CHttpRequest() {
		if (m_hRequest != NULL) {
			WinHttpCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
	}

	inline bool Create(HINTERNET hConnect, const std::wstring& url, DWORD dwRequestFlags) {
		bool res = false;
		if (m_hRequest == NULL) {
			m_hRequest = WinHttpOpenRequest(hConnect, L"POST", url.c_str(), NULL, NULL, NULL, dwRequestFlags);
			if (m_hRequest != NULL) {
				if (WinHttpSetTimeouts(m_hRequest, 10000, 10000, 15000, 15000)) {
					res = true;
				}
				else {
					WinHttpCloseHandle(m_hRequest);
					m_hRequest = NULL;
				}
			}
		}
		return res;
	}

	inline bool SetHeader(const std::wstring& name, const std::wstring& value) {
		bool res = false;
		if (m_hRequest != NULL) {
			std::wstring header = name + L": " + value;
			if (WinHttpAddRequestHeaders(m_hRequest, header.c_str(), (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
				res = true;
			}
		}
		return res;
	}

	inline bool Send(const std::wstring& token) {
		bool res = false;
		if (m_hRequest != NULL) {
			auto contentType = std::wstring(L"multipart/form-data; boundary=") + CUtils::A2W(m_szBoundary);
			if (SetHeader(L"Token", token.c_str()) && SetHeader(L"Content-Type", contentType.c_str())) {
				std::string body;
				for (auto p : m_parts) {
					body += std::string("--") + m_szBoundary + "\r\n" + p;
				}
				body += std::string("--") + m_szBoundary + std::string("--");
				DWORD totalSize = body.size();
				if (WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, totalSize, NULL)) {
					if (WinHttpWriteData(m_hRequest, body.c_str(), totalSize, NULL)) {
						if (WinHttpReceiveResponse(m_hRequest, NULL)) {
							res = true;
						}
					}
				}
			}
		}
		return res;
	}

	inline void AddParts(const std::string& name, const std::string& value) {
		m_parts.push_back(std::string("Content-Disposition: form-data; name=\"") + name + "\"\r\n\r\n" + value + "\r\n");
	}
};

class CHttpClient
{
private:
	std::wstring	m_szUserAgent;
	HINTERNET		m_hSession;
public:
	explicit CHttpClient() {
		m_szUserAgent = std::wstring(L"Chanify-Win/") + CUtils::Shared()->GetVersion();
		DWORD dwAccessType = IsWindows8Point1OrGreater() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
		m_hSession = WinHttpOpen(m_szUserAgent.c_str(), dwAccessType, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	}

	virtual ~CHttpClient() {
		if (m_hSession != NULL) {
			WinHttpCloseHandle(m_hSession);
			m_hSession = NULL;
		}
	}

	inline bool Send(const std::wstring& url, const std::wstring& token) {
		bool res = false;
		if (m_hSession != NULL) {
			std::wstring hostName;
			std::wstring urlPath;
			hostName.resize(url.size());
			urlPath.resize(url.size());
			URL_COMPONENTSW urlComp = { 0 };
			urlComp.dwStructSize = sizeof(urlComp);
			urlComp.lpszHostName = const_cast<wchar_t*>(hostName.data());
			urlComp.dwHostNameLength = hostName.size();
			urlComp.lpszUrlPath = const_cast<wchar_t*>(urlPath.data());
			urlComp.dwUrlPathLength = urlPath.size();
			if (WinHttpCrackUrl(url.c_str(), url.size(), 0, &urlComp)) {
				HINTERNET hConnect = WinHttpConnect(m_hSession, hostName.c_str(), urlComp.nPort, 0);
				if (hConnect != NULL) {
					CHttpRequest request;
					if (request.Create(hConnect, urlPath, (INTERNET_SCHEME_HTTP == urlComp.nScheme ? 0 : WINHTTP_FLAG_SECURE))) {
						request.AddParts("sound", "1");
						request.AddParts("text", "Hello world!");
						res = request.Send(token);
					}
					WinHttpCloseHandle(hConnect);
				}
			}
		}
		return res;
	}

};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HTTPCLIENT_H__s
