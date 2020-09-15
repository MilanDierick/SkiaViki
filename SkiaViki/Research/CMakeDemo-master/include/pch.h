//
// Created by milan on 9/5/2020.
//

#ifndef CMAKEDEMO_PCH_H
#define CMAKEDEMO_PCH_H

// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include "include/android/SkAnimatedImage.h"
#include "include/codec/SkAndroidCodec.h"
#include "include/core/SkBlendMode.h"
#include "include/core/SkBlurTypes.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkData.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkEncodedImageFormat.h"
#include "include/core/SkFilterQuality.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkM44.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathEffect.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkRRect.h"
#include "include/core/SkScalar.h"
#include "include/core/SkShader.h"
#include "include/core/SkString.h"
#include "include/core/SkStrokeRec.h"
#include "include/core/SkSurface.h"
#include "include/core/SkSurfaceProps.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkTypes.h"
#include "include/core/SkVertices.h"
#include "include/effects/SkCornerPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkDiscretePathEffect.h"
#include "include/effects/SkGradientShader.h"
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkRuntimeEffect.h"
#include "include/effects/SkTrimPathEffect.h"
#include "include/utils/SkParsePath.h"
#include "include/utils/SkRandom.h"
#include "include/utils/SkShadowUtils.h"
#include "modules/skshaper/include/SkShaper.h"
//#include "src/core/SkFontMgrPriv.h"
#include "src/core/SkPathPriv.h"
#include "src/core/SkResourceCache.h"
#include "src/sksl/SkSLCompiler.h"

#include <iostream>
#include <string>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <SDL.h>

#ifdef SK_GL
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/gl/GrGLTypes.h"

#include <GLES3/gl3.h>
#include <emscripten/html5.h>
#endif

#ifndef SK_NO_FONTS
//#include "include/core/SkFont.h"
//#include "include/core/SkFontMgr.h"
//#include "include/core/SkFontTypes.h"
#endif

#ifdef SK_INCLUDE_PARAGRAPH
#include "modules/skparagraph/include/Paragraph.h"
#endif

#ifdef SK_INCLUDE_PATHOPS
#include "include/pathops/SkPathOps.h"
#endif

#ifndef SK_NO_FONTS
//sk_sp<SkFontMgr> SkFontMgr_New_Custom_Data(const uint8_t** datas, const size_t* sizes, int n);
#endif

#include <Utilities.h>

#undef main

#endif //PCH_H

#endif //CMAKEDEMO_PCH_H
