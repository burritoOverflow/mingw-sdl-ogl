#include "gl_loader.h"

#include <stdio.h>

#define LOAD(type, name)                                                       \
  name = (type)SDL_GL_GetProcAddress(#name);                                   \
  if (!name) {                                                                 \
    fprintf(stderr, "Failed to load %s\n", #name);                             \
    return 0;                                                                  \
  }

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

int load_gl_functions(void) {
  LOAD(PFNGLCREATESHADERPROC, glCreateShader)
  LOAD(PFNGLSHADERSOURCEPROC, glShaderSource)
  LOAD(PFNGLCOMPILESHADERPROC, glCompileShader)
  LOAD(PFNGLGETSHADERIVPROC, glGetShaderiv)
  LOAD(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)
  LOAD(PFNGLCREATEPROGRAMPROC, glCreateProgram)
  LOAD(PFNGLATTACHSHADERPROC, glAttachShader)
  LOAD(PFNGLLINKPROGRAMPROC, glLinkProgram)
  LOAD(PFNGLUSEPROGRAMPROC, glUseProgram)
  LOAD(PFNGLDELETESHADERPROC, glDeleteShader)
  LOAD(PFNGLGENBUFFERSPROC, glGenBuffers)
  LOAD(PFNGLBINDBUFFERPROC, glBindBuffer)
  LOAD(PFNGLBUFFERDATAPROC, glBufferData)
  LOAD(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)
  LOAD(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)
  LOAD(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
  LOAD(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)
  return 1;
}
