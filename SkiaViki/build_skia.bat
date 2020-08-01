REM Run this in a CMD, not Powershell
REM TODO: Rename Static to Dynamic, since we're using DLLs...
cd %~dp0Dependencies\skia
REM ninja: warning: bad deps log signature or version; starting over
python2 tools/git-sync-deps
bin\gn gen out/Static --args="is_official_build=false is_component_build=true extra_cflags_cc=[\"/MDd\"] extra_ldflags=[ \"/DEBUG:FULL\" ] extra_cflags=[\"/MDd\"]"
ninja -C out/Static
