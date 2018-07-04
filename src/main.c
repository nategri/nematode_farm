//
//
// gcc graphics_demo/main.c -o demo -I /usr/local/Cellar/sdl2/2.0.8/include/SDL2 -L /usr/local/Cellar/sdl2/2.0.8/lib -l SDL2-2.0.0o
//
//

#include <stdlib.h>
#include <time.h>

#include "SDL.h"

#include "colors.h"
#include "display.h"

#include "behaviors.h"
#include "worm.h"
#include "trap.h"

#include "motion_component_display.h"
#include "muscle_display.h"

int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));

  // Initialize graphics window
  SDL_Window* win;
  SDL_Renderer* rend;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_X, WINDOW_Y, 0, &win, &rend);
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

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
  player_worm_init(player_worm, rend);

  // Create array of non-player AI worms
  static const uint8_t num_worms = 3;
  Worm* worm_arr = malloc(num_worms*sizeof(Worm));
  for(uint8_t n=0; n < num_worms; n++) {
    // Initialize worms
    worm_init(&worm_arr[n], rend, RED);
  }

  // Create a worm trap
  Trap* red_trap = malloc(sizeof(Trap));
  trap_init(red_trap, RED, 320, 240, 120, 120);

  // Begin graphical simulation
  SDL_Event sdl_event;

  int player_left_muscle = 0;
  int player_right_muscle = 0;

  // Flag for visualization
  uint8_t show_vis = 0;

  // Main animation loop
  for(int i=0; 1; i++) {

    // Check keyboard for input
    if(SDL_PollEvent(&sdl_event)) {
      if(sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
          const uint8_t* kb_state = SDL_GetKeyboardState(NULL);
          if(kb_state[SDL_SCANCODE_ESCAPE]) {
            break;
          }

          static const uint16_t player_speed = 150;

          if(kb_state[SDL_SCANCODE_UP] && kb_state[SDL_SCANCODE_LEFT]) {
            player_left_muscle = player_speed/2;
            player_right_muscle = player_speed;
          }
          else if(kb_state[SDL_SCANCODE_UP] && kb_state[SDL_SCANCODE_RIGHT]) {
            player_left_muscle = player_speed;
            player_right_muscle = player_speed/2;
          }
          else if(kb_state[SDL_SCANCODE_DOWN] && kb_state[SDL_SCANCODE_LEFT]) {
            player_left_muscle = -player_speed/2;
            player_right_muscle = -player_speed;
          }
          else if(kb_state[SDL_SCANCODE_DOWN] && kb_state[SDL_SCANCODE_RIGHT]) {
            player_left_muscle = -player_speed;
            player_right_muscle = -player_speed/2;
          }
          else if(kb_state[SDL_SCANCODE_UP]) {
            player_left_muscle = player_speed;
            player_right_muscle = player_speed;
          }
          else if(kb_state[SDL_SCANCODE_DOWN]) {
            player_left_muscle = -player_speed;
            player_right_muscle = -player_speed;
          }
          else if(kb_state[SDL_SCANCODE_LEFT]) {
            player_left_muscle = 0;
            player_right_muscle = player_speed/2;
          }
          else if(kb_state[SDL_SCANCODE_RIGHT]) {
            player_left_muscle = player_speed/2;
            player_right_muscle = 0;
          }
          else if(kb_state[SDL_SCANCODE_V]) {
            show_vis = !show_vis;
          }
          else {
            player_left_muscle = 0;
            player_right_muscle = 0;
          }
      }
    }

    SDL_SetRenderDrawColor(rend, 128, 128, 128, 0);
    SDL_RenderClear(rend);

    // Draw traps
    trap_draw(rend, red_trap);

    // Update player worm
    player_worm_update(player_worm, player_left_muscle, player_right_muscle);
    worm_phys_state_update(player_worm);
    collide_with_wall(player_worm);
    collide_with_worm(player_worm, -1, worm_arr, num_worms);
    sprite_update(player_worm);
    worm_draw(rend, player_worm);

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

      //sprite_update(&worm_arr[n]);

      worm_arr[n].nose_touching = 0;

      worm_update_trapped(&worm_arr[n], red_trap);
      if(worm_arr[n].trapped && (worm_arr[n].color == RED)) {
        // Collide with trap
        worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_trap(&worm_arr[n], red_trap);
      }

      // Collide with walls
      worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_wall(&worm_arr[n]);
      // Collide with other AI worms
      worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_worm(&worm_arr[n], n, worm_arr, num_worms);
      // Collide with player worm
      Worm player_worm_arr[] = {*player_worm};
      worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_worm(&worm_arr[n], -1, player_worm_arr, 1);

      sprite_update(&worm_arr[n]);

      worm_draw(rend, &worm_arr[n]);

      if(n==0) {
        motion_component_display_update(&motion_component_display, &worm_arr[n]);
        muscle_display_update(&muscle_display, &worm_arr[n]);
      }
    }

    if(show_vis) {
      motion_component_display_draw(rend, &motion_component_display);
      muscle_display_draw(rend, &muscle_display);
    }

    if(i > 1000) {
      SDL_RenderPresent(rend);
    }
  }

  SDL_Quit();
}
