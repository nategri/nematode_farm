#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "SDL.h"
#include "SDL_ttf.h"

typedef struct {
  SDL_Rect rect;

  int16_t x_center;
  int16_t y_center;

  TTF_Font* font;

  SDL_Color color;
  SDL_Texture* tex;

} TextBox;

void text_box_init(TextBox* const, int16_t, int16_t, const char*, uint8_t);
void text_box_draw(SDL_Renderer*, TextBox* const, const char*);

#endif
