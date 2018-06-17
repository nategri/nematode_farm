#ifndef WORM_H
#define WORM_H

#define SPRITE_W 40
#define SPRITE_H 40

#include "SDL.h"
#include "display.h"
#include "worm_internal_state.h"
#include "behaviors.h"

typedef struct {
  WormPhysicalState phys_state;
  WormBioState bio_state;
  Sprite sprite;
  SDL_Rect sprite_rect;
  uint8_t nose_touching;
} Worm;

void worm_init(Worm* const);
void worm_phys_state_update(Worm* const);
void sprite_update(Worm* const);
void worm_update(Worm* const, const uint16_t*, int);
uint8_t collide_with_worm(Worm* const, uint8_t, Worm* const, const uint8_t);
uint8_t collide_with_wall(Worm* const worm);

#endif
