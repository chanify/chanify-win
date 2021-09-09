// Chanify.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER        0x0500
#define _WIN32_WINNT  0x0500  // Require Windows 2000 or later
#define _WIN32_IE     0x0501  // Require IE 5.01 or later (comes with Windows 2000 or later)
#define _RICHEDIT_VER 0x0300  // Require RichEdit 3.0 or later (comes with Windows 2000 or later)
#define _CRT_SECURE_NO_WARNINGS	1

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
// Windows Header files
#include <windows.h>
// C Header files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
