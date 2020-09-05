#include <App.h>
#include "pch.h"

// Shader sources
const GLchar* vertexSource =
        "attribute vec4 position;                     \n"
        "void main()                                  \n"
        "{                                            \n"
        "  gl_Position = vec4(position.xyz, 1.0);     \n"
        "}                                            \n";
const GLchar* fragmentSource =
        "precision mediump float;\n"
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor[0] = gl_FragCoord.x/640.0;    \n"
        "  gl_FragColor[1] = gl_FragCoord.y/480.0;    \n"
        "  gl_FragColor[2] = 0.5;                     \n"
        "}                                            \n";

// TODO: This is dirty as hell, this HAS to be fixed somehow
App app = App();

//std::function<void()> loop;
void main_loop() { app.MainLoop(); }

//void glExample(int width, int height, void (*draw)(SkCanvas*), const char* path)
//{
//    // You've already created your OpenGL context and bound it.
//    sk_sp<const GrGLInterface> *interface = nullptr;
//    // Leaving interface as null makes Skia extract pointers to OpenGL functions for the current
//    // context in a platform-specific way. Alternatively, you may create your own GrGLInterface and
//    // initialize it however you like to attach to an alternate OpenGL implementation or intercept
//    // Skia's OpenGL calls.
//    sk_sp<GrContext> context = GrContext::MakeGL(*interface);
//    std::cout << "1" << std::endl;
//    SkImageInfo info = SkImageInfo:: MakeN32Premul(width, height);
//    std::cout << "2" << std::endl;
//    sk_sp<SkSurface> gpuSurface(SkSurface::MakeRenderTarget(context.get(), SkBudgeted::kNo, info));
//    std::cout << "3" << std::endl;
//    if (!gpuSurface)
//    {
//        SkDebugf("SkSurface::MakeRenderTarget returned null\n");
//        return;
//    }
//    SkCanvas* gpuCanvas = gpuSurface->getCanvas();
//    draw(gpuCanvas);
//    sk_sp<SkImage> img(gpuSurface->makeImageSnapshot());
//    if (!img) { return; }
//    sk_sp<SkData> png(img->encodeToData());
//    if (!png) { return; }
//    SkFILEWStream out(path);
//    (void)out.write(png->data(), png->size());
//}
//
//void example(SkCanvas* canvas) {
//    std::cout << "Starting to draw..." << std::endl;
//    const SkScalar scale = 256.0f;
//    const SkScalar R = 0.45f * scale;
//    const SkScalar TAU = 6.2831853f;
//    SkPath path;
//    for (int i = 0; i < 5; ++i) {
//        SkScalar theta = 2 * i * TAU / 5;
//        if (i == 0) {
//            path.moveTo(R * cos(theta), R * sin(theta));
//        } else {
//            path.lineTo(R * cos(theta), R * sin(theta));
//        }
//    }
//    path.close();
//    SkPaint p;
//    p.setAntiAlias(true);
//    canvas->clear(SK_ColorWHITE);
//    canvas->translate(0.5f * scale, 0.5f * scale);
//    canvas->drawPath(path, p);
//    std::cout << "Finished drawing..." << std::endl;
//}

int main()
{
    emscripten_set_main_loop(main_loop , 0, true);

//    SDL_Window *window;
//    SDL_CreateWindowAndRenderer(640, 480, 0, &window, nullptr);
//
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

//    // Create a Vertex Buffer Object and copy the vertex data to it
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//
//    GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    // Create and compile the vertex shader
//    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
//    glCompileShader(vertexShader);
//
//    // Create and compile the fragment shader
//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
//    glCompileShader(fragmentShader);
//
//    // Link the vertex and fragment shader into a shader program
//    GLuint shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//    glUseProgram(shaderProgram);
//
//    // Specify the layout of the vertex data
//    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
//    glEnableVertexAttribArray(posAttrib);
//    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

//    loop = [&]
//    {
//        // move a vertex
//        const uint32_t milliseconds_since_start = SDL_GetTicks();
//        const uint32_t milliseconds_per_loop = 3000;
//        vertices[0] = static_cast<float>( milliseconds_since_start % milliseconds_per_loop ) / float(milliseconds_per_loop) - 0.5f;
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//        // Clear the screen
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        // Draw a triangle from the 3 vertices
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//
//        SDL_GL_SwapWindow(window);
//    };

    //glExample( 256, 256, example, "out_gpu.png"    );

    return EXIT_SUCCESS;
}

