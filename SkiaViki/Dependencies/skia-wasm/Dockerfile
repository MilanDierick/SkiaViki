FROM ubuntu:bionic as ubuntu
RUN apt-get update \
 && apt-get install -y apt-transport-https curl wget python git make

###################################################################################################

FROM ubuntu as source
RUN git clone 'https://github.com/emscripten-core/emsdk' ~/emsdk \
 && git clone 'https://chromium.googlesource.com/chromium/tools/depot_tools.git' ~/depot_tools  \
 && git clone 'https://skia.googlesource.com/skia.git' ~/skia

###################################################################################################

FROM source as build

# we need exactly this version to build libskia.wasm :(
ARG EMSCRIPTEN_VERSION=1.39.18

RUN echo Activating emsdk... \
 && cd ~/emsdk \
 && ./emsdk install ${EMSCRIPTEN_VERSION} \ 
 && ./emsdk activate ${EMSCRIPTEN_VERSION} \
 && echo Fetching skia dependencies... \
 && cd ~/skia \
 && python2 tools/git-sync-deps

WORKDIR /work

COPY ./startup.sh /
RUN chmod +x /startup.sh
ENTRYPOINT [ "/startup.sh" ]

# FROM source as build
# COPY ./compile-skia-wasm.sh /
# RUN chmod +x /compile-skia-wasm.sh
# ENTRYPOINT [ "/compile-skia-wasm.sh" ]