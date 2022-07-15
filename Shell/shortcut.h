#pragma once
#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

#include <Shared/utility.h>

class Shortcut
{
public:
    Shortcut() {
        CoInitialize(NULL);
    };
    ~Shortcut() {
        CoUninitialize();
    }
    static HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDescription);
};

