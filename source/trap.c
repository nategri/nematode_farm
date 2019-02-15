#include "trap.h"

void trap_init(Trap* const trap, Color color, int16_t x_center, int16_t y_center, uint16_t w, uint16_t h) {
  trap->rect.x = x_center - (w/2);
  trap->rect.y = y_center - (h/2);
  trap->rect.w = w;
  trap->rect.h = h;

  trap->color = color;

  if(color == RED) {
    trap->rgba[0] = 255;
    trap->rgba[1] = 0;
    trap->rgba[2] = 0;
    trap->rgba[3] = 128;
  }
  else if(color == BLUE) {
    trap->rgba[0] = 0;
    trap->rgba[1] = 0;
    trap->rgba[2] = 255;
    trap->rgba[3] = 128;
  }

  trap->left_edge = x_center - (w/2);
  trap->right_edge = x_center + (w/2);
  trap->top_edge = y_center - (h/2);
  trap->bottom_edge = y_center + (h/2);
}

void trap_draw(SDL_Renderer* rend, Trap* const trap) {
  uint8_t* rgba = trap->rgba;
  SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
  SDL_RenderFillRect(rend, &(trap->rect));
  SDL_SetRenderDrawColor(rend, 128, 128, 128, 0);
}
