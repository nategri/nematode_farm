//
//
// gcc graphics_demo/main.c -o demo -I /usr/local/Cellar/sdl2/2.0.8/include/SDL2 -L /usr/local/Cellar/sdl2/2.0.8/lib -l SDL2-2.0.0o
//
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_ttf.h"

#include "colors.h"
#include "display.h"
#include "text_box.h"

#include "behaviors.h"
#include "worm.h"
#include "trap.h"

#include "motion_component_display.h"
#include "muscle_display.h"

void non_player_worm_array_init(SDL_Renderer* renderer, Worm* arr, uint8_t num) {
  for(uint8_t n=0; n < num; n++) {
    double coin_toss = (double) rand() / (double) RAND_MAX;
    // Initialize worms
    if(coin_toss > 0.5) {
      worm_init(&arr[n], renderer, RED);
    }
    else {
      worm_init(&arr[n], renderer, BLUE);
    }
  }
}

int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));

  // Initialize graphics window
  SDL_Window* win;
  SDL_Renderer* rend;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_X, WINDOW_Y, 0, &win, &rend);

  TTF_Init();

  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  // Initialize text boxes
  TextBox* primary_text = malloc(sizeof(TextBox));
  text_box_init(primary_text, 320, 100, "./fonts/OpenSans-Regular.ttf", 48);

  TextBox* secondary_text = malloc(sizeof(TextBox));
  text_box_init(secondary_text, 320, 145, "./fonts/OpenSans-Regular.ttf", 24);

  TextBox* tertiary_text = malloc(sizeof(TextBox));
  text_box_init(tertiary_text, 320, 380, "./fonts/OpenSans-Regular.ttf", 24);

  TextBox* hint_text = malloc(sizeof(TextBox));
  text_box_init(hint_text, 140, 462, "./fonts/OpenSans-Regular.ttf", 14);

  TextBox* timer_text = malloc(sizeof(TextBox));
  text_box_init(timer_text, 320, 20, "./fonts/OpenSans-Regular.ttf", 32);

  // Create and initialize muscle display
  MuscleDisplay muscle_display;
  muscle_display_init(&muscle_display);

  // Create and initialize motion component
  MotionComponentDisplay motion_component_display;
  motion_component_display_init(&motion_component_display);

  // Create player worm
  Worm* player_worm = malloc(sizeof(Worm));

  // Create array of non-player AI worms
  static const uint8_t num_worms = 20;
  Worm* worm_arr = malloc(num_worms*sizeof(Worm));
  non_player_worm_array_init(rend, worm_arr, num_worms);

  // Create blue and red worm traps
  Trap* red_trap = malloc(sizeof(Trap));
  trap_init(red_trap, RED, 200, 240, 120, 120);
  Trap* blue_trap = malloc(sizeof(Trap));
  trap_init(blue_trap, BLUE, 440, 240, 120, 120);

  // Begin graphical simulation
  SDL_Event sdl_event;

  int player_left_muscle = 0;
  int player_right_muscle = 0;

  // Flag for visualization
  uint8_t show_vis = 0;

  // Holds start time of gameplay
  uint32_t init_ticks;

  // Declare program states and initialize current state
  typedef enum {INIT, GAME_PLAY, GAME_OVER, GAME_WIN} program_state;
  program_state current_state = INIT;

  // Holds countdown value during gameplay
  uint32_t countdown_value;

  // Frame drawing loop
  for(int i=0; 1; i++) {

    SDL_SetRenderDrawColor(rend, 128, 128, 128, 0);
    SDL_RenderClear(rend);

    // Check keyboard for input
    if(SDL_PollEvent(&sdl_event)) {
      if(sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) {
          const uint8_t* kb_state = SDL_GetKeyboardState(NULL);
          if(kb_state[SDL_SCANCODE_ESCAPE]) {
            current_state = INIT;
            //break;
          }

          if(kb_state[SDL_SCANCODE_RETURN]) {
            if( (current_state == INIT) || (current_state == GAME_WIN) || (current_state == GAME_OVER)) {
              current_state = GAME_PLAY;

              // Init AI worms
              non_player_worm_array_init(rend, worm_arr, num_worms);

              // Init player worm
              player_worm_init(player_worm, rend);

              // Record start time of gameplay
              init_ticks = SDL_GetTicks();
            }
          }

          static const uint16_t player_speed = 300;

          if(kb_state[SDL_SCANCODE_UP] && kb_state[SDL_SCANCODE_LEFT]) {
            player_left_muscle = player_speed/1.5;
            player_right_muscle = player_speed;
          }
          else if(kb_state[SDL_SCANCODE_UP] && kb_state[SDL_SCANCODE_RIGHT]) {
            player_left_muscle = player_speed;
            player_right_muscle = player_speed/1.5;
          }
          else if(kb_state[SDL_SCANCODE_DOWN] && kb_state[SDL_SCANCODE_LEFT]) {
            player_left_muscle = -player_speed/1.5;
            player_right_muscle = -player_speed;
          }
          else if(kb_state[SDL_SCANCODE_DOWN] && kb_state[SDL_SCANCODE_RIGHT]) {
            player_left_muscle = -player_speed;
            player_right_muscle = -player_speed/1.5;
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

    // Draw traps
    if((current_state == GAME_PLAY) || (current_state == GAME_WIN) || (current_state == GAME_OVER)) {
      trap_draw(rend, red_trap);
      trap_draw(rend, blue_trap);
    }

    // Update player worm
    if(current_state == GAME_PLAY) {
      player_worm_update(player_worm, player_left_muscle, player_right_muscle);
      worm_phys_state_update(player_worm);
      collide_with_wall(player_worm);
      collide_with_worm(player_worm, -1, worm_arr, num_worms);
      sprite_update(player_worm);
      worm_draw(rend, player_worm);
    }

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

      worm_arr[n].nose_touching = 0;

      worm_update_trapped(&worm_arr[n], red_trap);
      worm_update_trapped(&worm_arr[n], blue_trap);

      // Collide with blue and red traps
      if((current_state == GAME_PLAY) || (current_state == GAME_WIN) || (current_state == GAME_OVER)) {
        worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_trap(&worm_arr[n], red_trap);
        worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_trap(&worm_arr[n], blue_trap);
      }

      // Collide with walls
      worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_wall(&worm_arr[n]);

      if (current_state != INIT) {
        // Collide with other AI worms
        worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_worm(&worm_arr[n], n, worm_arr, num_worms);
        // Collide with player worm
        Worm player_worm_arr[] = {*player_worm};
        worm_arr[n].nose_touching = worm_arr[n].nose_touching || collide_with_worm(&worm_arr[n], -1, player_worm_arr, 1);
      }

      sprite_update(&worm_arr[n]);

      worm_draw(rend, &worm_arr[n]);

      if(n==0) {
        motion_component_display_update(&motion_component_display, &worm_arr[n]);
        muscle_display_update(&muscle_display, &worm_arr[n]);

        if (current_state == INIT) {
          break;
        }
      }
    }

    // Update text boxes and check if game has finished
    if(current_state == INIT) {
      //text_box_draw(rend, primary_text, "n e m a t o d e . f a r m");
      text_box_draw(rend, primary_text, "nematode.farm");

      if(!show_vis) {
        text_box_draw(rend, secondary_text, "Press [Enter] to Play");
        text_box_draw(rend, hint_text, "Press [V] to toggle muscle visualization");
      }
    }
    else if(current_state == GAME_PLAY) {
      uint8_t time_limit = 200;
      uint32_t elapsed_time = ((SDL_GetTicks() - init_ticks) / 1000);
      countdown_value = time_limit - elapsed_time;
      if(elapsed_time <= time_limit) {
        char countdown_string[4];
        sprintf(countdown_string, "%d", countdown_value);
        text_box_draw(rend, timer_text, countdown_string);
      }
      // Game over, man
      else {
        current_state = GAME_OVER;
      }

      // Check if you've won
      uint8_t all_trapped = 1;
      for(uint8_t n = 0; n<num_worms; n++) {
        if(!worm_arr[n].trapped) {
          all_trapped = 0;
          break;
        }
      }
      if(all_trapped) {
        current_state = GAME_WIN;
      }
    }
    else if(current_state == GAME_WIN) {
      char score_message[11];
      sprintf(score_message, "Score: %d", countdown_value);
      text_box_draw(rend, primary_text, "You Put the 'Todes Away :D");
      text_box_draw(rend, secondary_text, score_message);
      text_box_draw(rend, tertiary_text, "Press [Enter] to Play Again");
    }
    else if(current_state == GAME_OVER) {
      text_box_draw(rend, primary_text, "Try Again :(");
      text_box_draw(rend, secondary_text, "Press [Enter] to Play Again");
    }

    if(show_vis && (current_state == INIT)) {
      motion_component_display_draw(rend, &motion_component_display);
      muscle_display_draw(rend, &muscle_display);
    }

    if(i > 1000) {
      SDL_RenderPresent(rend);
    }
  }

  TTF_Quit();
  SDL_Quit();
}
