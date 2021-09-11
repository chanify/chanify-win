// 
// Chanify.cpp
// Chanify
// 
// Created by WizJin.
//

#include "Chanify.h"
#include "CmdLine.h"
#include "HttpClient.h"
#include "ConfigFile.h"
#include "MainFrame.h"
#include "Utils.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    CUtils::Initialize(hInstance, hPrevInstance);

    CCmdLine cmdLine(lpCmdLine);
    CConfigFile configFile;
    if (cmdLine.IsInConsole()) {
        auto cmd = cmdLine.Command();
        if (cmd == L"version") {
            wprintf(L"Chanify version: %s\n", CUtils::Shared()->GetVersion().c_str());
        }
        return 0;
    }

    
    return 0;
//    CMainFrame mainFrame;
//    if (mainFrame.Create(nCmdShow)) {
//        return mainFrame.Run();
//    }
//    return FALSE;
}
