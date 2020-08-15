/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "pch.h"

extern "C"
{
	bool gCheckErrorGL = false;
	bool gLogCallsGL = false;
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


struct ApplicationState {
	ApplicationState() : fQuit(false) {}

  void skia_sdl_init()
  {
    int viewWidth, viewHeight;
    SDL_GL_GetDrawableSize(window, &viewWidth, &viewHeight);

    glViewport(0, 0, viewWidth, viewHeight);
    glClearColor(1, 1, 1, 1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    puts("setup GrContext");

    // setup GrContext
    auto interface = GrGLMakeNativeInterface();

    // setup contexts
  #ifdef __EMSCRIPTEN__
    grContext = GrContext::MakeGL(interface);
  #else
    grContext = GrDirectContext::MakeGL(interface);
  #endif
    SkASSERT(grContext);

    // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
    // render to it
    GrGLint buffer;
    GR_GL_GetIntegerv(interface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint)buffer;

    int contextType;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    SkColorType colorType;

  #ifdef __EMSCRIPTEN__
    info.fFormat = GL_RGBA8;
    colorType = kRGBA_8888_SkColorType;
  #else
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
  #endif

    printf("Using color type %d\n", colorType);

    target = new GrBackendRenderTarget(viewWidth, viewHeight, kMsaaSampleCount, kStencilBits, info);
    printf("Created render target %dx%d -> %d\n", viewWidth, viewHeight, target.isValid());

  #ifdef __EMSCRIPTEN__
    surface = SkSurface::MakeFromBackendRenderTarget(
      grContext.get(), target,
      kBottomLeft_GrSurfaceOrigin,
      colorType, nullptr, nullptr);
  #else
    // setup SkSurface
    // To use distance field text, use commented out SkSurfaceProps instead
    // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
    // 	SkSurfaceProps::kLegacyFontHost_InitType);
    SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);

   surface = SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
      kBottomLeft_GrSurfaceOrigin,
      colorType, nullptr, &props);
  #endif
    printf("Created render surface -> %p\n", surface.get());

    canvas = surface->getCanvas();
    // canvas->scale((float)dw / displayMode.w, (float)dh / displayMode.h);

    const char* helpMessage = "Click and drag to create rects.  Press esc to quit.";

    paint.setAntiAlias(true);

    // create a surface for CPU rasterization
    sk_sp<SkSurface> cpuSurface(SkSurface::MakeRaster(canvas->imageInfo()));

    printf("Created offscreen CPU surface -> %p", cpuSurface.get());

    SkCanvas* offscreen = cpuSurface->getCanvas();
    offscreen->save();
    offscreen->translate(50.0f, 50.0f);
    offscreen->drawPath(create_star(), paint);
    offscreen->restore();

    image = cpuSurface->makeImageSnapshot();

    font.setSize(24);
  }


	// Storage for the user created rectangles. The last one may still be being edited.
	SkTArray<SkRect> fRects;
	bool fQuit;

#ifdef __EMSCRIPTEN__
  sk_sp<GrContext> grContext;
#else
  sk_sp<GrDirectContext> grContext;
#endif

  SkPaint paint;
  SkScalar rotation = 0;
  SkFont font;

  sk_sp<SkCanvas> canvas;
  sk_sp<GrBackendRenderTarget> target;
  sk_sp<SkSurface> surface;
  sk_sp<SkImage> image;
  SDL_Window* window;
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

void skia_sdl_loop() {

  puts("Looping...");

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

	window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED,
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

  skia_sdl_init();

  puts("Starting...");
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(skia_sdl_loop, 0, true);
#else
	while (!state.fQuit) { // Our application loop
  	skia_sdl_loop();
  }
#endif

  puts("Exiting...");

	if (glContext) {
		SDL_GL_DeleteContext(glContext);
	}

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
	return 0;
}