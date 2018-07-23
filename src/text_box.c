#include "text_box.h"

void text_box_init(TextBox* const text_box, int16_t x_center, int16_t y_center, const char* font_file, uint8_t font_size) {
  text_box->x_center = x_center;
  text_box->y_center = y_center;

  text_box->color.r = 0;
  text_box->color.g = 0;
  text_box->color.b = 0;
  text_box->color.a = 0;

  text_box->font = TTF_OpenFont(font_file, font_size);
  text_box->tex = NULL;
}

void text_box_draw(SDL_Renderer* rend, TextBox* const text_box, const char* message) {
  SDL_Surface* surf = TTF_RenderText_Solid(text_box->font, message, text_box->color);

  if(text_box->tex != NULL) {
    SDL_DestroyTexture(text_box->tex);
  }
  text_box->tex = SDL_CreateTextureFromSurface(rend, surf);

  text_box->rect.w = surf->w;
  text_box->rect.h = surf->h;
  text_box->rect.x = text_box->x_center - (text_box->rect.w/2);
  text_box->rect.y = text_box->y_center - (text_box->rect.h/2);

  SDL_FreeSurface(surf);

  SDL_RenderCopy(rend, text_box->tex, NULL, &(text_box->rect));
}
