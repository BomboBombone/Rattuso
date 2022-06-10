/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2017 - 2020
*
*  TITLE:       COMSUP.H
*
*  VERSION:     3.53
*
*  DATE:        07 Nov 2020
*
*  Prototypes and definitions for COM interfaces and routines.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/
#pragma once

#ifndef UCM_DEFINE_GUID
#define UCM_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
     EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }  
#endif

UCM_DEFINE_GUID(IID_ICMLuaUtil, 0x6EDD6D74, 0xC007, 0x4E75, 0xB7, 0x6A, 0xE5, 0x74, 0x09, 0x95, 0xE2, 0x4C);

HRESULT ucmAllocateElevatedObject(
    _In_ LPCWSTR lpObjectCLSID,
    _In_ REFIID riid,
    _In_ DWORD dwClassContext,
    _Outptr_ void **ppv);

BOOL ucmMasqueradedCreateSubDirectoryCOM(
    _In_ LPCWSTR ParentDirectory,
    _In_ LPCWSTR SubDirectory);

BOOL ucmMasqueradedMoveCopyFileCOM(
    _In_ LPCWSTR SourceFileName,
    _In_ LPCWSTR DestinationDir,
    _In_ BOOL fMove);

BOOL ucmMasqueradedMoveFileCOM(
    _In_ LPCWSTR SourceFileName,
    _In_ LPCWSTR DestinationDir);

BOOL ucmMasqueradedDeleteDirectoryFileCOM(
    _In_ LPCWSTR FileName);

BOOL ucmMasqueradedRenameElementCOM(
    _In_ LPCWSTR OldName,
    _In_ LPCWSTR NewName);

BOOL ucmMasqueradedGetObjectSecurityCOM(
    _In_ LPCWSTR lpTargetObject,
    _In_ SECURITY_INFORMATION SecurityInformation,
    _In_ SE_OBJECT_TYPE ObjectType,
    _Inout_ LPOLESTR *Sddl);

BOOL ucmMasqueradedSetObjectSecurityCOM(
    _In_ LPCWSTR lpTargetObject,
    _In_ SECURITY_INFORMATION SecurityInformation,
    _In_ SE_OBJECT_TYPE ObjectType,
    _In_ LPCWSTR NewSddl);
