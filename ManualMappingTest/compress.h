/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2014 - 2020
*
*  TITLE:       COMPRESS.H
*
*  VERSION:     3.50
*
*  DATE:        14 Sep 2020
*
*  Prototypes and definitions for compression.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/
#pragma once

#include <msdelta.h>
#include <compressapi.h>
#include <bcrypt.h>

//Initialization vector max bytes
#define DCU_IV_MAX_BLOCK_LENGTH 16

typedef struct _DCU_HEADER {
    DWORD Magic;
    DWORD cbData;
    DWORD cbDeltaSize;
    DWORD HeaderCrc;
    BYTE bIV[DCU_IV_MAX_BLOCK_LENGTH];
    //PBYTE pbData[1];     /* not a member of the structure */
} DCU_HEADER, *PDCU_HEADER;

typedef PVOID(*pfnDecompressPayload)(
    _In_ ULONG PayloadId,
    _In_ PVOID pbBuffer,
    _In_ ULONG cbBuffer,
    _Out_ PULONG pcbDecompressed);

PVOID DecompressPayload(
    _In_ ULONG PayloadId,
    _In_ PVOID pbBuffer,
    _In_ ULONG cbBuffer,
    _Out_ PULONG pcbDecompressed);
PVOID DecompressContainerUnit(
    _In_ PBYTE pbBuffer,
    _In_ DWORD cbBuffer,
    _In_ PBYTE pbSecret,
    _In_ DWORD cbSecret,
    _Out_ PULONG pcbDecompressed
);
BOOL DecryptBuffer(
    _In_    PBYTE  pbBuffer,
    _In_    DWORD  cbBuffer,
    _In_    PBYTE  pbIV,
    _In_    PBYTE  pbSecret,
    _In_    DWORD  cbSecret,
    _Out_   PBYTE* pbDecryptedBuffer,
    _Out_   PDWORD pcbDecryptedBuffer
);
BOOL IsValidContainerHeader(
    _In_ PDCU_HEADER UnitHeader,
    _In_ DWORD FileSize
);
PVOID SelectSecretFromBlob(
    _In_ ULONG Id,
    _Out_ PDWORD pcbKeyBlob
);
VOID EncodeBuffer(
    _In_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_ ULONG Key
);