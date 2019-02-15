#ifndef TRAP_H
#define TRAP_H

#include "SDL.h"

#include "colors.h"

typedef struct {
  SDL_Rect rect;
  Color color;
  uint8_t rgba[4];

  int left_edge;
  int right_edge;
  int top_edge;
  int bottom_edge;
} Trap;

void trap_init(Trap* const, Color color, int16_t, int16_t, uint16_t, uint16_t);
void trap_draw(SDL_Renderer*, Trap* const);

#endif
