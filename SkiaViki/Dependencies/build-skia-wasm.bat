@echo off

docker build --target ubuntu -t sts/skia-wasm-ubuntu - < Dockerfile
if errorlevel 1 goto :error

docker build --target source -t sts/skia-wasm-source - < Dockerfile
if errorlevel 1 goto :error

docker build --target build -t sts/skia-wasm-build - < Dockerfile
if errorlevel 1 goto :error

REM The command below allows entering the docker OS in  a bash shell
REM docker run -it --rm --name skia-wasm --volume %cd%:/work sts/skia-wasm-source /bin/bash

echo :-) success!
goto :exit

:error
echo :-( failure!

:exit
