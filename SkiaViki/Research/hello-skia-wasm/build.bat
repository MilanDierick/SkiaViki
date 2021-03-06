@echo off

cd /d %~dp0

docker run -p 127.0.0.1:8000:8000/tcp -it --rm --name skia-hello-wasm ^
  --volume %cd%:/work ^
  --volume %cd%\..\..\Dependencies\skia-wasm\out\skia-wasm:/externals ^
  sts/skia-wasm-build ^
  /work/compile.sh
if errorlevel 1 goto :error

echo :-) success!
goto :exit

:error
echo :-( failure!

:exit
