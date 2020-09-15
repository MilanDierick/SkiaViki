#include "pch.h"
#include <App.h>

// TODO: This is dirty as hell, this HAS to be fixed somehow
App app = App();

void main_loop() { app.MainLoop(); }

int main()
{
    emscripten_set_main_loop(main_loop , 0, true);

    return EXIT_SUCCESS;
}

