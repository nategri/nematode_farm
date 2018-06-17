#ifndef MUSCLE_DISPLAY_H
#define MUSCLE_DISPLAY_H

#include "SDL.h"
#include "worm.h"

typedef struct {
  SDL_Rect rect;
  uint8_t rgba[4];
} MuscleDisplayCell;

typedef struct {
  MuscleDisplayCell left_d_cell[19];
  MuscleDisplayCell left_v_cell[19];
  MuscleDisplayCell right_d_cell[19];
  MuscleDisplayCell right_v_cell[19];
} MuscleDisplay;

void muscle_display_init(MuscleDisplay* const);
void muscle_display_update(MuscleDisplay* const, Worm* const);
void muscle_display_draw(SDL_Renderer*, MuscleDisplay* const);

#endif
