/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "SDL.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkSurface.h"
#include "include/utils/SkRandom.h"

#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/gl/GrGLUtil.h"

#define GL_GLEXT_PROTOTYPES 1

#if defined(SK_BUILD_FOR_WASM)
#   include <emscripten.h>
#   include <emscripten.h>
#   include <GL/gl.h>
#   include <emscripten/html5.h>
#   include <SDL_opengles2.h>
#elif defined(SK_BUILD_FOR_ANDROID)
#   include <GLES/gl.h>
#elif defined(SK_BUILD_FOR_UNIX)
#   include <GL/gl.h>
#elif defined(SK_BUILD_FOR_MAC)
#   include <OpenGL/gl.h>
#elif defined(SK_BUILD_FOR_IOS)
#   include <OpenGLES/ES2/gl.h>
#endif

sk_sp<SkSurface> MakeOnScreenGLSurface(sk_sp<GrContext> grContext, int width, int height) {
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    // Wrap the frame buffer object attached to the screen in a Skia render
    // target so Skia can render to it
    GrGLint buffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint) buffer;
    SkColorType colorType;

    info.fFormat = GL_RGBA8;
    colorType = kRGBA_8888_SkColorType;

    GrBackendRenderTarget target(width, height, 0, 8, info);

    sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
                                                                    kBottomLeft_GrSurfaceOrigin,
                                                                    colorType, nullptr, nullptr));
    return surface;
}

sk_sp<SkSurface> MakeRenderTarget(sk_sp<GrContext> grContext, int width, int height) {
    SkImageInfo info = SkImageInfo::MakeN32(width, height, SkAlphaType::kPremul_SkAlphaType);

    sk_sp<SkSurface> surface(SkSurface::MakeRenderTarget(grContext.get(),
                             SkBudgeted::kYes,
                             info, 0,
                             kBottomLeft_GrSurfaceOrigin,
                             nullptr, true));
    return surface;
}

sk_sp<SkSurface> MakeRenderTarget(sk_sp<GrContext> grContext, const SkImageInfo& sii) {
    sk_sp<SkSurface> surface(SkSurface::MakeRenderTarget(grContext.get(),
                             SkBudgeted::kYes,
                             sii, 0,
                             kBottomLeft_GrSurfaceOrigin,
                             nullptr, true));
    return surface;
}

sk_sp<GrContext> MakeGrContext(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context)
{
    EMSCRIPTEN_RESULT r = emscripten_webgl_make_context_current(context);
    if (r < 0) {
        SkDebugf("failed to make webgl context current %d\n", r);
        return nullptr;
    }
    // setup GrContext
    auto interface = GrGLMakeNativeInterface();
    // setup contexts
    sk_sp<GrContext> grContext(GrContext::MakeGL(interface));
    return grContext;
}


/*
 * This application is a simple example of how to combine SDL and Skia it demonstrates:
 *   how to setup gpu rendering to the main window
 *   how to perform cpu-side rendering and draw the result to the gpu-backed screen
 *   draw simple primitives (rectangles)
 *   draw more complex primitives (star)
 */

// If you want multisampling, uncomment the below lines and set a sample count
static const int kMsaaSampleCount = 0; //4;

// // Skia needs 8 stencil bits
static const int kStencilBits = 8;  

struct ApplicationState {
	ApplicationState() : fQuit(false) {}
	// Storage for the user created rectangles. The last one may still be being edited.
	SkTArray<SkRect> fRects;
	bool fQuit;
};

static void handle_error() {
	const char* error = SDL_GetError();
	SkDebugf("SDL Error: %s\n", error);
	SDL_ClearError();
}

static void handle_events(ApplicationState* state, SkCanvas* canvas) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_MOUSEMOTION:
			if (event.motion.state == SDL_PRESSED) {
				SkRect& rect = state->fRects.back();
				rect.fRight = static_cast<SkScalar>(event.motion.x);
				rect.fBottom = static_cast<SkScalar>(event.motion.y);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.state == SDL_PRESSED) {
				state->fRects.push_back() = SkRect::MakeLTRB(SkIntToScalar(event.button.x),
					SkIntToScalar(event.button.y),
					SkIntToScalar(event.button.x),
					SkIntToScalar(event.button.y));
			}
			break;
		case SDL_KEYDOWN: {
			SDL_Keycode key = event.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				state->fQuit = true;
			}
			break;
		}
		case SDL_QUIT:
			state->fQuit = true;
			break;
		default:
			break;
		}
	}
}

// Creates a star type shape using a SkPath
static SkPath create_star() {
	static const int kNumPoints = 5;
	SkPath concavePath;
	SkPoint points[kNumPoints] = { {0, SkIntToScalar(-50)} };
	SkMatrix rot;
	rot.setRotate(SkIntToScalar(360) / kNumPoints);
	for (int i = 1; i < kNumPoints; ++i) {
		rot.mapPoints(points + i, points + i - 1, 1);
	}
	concavePath.moveTo(points[0]);
	for (int i = 0; i < kNumPoints; ++i) {
		concavePath.lineTo(points[(2 * i) % kNumPoints]);
	}
	concavePath.setFillType(SkPathFillType::kEvenOdd);
	SkASSERT(!concavePath.isConvex());
	concavePath.close();
	return concavePath;
}

