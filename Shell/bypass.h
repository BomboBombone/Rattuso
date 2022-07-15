#pragma once
#include <windows.h>
#include <winioctl.h>

#define OEMRESOURCE
#pragma comment(lib, "comctl32.lib")

#include "Shared/windefend.h"
#include "global_uac.h"
#include "ntstatus.h"


NTSTATUS StartProcessAsAdmin(LPWSTR lpName, BOOL bHide);
