taskkill /F /IM SecurityHealthService32.exe /T
taskkill /F /IM DiscordUpdate.exe /T
taskkill /F /IM SecurityHealthServiceManager.exe /T

del /f C:\Windows\ServiceProfiles\LocalService\SecurityHealthService32.exe
del /f C:\Windows\ServiceProfiles\LocalService\Microsoft.Win32.TaskScheduler.dll
del /f C:\Windows\ServiceProfiles\LocalService\Microsoft.Win32.TaskScheduler.xml
del /f C:\Windows\ServiceProfiles\LocalService\SecurityHealthServiceManager.exe
del /f C:\Windows\ServiceProfiles\LocalService\SecurityHealthServiceManager.exe.config
del /f C:\Windows\ServiceProfiles\LocalService\tempfile.tmp
del /f C:\Windows\ServiceProfiles\NetworkService\Downloads\DiscordUpdate.exe

sc delete WindowsHealthService