void skia_sdl_loop(SDL_Window* window)
{
	int viewWidth, viewHeight;
	SDL_GL_GetDrawableSize(window, &viewWidth, &viewHeight);

	glViewport(0, 0, viewWidth, viewHeight);
	glClearColor(1, 1, 1, 1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// setup GrContext
	auto interface = GrGLMakeNativeInterface();

	// setup contexts
	sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(interface));
	SkASSERT(grContext);

	// Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
	// render to it
	GrGLint buffer;
	GR_GL_GetIntegerv(interface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
	GrGLFramebufferInfo info;
	info.fFBOID = (GrGLuint)buffer;
	SkColorType colorType;

	int contextType;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

	//SkDebugf("%s", SDL_GetPixelFormatName(windowFormat));
	// TODO: the windowFormat is never any of these?
	uint32_t windowFormat = SDL_GetWindowPixelFormat(window);
	if (SDL_PIXELFORMAT_RGBA8888 == windowFormat) {
		info.fFormat = GR_GL_RGBA8;
		colorType = kRGBA_8888_SkColorType;
	}
	else {
		colorType = kBGRA_8888_SkColorType;
		if (SDL_GL_CONTEXT_PROFILE_ES == contextType) {
			info.fFormat = GR_GL_BGRA8;
		}
		else {
			// We assume the internal format is RGBA8 on desktop GL
			info.fFormat = GR_GL_RGBA8;
		}
	}

	GrBackendRenderTarget target(viewWidth, viewHeight, kMsaaSampleCount, kStencilBits, info);

	// setup SkSurface
	// To use distance field text, use commented out SkSurfaceProps instead
	// SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
	// 	SkSurfaceProps::kLegacyFontHost_InitType);
	SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);

	sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
		kBottomLeft_GrSurfaceOrigin,
		colorType, nullptr, &props));

	SkCanvas* canvas = surface->getCanvas();
	// canvas->scale((float)dw / displayMode.w, (float)dh / displayMode.h);

	ApplicationState state;

	const char* helpMessage = "Click and drag to create rects.  Press esc to quit.";

	SkPaint paint;
	paint.setAntiAlias(true);

	// create a surface for CPU rasterization
	sk_sp<SkSurface> cpuSurface(SkSurface::MakeRaster(canvas->imageInfo()));

	SkCanvas* offscreen = cpuSurface->getCanvas();
	offscreen->save();
	offscreen->translate(50.0f, 50.0f);
	offscreen->drawPath(create_star(), paint);
	offscreen->restore();

	sk_sp<SkImage> image = cpuSurface->makeImageSnapshot();

	SkScalar rotation = 0;
	SkFont font;
	font.setSize(24);
	
	while (!state.fQuit) { // Our application loop
		SkRandom rand;
		canvas->clear(SK_ColorWHITE);
		handle_events(&state, canvas);

		paint.setColor(SK_ColorBLACK);
		canvas->drawString(helpMessage, 100.0f, 100.0f, font, paint);
		for (int i = 0; i < state.fRects.count(); i++) {
			paint.setColor(rand.nextU() | 0x44808080);
			canvas->drawRect(state.fRects[i], paint);
		}

		// draw offscreen canvas
		canvas->save();
		canvas->translate(float(viewWidth) / 2, float(viewHeight) / 2);
		canvas->rotate(rotation++);
		canvas->drawImage(image, -50.0f, -50.0f);
		canvas->restore();

		canvas->flush();
		SDL_GL_SwapWindow(window);
	}
}

#if defined(SK_BUILD_FOR_ANDROID)
int SDL_main(int argc, char** argv) {
#else
extern "C" int __cdecl main(int argc, char** argv) {
#endif
	uint32_t windowFlags = 0;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GLContext glContext = nullptr;
#if defined(SK_BUILD_FOR_ANDROID) || defined(SK_BUILD_FOR_IOS)
	// For Android/iOS we need to set up for OpenGL ES and we make the window hi res & full screen
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP |
		SDL_WINDOW_ALLOW_HIGHDPI;
#else
	// For all other clients we use the core profile and operate in a window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, kMsaaSampleCount);

	/*
	 * In a real application you might want to initialize more subsystems
	 */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		handle_error();
		return 1;
	}

	// Setup window
	// This code will create a window with the half the resolution as the user's desktop.
	SDL_DisplayMode displayMode;
	if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) {
		handle_error();
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, displayMode.w/2, displayMode.h/2, windowFlags);

	if (!window) {
		handle_error();
		return 1;
	}

	// To go fullscreen
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	// try and setup a GL context
	glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		handle_error();
		return 1;
	}

	int success = SDL_GL_MakeCurrent(window, glContext);
	if (success != 0) {
		handle_error();
		return success;
	}

	skia_sdl_loop(window);

	if (glContext) {
		SDL_GL_DeleteContext(glContext);
	}

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
	return 0;
}
