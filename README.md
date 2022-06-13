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
.\vcpkg install libzippp:x64-windows-static
.\vcpkg integrate install
```

## How to build server (Native Windows/Linux):
Use these commands from Server source files folder
```
cmake -S . -B build 
cmake --build build --target ServerCrossPlatform --config Release
```

## How it works
At the moment the RAT is made of 4 modules + 1 that is the legitimate application.

### Module list:
1. Main loader that extracts and executes the legitimate executable
2. Main DLL which gets manually mapped by the loader inside the legitimate executable
3. Shell manager service to aid with persistence
4. Reverse shell

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
The shell has 2 branches inside the main function, and each one belongs to one specific process.
When first started it will attempt to open the backup shell, which only purpose in life is to keep the main shell running.
### Important considerations:
1. Backup shell code needs to run on a secondary process to make shit difficult to reverse.
2. Backup shell main function should not just Sleep(INDEFINITELY) but you should put useless (but not useless looking) code that loops infinitely.
3. None of the branches will be reached if the shell isn't started in one of 2 predefined folders with a specific name.
4. Backup shell will always try to be the parent process of the main shell, so that it can be its debugger (check out self debugging malware)

## Shell manager service (.NET framework)
This service acts as a backup mechanism to get the main shell back up in case the backup shell fails (idk why it should happen, but u never know).
It will check every couple seconds if the main shell is still running, and if it still exists on disk.
It will also set a task to start the backup shell once a day starting from a couple hours after initial infection.

# Credits:
Shell and server have been cloned and modified from Lilith RAT to use as boilerplate.
UAC bypass has been kindly cloned from n.41 bypass of UACme.
I'd also like to thank myself for putting effort into this knowing these weeks of work will be paid only with a couple laughs in the near future.
