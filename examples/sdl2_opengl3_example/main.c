// cc main.c -lSDL2 -lGLEW -lGL -o app

#include <GL/glew.h>
#include <SDL2/SDL.h>

GLuint CreateProgram(const char * vert_filepath, const char * frag_filepath);
GLuint CreateShader(GLenum shader_type, const char * shader_filepath);

int main()
{
  SDL_Init(SDL_INIT_VIDEO);
  
  SDL_Window * sdl_window = SDL_CreateWindow(
    "quatFirstPersonCamera",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    800, 600,
    SDL_WINDOW_OPENGL
  );

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);  
  
  SDL_GLContext sdl_glcontext = SDL_GL_CreateContext(sdl_window);
  
  glewExperimental = GL_TRUE;
  GLenum glew_status = glewInit();
  if(glew_status)
  {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }
  glGetError();
  
  float triangle[] = {
    // Vertex positions
    0.0f,  0.5f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.0f,  1.0f,
   -0.5f, -0.5f,  0.0f,  1.0f,
    // Vertex colors
    1.0f,  0.0f,  0.0f,  1.0f,
    0.0f,  1.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,  1.0f   
  };
  
  GLuint program = CreateProgram("vs.vert", "fs.frag");
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint triangle_bo;
  glGenBuffers(1, &triangle_bo);
  glBindBuffer(GL_ARRAY_BUFFER, triangle_bo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)48);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  while(1)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case(SDL_QUIT): goto exit;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(sdl_window);
  }

  exit:

  SDL_GL_DeleteContext(sdl_glcontext);
  SDL_Quit();
  
  return 0;
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
