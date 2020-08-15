#!/bin/bash

# exit asap
set -ex

# we need exactly this version to build libskia.wasm :(
EMSCRIPTEN_VERSION=1.39.18

echo Updating emsdk...
cd /src/emsdk
git reset --hard
git pull

echo Activating emsdk...
./emsdk install ${EMSCRIPTEN_VERSION}
./emsdk activate ${EMSCRIPTEN_VERSION}
source ./emsdk_env.sh

echo Updating depot_tools...
cd /src/depot_tools
git reset --hard
git pull

echo Activating depot_tools...
export PATH="/src/depot_tools:${PATH}"

echo Updating skia...

BASE_DIR="/src/skia"

cd $BASE_DIR
python2 tools/git-sync-deps

EMCC=`which emcc`
EMCXX=`which em++`
EMAR=`which emar`
NINJA=`which ninja`

if [[ $@ == *debug* ]]; then
  echo "Building a Debug build"
  EXTRA_CFLAGS="\"-DSK_DEBUG\","
  RELEASE_CONF="-O0 --js-opts 0 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -g4 \
                --source-map-base /node_modules/debugger/bin/ -DSK_DEBUG"
  BUILD_DIR=${BUILD_DIR:="/work/out/skia-wasm/debug"}
else
  echo "Building a Release build"
  EXTRA_CFLAGS="\"-DSK_RELEASE\", \"-DGR_GL_CHECK_ALLOC_WITH_GET_ERROR=0\","
  RELEASE_CONF="-Oz --closure 1 --llvm-lto 3 -DSK_RELEASE -DGR_GL_CHECK_ALLOC_WITH_GET_ERROR=0"
  BUILD_DIR=${BUILD_DIR:="/work/out/skia-wasm/release"}
fi

mkdir -p $BUILD_DIR

GN_GPU_FLAGS="\"-DSK_DISABLE_LEGACY_SHADERCONTEXT\","
WASM_GPU="-lEGL -lGLESv2 -DSK_SUPPORT_GPU=1 -DSK_GL \
          -DSK_DISABLE_LEGACY_SHADERCONTEXT --pre-js $BASE_DIR/cpu.js --pre-js $BASE_DIR/gpu.js"

./bin/fetch-gn

echo "Compiling bitcode"

./bin/gn gen ${BUILD_DIR} \
  --args="cc=\"${EMCC}\" \
  cxx=\"${EMCXX}\" \
  ar=\"${EMAR}\" \
  extra_cflags_cc=[\"-frtti\"] \
  extra_cflags=[\"-s\", \"WARN_UNALIGNED=1\", \"-s\", \"MAIN_MODULE=1\",
    \"-DSKNX_NO_SIMD\", \"-DSK_DISABLE_AAA\",
    ${GN_GPU_FLAGS}
    ${EXTRA_CFLAGS}
  ] \
  is_debug=false \
  is_official_build=true \
  is_component_build=false \
  werror=true \
  target_cpu=\"wasm\" \
  \
  skia_use_angle = false \
  skia_use_dng_sdk=false \
  skia_use_egl=true \
  skia_use_expat=false \
  skia_use_fontconfig=false \
  skia_use_freetype=true \
  skia_use_libheif=false \
  skia_use_libjpeg_turbo_decode=true \
  skia_use_libjpeg_turbo_encode=false \
  skia_use_libpng_decode=true \
  skia_use_libpng_encode=false \
  skia_use_libwebp_decode=true \
  skia_use_libwebp_encode=false \
  skia_use_wuffs=true \
  skia_use_lua=false \
  skia_use_piex=false \
  skia_use_system_libpng=false \
  skia_use_system_freetype2=false \
  skia_use_system_libjpeg_turbo = false \
  skia_use_system_libwebp=false \
  skia_use_system_zlib=false\
  skia_use_vulkan=false \
  skia_use_zlib=true \
  skia_enable_gpu=true \
  skia_enable_tools=false \
  skia_enable_skshaper=false \
  skia_enable_ccpr=false \
  skia_enable_nvpr=false \
  skia_enable_fontmgr_custom_directory=false \
  skia_enable_fontmgr_custom_embedded=true \
  skia_enable_fontmgr_custom_empty=false \
  skia_enable_pdf=false"

# Build all the libs, we'll link the appropriate ones down below
${NINJA} -C ${BUILD_DIR} libskia.a
