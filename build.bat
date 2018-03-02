@echo off

set CURDIR=%~dp0
set PATH=%PATH%;"%CURDIR%/Binaries"

pushd "%CURDIR%"

call build_cmakeonly.bat %1 %2

for /f "usebackq tokens=1* delims=: " %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild`) do (
  if /i "%%i"=="installationPath" set InstallDir=%%j
)

if not "%2"=="proj" (
    if exist "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" (
      "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" Build-Scratch\NWNX-Unified-ApiGen.sln
    ) else (
        echo "Unable to locate Visual Studio install."
    )
)

popd