//
// Created by milan on 9/5/2020.
//

#include "App.h"

App::App()
{
    SkiaSDLInit();
}

void App::Run()
{
    SkiaSDLInit();
    //SkiaSDLLoop();
}

void App::MainLoop()
{
    SkiaSDLLoop();
}

void App::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                if (event.motion.state == SDL_PRESSED) {
                    SkRect& rect = fRects.back();
                    rect.fRight = static_cast<SkScalar>(event.motion.x);
                    rect.fBottom = static_cast<SkScalar>(event.motion.y);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.state == SDL_PRESSED) {
                    fRects.push_back() = SkRect::MakeLTRB(SkIntToScalar(event.button.x),
                                                          SkIntToScalar(event.button.y),
                                                          SkIntToScalar(event.button.x),
                                                          SkIntToScalar(event.button.y));
                }
                break;
            case SDL_KEYDOWN: {
                SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    fQuit = true;
                }
                break;
            }
            case SDL_QUIT:
                fQuit = true;
                break;
            default:
                break;
        }
    }
}

void App::SkiaSDLInit()
{
    int contextType;
    SkColorType colorType;

    SDL_CreateWindowAndRenderer(640, 480, 0, &window, nullptr);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

//    SDL_GL_GetDrawableSize(window, &viewWidth, &viewHeight);
//
//    glViewport(0, 0, viewWidth, viewHeight);
//    glClearColor(1, 1, 1, 1);
//    glClearStencil(0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // setup GrContext
    sk_sp<const GrGLInterface> interface = GrGLMakeNativeInterface();

    // setup contexts
    grContext = GrDirectContext::MakeGL(interface);
    SkASSERT(grContext);

    // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
    // render to it
    GrGLint buffer;
    GR_GL_GetIntegerv(interface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint)buffer;

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

    GrBackendRenderTarget target(viewWidth, viewHeight, msaaSampleCount, stencilBits, info);

    // setup SkSurface
    // To use distance field text, use commented out SkSurfaceProps instead
    // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
    // 	SkSurfaceProps::kLegacyFontHost_InitType);
    SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);

    surface = SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
                                                     kBottomLeft_GrSurfaceOrigin,
                                                     colorType, nullptr, &props);

    SkCanvas* canvas = surface->getCanvas();
    // canvas->scale((float)dw / displayMode.w, (float)dh / displayMode.h);

    paint.setAntiAlias(true);

    // TODO: Use a GPU surface
    // create a surface for CPU rasterization
    cpuSurface = SkSurface::MakeRaster(canvas->imageInfo());

    SkCanvas* offscreen = cpuSurface->getCanvas();
    offscreen->save();
    offscreen->translate(50.0f, 50.0f);
    offscreen->drawPath(Utilities::CreateStar(), paint);
    offscreen->restore();

    image = cpuSurface->makeImageSnapshot();

    font.setSize(24);
}

void App::SkiaSDLLoop()
{
    SkCanvas* canvas = surface->getCanvas();
    const char* helpMessage = "Click and drag to create rects.  Press esc to quit.";

    while (!fQuit) { // Our application loop
        canvas->clear(SK_ColorWHITE);
        HandleEvents();

        paint.setColor(SK_ColorBLACK);

        canvas->drawString(helpMessage, 100.0f, 100.0f, font, paint);

        SkRandom rand;
        for (int i = 0; i < fRects.count(); i++) {
            paint.setColor(rand.nextU() | 0x44808080);
            canvas->drawRect(fRects[i], paint);
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
