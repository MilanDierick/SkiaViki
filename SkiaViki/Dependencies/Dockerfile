FROM ubuntu:bionic as ubuntu
RUN apt-get update \
 && apt-get install -y apt-transport-https curl wget python git make

FROM ubuntu as source
WORKDIR /src
RUN git clone 'https://github.com/emscripten-core/emsdk' ./emsdk \
 && git clone 'https://chromium.googlesource.com/chromium/tools/depot_tools.git' ./depot_tools  \
 && git clone 'https://skia.googlesource.com/skia.git' ./skia

FROM source as build
WORKDIR /src
RUN chmod +x ./compile-skia-wasm.sh
ENTRYPOINT [ "/compile-skia-wasm.sh" ]