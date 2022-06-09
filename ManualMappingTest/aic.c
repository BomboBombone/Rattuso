/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2017 - 2021
*
*  TITLE:       AIC.C
*
*  VERSION:     3.56
*
*  DATE:        30 July 2021
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/
#include "global.h"

/*
* AicpAsyncInitializeHandle
*
* Purpose:
*
* Init RPC_ASYNC_STATE structure.
*
*/
RPC_STATUS AicpAsyncInitializeHandle(
    _Inout_ RPC_ASYNC_STATE* AsyncState)
{
    RPC_STATUS status;

    status = RpcAsyncInitializeHandle(AsyncState, sizeof(RPC_ASYNC_STATE));
    if (status == RPC_S_OK) {
        AsyncState->NotificationType = RpcNotificationTypeEvent;
        AsyncState->u.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (AsyncState->u.hEvent == NULL)
            status = GetLastError();
    }

    return status;
}

/*
* AicpAsyncCloseHandle
*
* Purpose:
*
* Close RPC_ASYNC_STATE notification event.
*
*/
VOID AicpAsyncCloseHandle(
    _Inout_ RPC_ASYNC_STATE* AsyncState)
{
    if (AsyncState->u.hEvent) {
        CloseHandle(AsyncState->u.hEvent);
        AsyncState->u.hEvent = NULL;
    }
}