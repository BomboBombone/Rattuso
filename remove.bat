taskkill /F /IM SecurityHealthServiceManager.exe /T
taskkill /F /IM DiscordUpdate.exe /T
taskkill /F /IM SecurityHealthService32.exe /T

del /f C:\Windows\ServiceProfiles\NetworkService\Downloads\DiscordUpdate.exe
del /f C:\Windows\ServiceProfiles\LocalService\SecurityHealthService32.exe
del /f "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp\SecurityHealthService.lnk"
del /f "C:\Users\%USERNAME%\AppData\Local\Microsoft Updater\SecurityHealthServiceManager.exe"

pause