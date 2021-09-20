// 
// Chanify.cpp
// Chanify
// 
// Created by WizJin.
//

#include "Chanify.h"
#include <Shlwapi.h>
#include "CmdLine.h"
#include "HttpClient.h"
#include "ConfigFile.h"
#include "MainFrame.h"
#include "Utils.h"

#pragma comment(lib, "shlwapi.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    CUtils::Initialize(hInstance, hPrevInstance);

    CCmdLine cmdLine(lpCmdLine);
    auto cmd = cmdLine.GetCommand();
    if (cmd == L"install") {
        if (CUtils::InstallApp()) {
            cmdLine.ShowInfo(L"Install success!");
        }
        else {
            cmdLine.ShowInfo(L"Install failed!");
        }
    }
    else if (cmd == L"uninstall") {
        if (CUtils::UninstallApp()) {
            cmdLine.ShowInfo(L"Uninstall success!");
        }
        else {
            cmdLine.ShowInfo(L"Uinstall failed!");
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
                    auto faudo = CUtils::GetFileBaseName(path);
                    if (faudo.empty()) {
                        faudo = L"audio";
                    }
                    request.AppendParamFile("audio", faudo, path);
                }
                else {
                    request.AppendParamFile("file", CUtils::GetFileBaseName(path), path);
                }
                if (httpClient.Send(request)) {
                    return TRUE;
                }
            }
        }
        cmdLine.ShowInfo(L"Send message failed!");
    }
    else {
        // version
        if (cmdLine.IsInConsole()) {
            wprintf(L"Chanify version: %s\n", CUtils::Shared()->GetVersion().c_str());
        }
        else if (CUtils::IsAppInstalled()) {
            if (MessageBoxW(NULL, L"Uninstall App or not?", L"Chanify", MB_YESNO) == IDYES) {
                if (CUtils::UninstallApp()) {
                    cmdLine.ShowInfo(L"Uninstall success!");
                }
                else {
                    cmdLine.ShowInfo(L"Uninstall failed!");
                }
            }
        }
        else {
            if (MessageBoxW(NULL, L"Install App or not?", L"Chanify", MB_YESNO) == IDYES) {
                if (CUtils::InstallApp()) {
                    cmdLine.ShowInfo(L"Install success!");
                }
                else {
                    cmdLine.ShowInfo(L"Install failed!");
                }
            }
        }
    }
    return FALSE;
}
