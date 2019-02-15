#ifndef MOTION_COMPONENT_DISPLAY_H
#define MOTION_COMPONENT_DISPLAY_H

#include "SDL.h"
#include "worm.h"

typedef struct {
  // Lower left anchor points for plot
  uint16_t x_anchor;
  uint16_t y_anchor;
  // Rectangles that comprise plot
  SDL_Rect left_body;
  SDL_Rect right_body;
  SDL_Rect left_neck;
  SDL_Rect right_neck;
  // Colors
  uint8_t color_body[4];
  uint8_t color_neck[4];
} MotionComponentDisplay;

void motion_component_display_init(MotionComponentDisplay* const);
void motion_component_display_update(MotionComponentDisplay* const, Worm* const);
void motion_component_display_draw(SDL_Renderer*, MotionComponentDisplay* const);

#endif
