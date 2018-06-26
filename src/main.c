//
//
// gcc graphics_demo/main.c -o demo -I /usr/local/Cellar/sdl2/2.0.8/include/SDL2 -L /usr/local/Cellar/sdl2/2.0.8/lib -l SDL2-2.0.0o
//
//

#include <stdlib.h>
#include <time.h>

#include "SDL.h"

#include "display.h"

#include "behaviors.h"
#include "worm.h"

#include "muscle_display.h"
#include "motion_component_display.h"

int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));

  // Initialize graphics window
  SDL_Window* win;
  SDL_Renderer* rend;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_X, WINDOW_Y, 0, &win, &rend);
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  // Pull images for worm sprites and create textures
  SDL_Surface* surf;
  SDL_Texture* worm_tex;
  SDL_Texture* worm_nose_tex;
  SDL_Texture* player_worm_tex;
  SDL_Texture* player_worm_nose_tex;

  surf = SDL_LoadBMP("./img/worm.bmp");
  worm_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  surf = SDL_LoadBMP("./img/worm_nose.bmp");
  worm_nose_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  surf = SDL_LoadBMP("./img/worm_mark.bmp");
  player_worm_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  surf = SDL_LoadBMP("./img/worm_mark_nose.bmp");
  player_worm_nose_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  // Texture for current state of the worm
  SDL_Texture* curr_tex;

  // Create and initialize muscle display
  MuscleDisplay muscle_display;
  muscle_display_init(&muscle_display);

  // Create and initialize motion component
  MotionComponentDisplay motion_component_display;
  motion_component_display_init(&motion_component_display);

  // Create player worm
  Worm* player_worm = malloc(sizeof(Worm));
  player_worm_init(player_worm);

  // Create array of non-player AI worms
  static const uint8_t num_worms = 25;
  Worm* worm_arr = malloc(num_worms*sizeof(Worm));
  for(uint8_t n=0; n < num_worms; n++) {
    // Initialize worms
    worm_init(&worm_arr[n]);
  }

  // Begin graphical simulation
  SDL_Event sdl_event;

  int player_left_muscle = 0;
  int player_right_muscle = 0;

  // Main animation loop
  for(int i=0; 1; i++) {

    // Check 
    if(SDL_PollEvent(&sdl_event)) {
      if(sdl_event.type == SDL_KEYDOWN) {
          if(sdl_event.key.keysym.sym == SDLK_ESCAPE) {
            break;
          }

          if((sdl_event.key.keysym.sym == SDLK_UP) && (sdl_event.key.keysym.sym == SDLK_LEFT)) {
            player_left_muscle = 125;
            player_right_muscle = 0;
          }
          else if((sdl_event.key.keysym.sym == SDLK_UP) && (sdl_event.key.keysym.sym == SDLK_RIGHT)) {
            player_left_muscle = 0;
            player_right_muscle = 125;
          }
          else if(sdl_event.key.keysym.sym == SDLK_UP) {
            player_left_muscle = 250;
            player_right_muscle = 250;
          }
          else if(sdl_event.key.keysym.sym == SDLK_DOWN) {
            player_left_muscle = -250;
            player_right_muscle = -250;
          }
      }
      else {
        player_left_muscle = 0;
        player_right_muscle = 0;
      }
    }

    SDL_SetRenderDrawColor(rend, 128, 128, 128, 0);
    SDL_RenderClear(rend);

    // Update player worm
    player_worm_update(player_worm, player_left_muscle, player_right_muscle);
    worm_phys_state_update(player_worm);
    curr_tex = player_worm_tex;
    sprite_update(player_worm);
    collide_with_wall(player_worm);
    collide_with_worm(player_worm, num_worms+1, worm_arr, num_worms);
    SDL_RenderCopyEx(rend, curr_tex, NULL, &(player_worm->sprite_rect), player_worm->sprite.theta, NULL, SDL_FLIP_NONE);
    SDL_RenderDrawRect(rend, &(player_worm->sprite_rect));

    // Update worm AIs
    for(uint8_t n=0; n<num_worms; n++) {
      if(i % 120 == 0) {
        if(worm_arr[n].nose_touching) {
          worm_update(&worm_arr[n], NOSE_TOUCH, NOSE_TOUCH_LEN);
        }
        else {
          worm_update(&worm_arr[n], CHEMOTAXIS, CHEMOTAXIS_LEN);
        }
      }
      worm_phys_state_update(&worm_arr[n]);

      // Set what graphic to to use
      if(worm_arr[n].nose_touching) {
        curr_tex = worm_nose_tex;
      }
      else {
        curr_tex = worm_tex;
      }

      sprite_update(&worm_arr[n]);

      worm_arr[n].nose_touching = 0;
      worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_wall(&worm_arr[n]);
      worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_worm(&worm_arr[n], n, worm_arr, num_worms);

      sprite_update(&worm_arr[n]);

      SDL_RenderCopyEx(rend, curr_tex, NULL, &(worm_arr[n].sprite_rect), worm_arr[n].sprite.theta, NULL, SDL_FLIP_NONE);
      SDL_RenderDrawRect(rend, &(worm_arr[n].sprite_rect));

      if(n==0) {
        motion_component_display_update(&motion_component_display, &worm_arr[n]);
        muscle_display_update(&muscle_display, &worm_arr[n]);
      }
    }

    motion_component_display_draw(rend, &motion_component_display);
    muscle_display_draw(rend, &muscle_display);

    if(i > 1000) {
      SDL_RenderPresent(rend);
    }
  }

  SDL_Quit();
}
