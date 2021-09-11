// 
// MainFrame.cpp
// Chanify
// 
// Created by WizJin.
//

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#pragma once

#include "Chanify.h"
#include "Utils.h"
#include "Resource.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class CWndClass
{
private:
    WCHAR       m_szName[128];
public:
    explicit CWndClass(LPCWSTR szWindowClass, WNDPROC WndProc) {
        if (szWindowClass == NULL || lstrcpynW(m_szName, szWindowClass, _countof(m_szName)) == NULL) {
            wprintf(m_szName, "WndClz.%p", this);
        }

        HINSTANCE hInstance = CUtils::Shared()->GetInstance();

        WNDCLASSEX wcex;
        ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHANIFY));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CHANIFY);
        wcex.lpszClassName = m_szName;
        wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHANIFY));
    
        RegisterClassExW(&wcex);
    }

    virtual ~CWndClass() {
        UnregisterClassW(m_szName, CUtils::Shared()->GetInstance());
    }

    inline LPCWSTR Name(void) const { return m_szName; }
};

class CMainFrame
{
private:
    HWND        m_hWnd;
    WCHAR       m_szTitle[128];
public:
    explicit CMainFrame() {
        m_hWnd = NULL;
        LoadStringW(CUtils::Shared()->GetInstance(), IDS_APP_TITLE, m_szTitle, _countof(m_szTitle));
    }

    BOOL Create(int nCmdShow) {
        m_hWnd = CreateWindowW(GetWndClass()->Name(), m_szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, CUtils::Shared()->GetInstance(), this);
        if (!m_hWnd) {
            return FALSE;
        }
        ShowWindow(m_hWnd, nCmdShow);
        UpdateWindow(m_hWnd);
        return TRUE;
    }

	int Run(void) {
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        HACCEL hAccelTable = LoadAccelerators(CUtils::Shared()->GetInstance(), MAKEINTRESOURCE(IDC_CHANIFY));
        if (hAccelTable) {
            while (GetMessage(&msg, nullptr, 0, 0)) {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            DestroyAcceleratorTable(hAccelTable);
        }
        return (int)msg.wParam;
	}
private:
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId) {
            case IDM_ABOUT:
                DialogBox(CUtils::Shared()->GetInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, &CMainFrame::About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
        UNREFERENCED_PARAMETER(lParam);
        switch (message) {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
        return (INT_PTR)FALSE;
    }

    static LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        CMainFrame* pMainFrame = reinterpret_cast<CMainFrame*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (pMainFrame == nullptr && message == WM_CREATE) {
            LPCREATESTRUCTW lpCreateStruct = (LPCREATESTRUCTW)lParam;
            if (lpCreateStruct != NULL) {
                pMainFrame = reinterpret_cast<CMainFrame*>(lpCreateStruct->lpCreateParams);
                if (pMainFrame != NULL) {
                    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pMainFrame);
                }
            }
        }
        if (pMainFrame != nullptr) {
            return pMainFrame->WndProc(hWnd, message, wParam, lParam);
        }
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    static std::shared_ptr<CWndClass>& GetWndClass(void) {
        static std::shared_ptr<CWndClass> m_pWndClass;
        if (!m_pWndClass) {
            m_pWndClass = std::make_shared<CWndClass>(L"CMainFrame", &CMainFrame::WndProcCallback);
        }
        return m_pWndClass;
    }
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MAINFRAME_H__
