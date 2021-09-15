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
#include <sys/stat.h>
#include <vector>
#include "Utils.h"
#pragma comment(lib, "winhttp.lib")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CHttpRequest
{
private:
	class IParam {
	protected:
		std::string m_data;
	public:
		explicit IParam(const std::string& name) {
			m_data = std::string("Content-Disposition: form-data; name=\"") + CUtils::HttpEscape(name) + "\"\r\n\r\n";
		}
		virtual ~IParam() {}
		virtual DWORD GetContentLength(void) const {
			return (DWORD)m_data.size();
		}
		virtual bool WriteData(HINTERNET hRequest) const {
			DWORD nSize = 0;
			return WinHttpWriteData(hRequest, m_data.c_str(), (DWORD)m_data.size(), &nSize);
		}
	};

	class IntParam : public IParam {
	public:
		explicit IntParam(const std::string& name, UINT val) : IParam(name) {
			CHAR str[128] = { 0 };
			sprintf_s(str, _countof(str), "%d\r\n", val);
			m_data += std::string(str);
		}
	};

	class StringParam : public IParam {
	public:
		explicit StringParam(const std::string& name, const std::wstring& val) : IParam(name) {
			m_data += CUtils::W2A(val) + "\r\n";
		}
	};

	class FileParam : public IParam {
	private:
		FILE* m_fp;

	public:
		explicit FileParam(const std::string& name, const std::wstring& fname, const std::wstring& path) : IParam(name) {
			m_data = std::string("Content-Disposition: form-data; name=\"") + CUtils::HttpEscape(name)
					+ "\"; filename=\"" + CUtils::HttpEscape(CUtils::W2A(fname)) + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
			m_fp = _wfopen(path.c_str(), L"rb");
		}
		virtual ~FileParam() {
			if (m_fp) {
				fclose(m_fp);
				m_fp = NULL;
			}
		}
		virtual DWORD GetContentLength(void) const {
			DWORD size = (DWORD)m_data.size() + 2;
			if (m_fp != NULL) {
				fseek(m_fp, 0, SEEK_END);
				size += ftell(m_fp);
			}
			return size;
		}
		virtual bool WriteData(HINTERNET hRequest) const {
			DWORD nSize = 0;
			if (!WinHttpWriteData(hRequest, m_data.c_str(), (DWORD)m_data.size(), &nSize)) {
				return false;
			}
			if (m_fp != NULL) {
				fseek(m_fp, 0, SEEK_SET);
				for (;;) {
					uint8_t buffer[4096];
					size_t sz = fread(buffer, 1, _countof(buffer), m_fp);
					if (sz <= 0) {
						break;
					} else {
						if (!WinHttpWriteData(hRequest, buffer, (DWORD)sz, &nSize)) {
							return false;
						}
					}
				}
			}
			CHAR end[] = "\r\n";
			return WinHttpWriteData(hRequest, end, 2, &nSize);
		}
	};

	std::wstring	m_url;
	std::wstring	m_token;
	std::string		m_boundary;
	std::wstring	m_contentType;
	std::vector<std::unique_ptr<IParam>>	m_params;
public:
	explicit CHttpRequest(const std::wstring& url, const std::wstring& token) {
		m_url = url;
		m_token = token;
		CHAR boundary[31] = { 0 };
		for (int i = 0; i < _countof(boundary) - 1; i++) {
			static const CHAR tbl[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
			boundary[i] = tbl[rand() % (_countof(tbl) - 1)];
		}
		m_boundary = boundary;
		m_contentType = std::wstring(L"multipart/form-data; boundary=") + CUtils::A2W(m_boundary);
	}

	virtual ~CHttpRequest() {
	}

	void AppendParamInteger(const std::string& name, UINT val) {
		m_params.push_back(std::make_unique<IntParam>(name, val));
	}

	void AppendParamString(const std::string& name, const std::wstring& val) {
		m_params.push_back(std::make_unique<StringParam>(name, val));
	}

	void AppendParamFile(const std::string& name, const std::wstring fname, const std::wstring path) {
		m_params.push_back(std::make_unique<FileParam>(name, fname, path));
	}

	inline const std::wstring& GetUrl(void) const { return m_url; }
	inline const std::wstring& GetToken(void) const { return m_token; }
	inline const std::wstring& GetContentType(void) const { return m_contentType; }

	DWORD GetContentLength(void) const {
		DWORD length = 0;
		if (!m_params.empty()) {
			for (auto p = m_params.begin(); p != m_params.end(); p++) {
				length += (*p)->GetContentLength() + (DWORD)m_boundary.size() + 4;
			}
			length += (DWORD)m_boundary.size() + 4;
		}
		return length;
	}

	bool WriteData(HINTERNET hRequest) const {
		bool res = true;
		if (!m_params.empty()) {
			DWORD nSize = 0;
			std::string boundary = std::string("--") + m_boundary + "\r\n";
			for (auto p = m_params.begin(); p != m_params.end(); p++) {
				if (!WinHttpWriteData(hRequest, boundary.c_str(), (DWORD)boundary.size(), &nSize)) {
					res = false;
					break;
				}
				if (!(*p)->WriteData(hRequest)) {
					res = false;
					break;
				}
			}
			if (res) {
				boundary = std::string("--") + m_boundary + "--";
				if (!WinHttpWriteData(hRequest, boundary.c_str(), (DWORD)boundary.size(), &nSize)) {
					res = false;
				}
			}
		}
		return res;
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

	bool Send(const CHttpRequest& request) {
		bool res = false;
		if (m_hSession != NULL) {
			std::wstring hostName;
			std::wstring urlPath;
			auto url = request.GetUrl();
			hostName.resize(url.size());
			urlPath.resize(url.size());
			URL_COMPONENTSW urlComp = { 0 };
			urlComp.dwStructSize = sizeof(urlComp);
			urlComp.lpszHostName = const_cast<wchar_t*>(hostName.data());
			urlComp.dwHostNameLength = (DWORD)hostName.size();
			urlComp.lpszUrlPath = const_cast<wchar_t*>(urlPath.data());
			urlComp.dwUrlPathLength = (DWORD)urlPath.size();
			if (WinHttpCrackUrl(url.c_str(), (DWORD)url.size(), 0, &urlComp)) {
				HINTERNET hConnect = WinHttpConnect(m_hSession, hostName.c_str(), urlComp.nPort, 0);
				if (hConnect != NULL) {
					DWORD dwRequestFlags = (INTERNET_SCHEME_HTTP == urlComp.nScheme ? 0 : WINHTTP_FLAG_SECURE);
					HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath.c_str(), NULL, NULL, NULL, dwRequestFlags);
					if (hRequest != NULL) {
						if (WinHttpSetTimeouts(hRequest, 10000, 10000, 15000, 15000)
							&& SetHeader(hRequest, L"Token", request.GetToken().c_str())
							&& SetHeader(hRequest, L"Content-Type", request.GetContentType().c_str())) {
							DWORD totalSize = request.GetContentLength();
							if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, totalSize, NULL)
								&& request.WriteData(hRequest)
								&& WinHttpReceiveResponse(hRequest, NULL)) {

								res = true;
							}
						}
						WinHttpCloseHandle(hRequest);
					}
					WinHttpCloseHandle(hConnect);
				}
			}
		}
		return res;
	}
private:
	static inline bool SetHeader(HINTERNET hRequest, const std::wstring & name, const std::wstring & value) {
		bool res = false;
		if (hRequest != NULL) {
			std::wstring header = name + L": " + value;
			if (WinHttpAddRequestHeaders(hRequest, header.c_str(), (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
				res = true;
			}
		}
		return res;
	}
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HTTPCLIENT_H__s
