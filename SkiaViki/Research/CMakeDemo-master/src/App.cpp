//
// Created by milan on 9/5/2020.
//

#include "App.h"

// Set the pixel format based on the colortype.
// These degrees of freedom are removed from canvaskit only to keep the interface simpler.
struct ColorSettings {
    explicit ColorSettings(uint32_t colorSpace) {
        if (colorSpace == SDL_PIXELFORMAT_UNKNOWN || colorSpace == SDL_PIXELFORMAT_RGBA8888) {
            colorType = kRGBA_8888_SkColorType;
            pixFormat = GL_RGBA8;
        } else {
            colorType = kRGBA_F16_SkColorType;
            pixFormat = GL_RGBA16F;
        }
    };

    SkColorType colorType;
    GrGLenum pixFormat;
};

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

// TODO: If it's impossible to create a working OpenGL backend using SDL, demand a WebGL backend from javascript
//  using 'emscripten_webgl_make_context_current(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE)'
void App::SkiaSDLInit()
{
    int contextType;

    SDL_CreateWindowAndRenderer(640, 480, 0, &window, nullptr);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    // setup GrContext
    auto interface = GrGLMakeNativeInterface();
    // setup contexts
    grContext = sk_sp<GrContext>(GrContext::MakeGL(interface));

    // WebGL should already be clearing the color and stencil buffers, but do it again here to
    // ensure Skia receives them in the expected state.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    grContext->resetContext(kRenderTarget_GrGLBackendState | kMisc_GrGLBackendState);

    // The on-screen canvas is FBO 0. Wrap it in a Skia render target so Skia can render to it.
    GrGLFramebufferInfo info;
    info.fFBOID = 0;

    GrGLint sampleCnt;
    glGetIntegerv(GL_SAMPLES, &sampleCnt);

    GrGLint stencil;
    glGetIntegerv(GL_STENCIL_BITS, &stencil);

    //SkDebugf("%s", SDL_GetPixelFormatName(windowFormat));
    // TODO: the windowFormat is never any of these?
    uint32_t windowFormat = SDL_GetWindowPixelFormat(window);

    const auto colorSettings = ColorSettings(windowFormat);
    info.fFormat = colorSettings.pixFormat;
    GrBackendRenderTarget target(width, height, sampleCnt, stencil, info);
    surface = sk_sp<SkSurface>(SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
                                                                    kBottomLeft_GrSurfaceOrigin, colorSettings.colorType, nullptr, nullptr));
}

void App::SkiaSDLLoop()
{
    SkCanvas* canvas = surface->getCanvas();
    const char* helpMessage = "Click and drag to create rects.  Press esc to quit.";

    while (!fQuit) { // Our application loop
        canvas->clear(SK_ColorWHITE);
        HandleEvents();

        paint.setColor(SK_ColorBLACK);

        //canvas->drawString(helpMessage, 100.0f, 100.0f, font, paint);

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

    SDL_GL_SwapWindow(window);
}
