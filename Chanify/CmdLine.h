// 
// CmdLine.h
// Chanify
// 
// Created by WizJin.
//

#ifndef __CMDLINE_H__
#define __CMDLINE_H__

#pragma once

#include "Chanify.h"
#include <stdio.h>
#include <shellapi.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CCmdLine
{
private:
	bool			m_bAttached;
	std::wstring	m_cmd;
public:
	explicit CCmdLine(LPWSTR lpCmdLine) {
		m_bAttached = AttachOutputToConsole();
		if (lpCmdLine != NULL) {
			int nArgs;
			LPWSTR* szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);
			if (szArglist != NULL && nArgs > 0) {
				int start = 0;
				std::wstring cmd = szArglist[0];
				if (!cmd.empty() && cmd[0] != '-' && cmd[0] != '/') {
					for (auto c : cmd) {
						m_cmd += std::tolower(c);
					}
					start++;
				}
				for (int i = start; i < nArgs; i++) {
					//wprintf(L"%d: %s\n", i, szArglist[i]);
				}
				LocalFree(szArglist);
			}
		}
	}

	virtual ~CCmdLine() {
		if (m_bAttached) {
			HWND hWndConsole = GetConsoleWindow();
			FreeConsole();
			if (hWndConsole == GetForegroundWindow()) {
				INPUT ip;
				ZeroMemory(&ip, sizeof(ip));
				ip.type = INPUT_KEYBOARD;
				ip.ki.wVk = VK_RETURN;
				ip.ki.dwFlags = 0;
				SendInput(1, &ip, sizeof(INPUT));
				ip.ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(1, &ip, sizeof(INPUT));
			}
		}
	}

	inline bool IsInConsole(void) const { return m_bAttached; }

	inline const std::wstring Command(void) const { return m_cmd; }

private:
	inline bool AttachOutputToConsole(void) {
		if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
			return false;
		}
		HANDLE handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (handleOut == INVALID_HANDLE_VALUE) {
			return false;
		}
		FILE* file = stdout;
		freopen_s(&file, "CONOUT$", "w", stdout);
		setvbuf(stdout, NULL, _IONBF, 0);
		HANDLE handleError = GetStdHandle(STD_ERROR_HANDLE);
		if (handleError == INVALID_HANDLE_VALUE) {
			return false;
		}
		file = stderr;
		freopen_s(&file, "CONOUT$", "w", stderr);
		setvbuf(stderr, NULL, _IONBF, 0);

		ClearLastLine(handleOut);
		return true;
	}

	inline void ClearLastLine(HANDLE hStdOut) {
		DWORD mode = 0;
		if (GetConsoleMode(hStdOut, &mode)) {
			const DWORD originalMode = mode;
			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			if (SetConsoleMode(hStdOut, mode)) {
				DWORD written = 0;
				PCWSTR sequence = L"\33[2K\r";
				WriteConsoleW(hStdOut, sequence, (DWORD)wcslen(sequence), &written, NULL);
			}
			SetConsoleMode(hStdOut, originalMode);
		}
	}
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __CMDLINE_H__
