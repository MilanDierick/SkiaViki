@echo off

set OLD_PATH=%PATH%

echo ### Run this in a CMD, not Powershell ###

set PATH="%~dp0Dependencies\depot_tools";%PATH%

pushd %~dp0Dependencies\skia
if errorlevel 1 goto :error

REM ninja: warning: bad deps log signature or version; starting over

python tools/git-sync-deps
if errorlevel 1 goto :error

bin\gn gen out/Dynamic/Debug --args="is_official_build=false is_component_build=true extra_cflags_cc=[\"/MDd\"] extra_ldflags=[ \"/DEBUG:FULL\" ] extra_cflags=[\"/MDd\"]"
if errorlevel 1 goto :error

ninja -C out/Dynamic/Debug
if errorlevel 1 goto :error

echo :-) Success
goto :exit

:error
echo :-( Failure
goto :exit

:exit
set PATH=%OLD_PATH%

popd
popd
