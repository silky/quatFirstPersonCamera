**License**: Public domain

**Language**: C

**Dependencies**: libm (can be omitted if `QFPC_SIN` and `QFPC_COS` are defined with custom functions)

Tested only on Linux so far.

An example for SDL2:

```c
#define QFPC_IMPLEMENTATION
#include "qfpc.h"

int main()
{
  /* SDL2 initialization ... */
  
  /* vec3 cam_pos = init ... */
  /* quat cam_rot = init ... */

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

    /* Now cam_pos and cam_rot are ready to be used in GLSL */

    /* The rest of application loop ... */
  }
}
```
