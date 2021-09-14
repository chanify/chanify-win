// 
// Chanify.cpp
// Chanify
// 
// Created by WizJin.
//

#include "Chanify.h"
#include <Shlwapi.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>
#include "CmdLine.h"
#include "HttpClient.h"
#include "ConfigFile.h"
#include "MainFrame.h"
#include "Utils.h"

#pragma comment(lib, "shlwapi.lib")

#define AppLnkPath  L"AppData\\Roaming\\Microsoft\\Windows\\SendTo\\Chanify.lnk"

static inline bool InstallApp(LPCWSTR lpPath) {
    bool res = false;
    HRESULT hres = CoInitialize(NULL);
    if (SUCCEEDED(hres)) {
        IShellLink* psl = NULL;
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
        if (SUCCEEDED(hres)) {
            IPersistFile* ppf;
            psl->SetPath(CUtils::Shared()->GetModulePath().c_str());
            psl->SetDescription(L"Chanify Sender");
            psl->SetArguments(L"msend");
            hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hres)) {
                hres = ppf->Save(lpPath, TRUE);
                if (SUCCEEDED(hres)) {
                    res = true;
                }
                ppf->Release();
            }
            psl->Release();
        }
        CoUninitialize();
    }
    return res;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    CUtils::Initialize(hInstance, hPrevInstance);

    CCmdLine cmdLine(lpCmdLine);
    auto cmd = cmdLine.GetCommand();
    if (cmd == L"install") {
        auto path = CUtils::GetUserProfilePath(AppLnkPath);
        if (PathFileExists(path.c_str())) {
            DeleteFileW(path.c_str());
        }
        if (InstallApp(path.c_str())) {
            MessageBoxW(NULL, L"Install success!", L"Info", MB_OK);
        }
        else {
            MessageBoxW(NULL, L"Install failed!", L"Info", MB_OK);
        }
    }
    else if (cmd == L"uninstall") {
        auto path = CUtils::GetUserProfilePath(AppLnkPath);
        if (!PathFileExists(path.c_str()) || DeleteFileW(path.c_str())) {
            MessageBoxW(NULL, L"Uninstall success!", L"Info", MB_OK);
        }
        else {
            MessageBoxW(NULL, L"Uninstall failed!", L"Info", MB_OK);
        }
    }
    else if (cmd == L"send") {
        CConfigFile configFile;
        CHttpClient httpClient;
        auto endpoint = cmdLine.GetParam(L"endpoint");
        if (endpoint.empty()) endpoint = configFile.GetEndpoint();
        if (!endpoint.empty()) {
            auto token = cmdLine.GetParam(L"token");
            if (token.empty()) token = configFile.GetToken();
            CHttpRequest request(endpoint + L"/v1/sender", token);
            request.AppendParamInteger("sound", configFile.GetSound());
            auto text = cmdLine.GetParam(L"text");
            if (!text.empty()) request.AppendParamString("text", text);
            auto title = cmdLine.GetParam(L"title");
            if (!title.empty()) request.AppendParamString("title", title);
            auto link = cmdLine.GetParam(L"link");
            if (!link.empty()) request.AppendParamString("link", link);
            if (httpClient.Send(request)) {
                return TRUE;
            }
        }
    }
    else if (cmd == L"msend") {
        auto path = cmdLine.GetArgument();
        if (!path.empty()) {
            CConfigFile configFile;
            CHttpClient httpClient;
            auto endpoint = cmdLine.GetParam(L"endpoint");
            if (endpoint.empty()) endpoint = configFile.GetEndpoint();
            if (!endpoint.empty()) {
                auto token = cmdLine.GetParam(L"token");
                if (token.empty()) token = configFile.GetToken();
                CHttpRequest request(endpoint + L"/v1/sender", token);
                request.AppendParamInteger("sound", configFile.GetSound());
                auto ext = CUtils::GetFileExt(path);
                if (ext == L"png" || ext == L"jpeg" || ext == L"jpg") {
                    request.AppendParamFile("image", L"image", path);
                }
                else if (ext == L"mp3" || ext == L"m4a" || ext == L"aa" || ext == L"aax" || ext == L"aac" || ext == L"ac3" || ext == L"wav" || ext == L"aiff" || ext == L"flac") {
                    request.AppendParamFile("audio", L"audio", path);
                }
                else {
                    request.AppendParamFile("file", CUtils::GetFileBaseName(path), path);
                }
                if (httpClient.Send(request)) {
                    return TRUE;
                }
            }
        }
        MessageBoxW(NULL, L"Send message failed!", L"Info", MB_OK);
    }
    else {
        // version
        if (cmdLine.IsInConsole()) {
            wprintf(L"Chanify version: %s\n", CUtils::Shared()->GetVersion().c_str());
        }
        else {
            auto ver = std::wstring(L"Chanify version: ") + CUtils::Shared()->GetVersion();
            MessageBoxW(NULL, ver.c_str(), L"Chanify", MB_OK);
        }
    }
    return FALSE;
}
