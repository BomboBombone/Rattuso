#pragma once

#define OEMRESOURCE
#pragma comment(lib, "comctl32.lib")

#include "Shared/windefend.h"
#include "global.h"
#include "ntstatus.h"

#include <windows.h>

NTSTATUS StartProcessAsAdmin(LPWSTR lpName, BOOL bHide);
