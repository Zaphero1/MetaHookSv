echo off

set LauncherExe=metahook.exe
set LauncherMod=czeror
set FullGameName=Counter-Strike : Condition Zero - Deleted Scenes

for /f "delims=" %%a in ('"%~dp0SteamAppsLocation/SteamAppsLocation" 100 InstallDir') do set GameDir=%%a

if "%GameDir%"=="" goto fail

echo -----------------------------------------------------

echo Writing debug properties...
copy global_template.props global.props /y
call powershell -Command "(gc global.props) -replace '<MetaHookLaunchName>.*</MetaHookLaunchName>', '<MetaHookLaunchName>%LauncherExe%</MetaHookLaunchName>' | Out-File global.props"
call powershell -Command "(gc global.props) -replace '<MetaHookLaunchCommnand>.*</MetaHookLaunchCommnand>', '<MetaHookLaunchCommnand>-game %LauncherMod%</MetaHookLaunchCommnand>' | Out-File global.props"
call powershell -Command "(gc global.props) -replace '<MetaHookGameDirectory>.*</MetaHookGameDirectory>', '<MetaHookGameDirectory>%GameDir%\</MetaHookGameDirectory>' | Out-File global.props"
call powershell -Command "(gc global.props) -replace '<MetaHookModName>.*</MetaHookModName>', '<MetaHookModName>%LauncherMod%</MetaHookModName>' | Out-File global.props"

echo -----------------------------------------------------

echo Done

@echo off

tasklist | find /i "devenv.exe"

if "%errorlevel%"=="1" (goto ok1) else (goto ok2)

:ok1
@echo You can open MetaHook.sln with Visual Studio IDE now
pause
exit

:ok2
@echo Please restart Visual Studio IDE to apply changes to the debug properties
pause
exit

:fail
@echo Failed to locate GameInstallDir of %FullGameName%, please make sure Steam is running and you have %FullGameName% installed correctly.
pause
exit