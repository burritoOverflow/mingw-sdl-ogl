#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

/* ── OpenGL function pointer types ──
   SDL_opengl_glext.h (pulled in by SDL_opengl.h) already typedefs the
   standard PFNGL* names, so we just use those directly rather than
   redefining them and getting conflicting-type errors.             ── */

#define LOAD(type, name) \
    type name = (type)SDL_GL_GetProcAddress(#name); \
    if (!name) { fprintf(stderr, "Failed to load " #name "\n"); return 1; }

/* ── Shaders ── */
static const char *VERT_SRC =
    "#version 330 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "out vec3 vColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    vColor = aColor;\n"
    "}\n";

static const char *FRAG_SRC =
    "#version 330 core\n"
    "in vec3 vColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(vColor, 1.0);\n"
    "}\n";

/* ── Vertex data: position (x,y) + color (r,g,b) ── */
static const float VERTS[] = {
/*   x      y      r     g     b  */
     0.0f,  0.6f,  1.0f, 0.2f, 0.2f,   /* top    – red   */
    -0.6f, -0.4f,  0.2f, 1.0f, 0.2f,   /* left   – green */
     0.6f, -0.4f,  0.2f, 0.2f, 1.0f,   /* right  – blue  */
};

static GLuint compile_shader(
    PFNGLCREATESHADERPROC glCreateShader,
    PFNGLSHADERSOURCEPROC glShaderSource,
    PFNGLCOMPILESHADERPROC glCompileShader,
    PFNGLGETSHADERIVPROC glGetShaderiv,
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog,
    GLenum type, const char *src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, sizeof log, NULL, log);
        fprintf(stderr, "Shader error:\n%s\n", log);
        return 0;
    }
    return s;
}

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* SDL3: no x/y position params */
    SDL_Window *win = SDL_CreateWindow("OpenGL Triangle", 800, 600,
                                       SDL_WINDOW_OPENGL);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) {
        fprintf(stderr, "SDL_GL_CreateContext: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GL_SetSwapInterval(1); /* vsync */

    /* ── Load GL function pointers ── */
    LOAD(PFNGLCREATESHADERPROC,           glCreateShader)
    LOAD(PFNGLSHADERSOURCEPROC,           glShaderSource)
    LOAD(PFNGLCOMPILESHADERPROC,          glCompileShader)
    LOAD(PFNGLGETSHADERIVPROC,            glGetShaderiv)
    LOAD(PFNGLGETSHADERINFOLOGPROC,       glGetShaderInfoLog)
    LOAD(PFNGLCREATEPROGRAMPROC,          glCreateProgram)
    LOAD(PFNGLATTACHSHADERPROC,           glAttachShader)
    LOAD(PFNGLLINKPROGRAMPROC,            glLinkProgram)
    LOAD(PFNGLUSEPROGRAMPROC,             glUseProgram)
    LOAD(PFNGLDELETESHADERPROC,           glDeleteShader)
    LOAD(PFNGLGENBUFFERSPROC,             glGenBuffers)
    LOAD(PFNGLBINDBUFFERPROC,             glBindBuffer)
    LOAD(PFNGLBUFFERDATAPROC,             glBufferData)
    LOAD(PFNGLGENVERTEXARRAYSPROC,        glGenVertexArrays)
    LOAD(PFNGLBINDVERTEXARRAYPROC,        glBindVertexArray)
    LOAD(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
    LOAD(PFNGLVERTEXATTRIBPOINTERPROC,    glVertexAttribPointer)

    GLuint vert = compile_shader(glCreateShader, glShaderSource, glCompileShader,
                                  glGetShaderiv, glGetShaderInfoLog,
                                  GL_VERTEX_SHADER, VERT_SRC);
    GLuint frag = compile_shader(glCreateShader, glShaderSource, glCompileShader,
                                  glGetShaderiv, glGetShaderInfoLog,
                                  GL_FRAGMENT_SHADER, FRAG_SRC);
    if (!vert || !frag) return 1;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glDeleteShader(vert);
    glDeleteShader(frag);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof VERTS, VERTS, GL_STATIC_DRAW);

    /* position: location 0, 2 floats, stride = 5 floats */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    /* color: location 1, 3 floats, offset = 2 floats */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    int running = 1;
    SDL_Event ev;
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = 0;
            if (ev.type == SDL_EVENT_KEY_DOWN &&
                ev.key.key == SDLK_ESCAPE)
                running = 0;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(win);
    }

    SDL_GL_DestroyContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

