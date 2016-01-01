**Description**: Single-file C implementation of first-person quaternion camera used in such video game genres like FPS.

**License**: Public domain

**Language**: C

**Dependencies**: libm (can be omitted if custom `QFPC_SIN` and `QFPC_COS` are defined)

Example for SDL2:
-----------------

```c
#define QFPC_IMPLEMENTATION
#include "qfpc.h"

int main()
{
  /* Application init */
  
  /* float cam_pos[3] init */
  /* float cam_rot[4] init */

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

    /* Now cam_pos and cam_rot can be passed to a shader */

    /* The rest of application */
  }
}
```

Example for GLFW3:
------------------

```c
#define QFPC_IMPLEMENTATION
#include "qfpc.h"

int main()
{
  /* Application init */
  
  /* float cam_pos[3] init */
  /* float cam_rot[4] init */

  glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  int win_w, win_h;
  glfwGetWindowSize(glfw_window, &win_w, &win_h);
  for(int i = 0; i < 2; ++i)
  {
    glfwPollEvents();
    glfwSetCursorPos(glfw_window, win_w/2, win_h/2);
  }

  while(!glfwWindowShouldClose(glfw_window))
  {
    glfwPollEvents();
    double mouse_x, mouse_y;
    glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);
    glfwSetCursorPos(glfw_window, win_w/2, win_h/2);

    quatFirstPersonCamera(
      cam_pos, cam_rot,
      mouse_x, mouse_y,
      win_w/2, win_h/2,
      0.1f, 0.05f,
      glfwGetKey(glfw_window, GLFW_KEY_W),
      glfwGetKey(glfw_window, GLFW_KEY_A),
      glfwGetKey(glfw_window, GLFW_KEY_S),
      glfwGetKey(glfw_window, GLFW_KEY_D),
      glfwGetKey(glfw_window, GLFW_KEY_E),
      glfwGetKey(glfw_window, GLFW_KEY_Q)
    );

    /* Now cam_pos and cam_rot can be passed to a shader */

    /* The rest of application */
  }
}
```
