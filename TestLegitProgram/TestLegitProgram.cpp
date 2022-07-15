#include <windows.h>
#include <iostream>

#include "shortcut.h"

Shortcut shManager;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    shManager.CreateLink("C:/Users/bombo/Downloads/exp.json", "C:/Users/bombo/Desktop/exp.lnk", "HIIII");
    return 0;
}