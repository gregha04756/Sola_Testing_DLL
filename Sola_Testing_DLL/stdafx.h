// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#pragma comment (lib,"comctl32.lib")
#pragma comment <lib,"strsafe.lib")
#pragma comment (lib,"Report_Error_DLL.lib")

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// TODO: reference additional headers your program requires here
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdint.h>
#include <IPHlpApi.h>
#include <WinSock2.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <new>
#include <list>
#include <string>
#include "Report_Error_DLL.h"