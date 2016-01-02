// cc main.c -lSDL2 -lGLEW -lGL -lm -o app

#include <math.h>
#include <stdio.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#define QFPC_IMPLEMENTATION
#include "../../qfpc.h"

#include "torus.h"

void CreatePerspectiveProjection(float * proj, float aspect, float fov_y_rad, float n, float f);
float GetAspectRatio(SDL_Window * sdl_window);

GLuint CreateProgram(const char * vert_filepath, const char * frag_filepath);
GLuint CreateShader(GLenum shader_type, const char * shader_filepath);

int main()
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  
  SDL_Window * sdl_window = SDL_CreateWindow(
    "quatFirstPersonCamera",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    800, 600,
    SDL_WINDOW_OPENGL
  );

  SDL_GLContext sdl_glcontext = SDL_GL_CreateContext(sdl_window);
  SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);

  glewExperimental = GL_TRUE;
  GLenum glew_status = glewInit();
  if(glew_status)
  {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }
  glGetError();
  
  GLuint program = CreateProgram("vs.vert", "fs.frag");
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint mesh_bo;
  glGenBuffers(1, &mesh_bo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh_bo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glClearColor(168 / 255.f, 204 / 255.f, 207 / 255.f, 1.f);

  float cam_pos[3] = {};
  float cam_rot[4] = { [3] = 1.f };
  float cam_prj[4] = {};

  CreatePerspectiveProjection(
    cam_prj, GetAspectRatio(sdl_window),
    85.0f * QFPC_TO_RAD,
    0.01f, 1000.f
  );

  SDL_ShowCursor(0);
  int win_w, win_h;
  SDL_GetWindowSize(sdl_window, &win_w, &win_h);
  SDL_WarpMouseInWindow(sdl_window, win_w/2, win_h/2);

  while(1)
  {
    SDL_PumpEvents();
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    const Uint8 * key = SDL_GetKeyboardState(NULL);
    SDL_WarpMouseInWindow(sdl_window, win_w/2, win_h/2);

    quatFirstPersonCamera(
      cam_pos, cam_rot,
      mouse_x, mouse_y,
      win_w/2, win_h/2,
      0.1f, 0.05f,
      key[SDL_SCANCODE_W],
      key[SDL_SCANCODE_A],
      key[SDL_SCANCODE_S],
      key[SDL_SCANCODE_D],
      key[SDL_SCANCODE_E],
      key[SDL_SCANCODE_Q]
    );

    glProgramUniform3fv(program, 0, 1, cam_pos);
    glProgramUniform4fv(program, 1, 1, cam_rot);
    glProgramUniform4fv(program, 2, 1, cam_prj);

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case(SDL_QUIT): goto exit;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glUseProgram(program);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(mesh) / sizeof(float) / 3);

    SDL_GL_SwapWindow(sdl_window);
  }

  exit:

  SDL_GL_DeleteContext(sdl_glcontext);
  SDL_Quit();
  
  return 0;
}

void CreatePerspectiveProjection(
  float * proj,
  float aspect,
  float fov_y_rad,
  float n,
  float f)
{
  float d = 1.f / tanf(fov_y_rad * 0.5f);
  proj[0] = d / aspect;
  proj[1] = d;
  proj[2] = (n + f) / (n - f);
  proj[3] = (2.f * n * f) / (n - f);
}

float GetAspectRatio(SDL_Window * sdl_window)
{
  int w, h;
  SDL_GetWindowSize(sdl_window, &w, &h);
  return w / (float)h;
}

GLuint CreateProgram(const char * vert_filepath, const char * frag_filepath)
{
  GLuint vert = CreateShader(GL_VERTEX_SHADER, vert_filepath);
  GLuint frag = CreateShader(GL_FRAGMENT_SHADER, frag_filepath);
  
  GLuint program = glCreateProgram();
  
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  
  glLinkProgram(program);
  
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if(status == GL_FALSE)
  {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    GLchar log[length];
    glGetProgramInfoLog(program, length, NULL, log);
    printf("Program link error: %s\n", log);
    return 0;
  }
  
  glDetachShader(program, vert);
  glDetachShader(program, frag);
  
  return program;
}

GLuint CreateShader(GLenum shader_type, const char * shader_filepath)
{
  GLuint shader = glCreateShader(shader_type);

  FILE * fd = fopen(shader_filepath, "rb");

  if(fd == NULL)
  {
    fprintf(stderr, "Can't open file: %s\n", shader_filepath);
    return 0;
  }

  fseek(fd, 0, SEEK_END);
  long int bytes = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char src[bytes + 1];
  src[bytes] = 0;
  fread(&src, bytes, 1, fd);

  fclose(fd);

  char * shader_str = &src[0];
  glShaderSource(shader, 1, (const char **)&shader_str, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(status == GL_FALSE)
  {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar log[length];
    glGetShaderInfoLog(shader, length, NULL, log);
    char shader_type_str[8];
    switch(shader_type)
    {
      case GL_VERTEX_SHADER:   sprintf(shader_type_str, "vertex");   break;
      case GL_FRAGMENT_SHADER: sprintf(shader_type_str, "fragment"); break;
    }
    fprintf(stderr, "Compile error in %s shader:\n%s\n", shader_type_str, log);
    return 0;
  }

  return shader;
}
