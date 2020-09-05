//
// Created by milan on 9/5/2020.
//

#ifndef CMAKEDEMO_APP_H
#define CMAKEDEMO_APP_H

#include "pch.h"

class App {
public:
    explicit App();

    void Run();
    void MainLoop();

    std::function<void()> Loop;

private:
    void HandleEvents();
    void SkiaSDLInit();
    void SkiaSDLLoop();

    SkTArray<SkRect> fRects = {};
    SDL_Window* window = nullptr;
    sk_sp<GrDirectContext> grContext;
    sk_sp<SkSurface> surface;
    sk_sp<SkSurface> cpuSurface;
    sk_sp<SkImage> image;
    SkPaint paint;
    SkFont font;
    float rotation = 0;
    int viewWidth = 0;
    int viewHeight = 0;
    bool fQuit = false;
    int msaaSampleCount = 0; // 4
    int stencilBits = 8;
};


#endif //CMAKEDEMO_APP_H
