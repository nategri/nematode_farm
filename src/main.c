//
//
// gcc graphics_demo/main.c -o demo -I /usr/local/Cellar/sdl2/2.0.8/include/SDL2 -L /usr/local/Cellar/sdl2/2.0.8/lib -l SDL2-2.0.0o
//
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <emscripten.h>

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

typedef struct {
  SDL_Window* win;
  SDL_Renderer* rend;

  TextBox* primary_text;
  TextBox* secondary_text;
  TextBox* tertiary_text;

  TextBox* hint_text;
  TextBox* timer_text;

  MuscleDisplay muscle_display;
  MotionComponentDisplay motion_component_display;

  Worm* player_worm;

  static const uint8_t num_worms;

  Worm* worm_arr;

  Trap* red_trap;
  Trap* blue_trap;

  SDL_Event sdl_event;

  int player_left_muscle;
  int player_right_muscle;

  // Flag for visualization
  uint8_t show_vis;

  // Holds start time of gameplay
  uint32_t init_ticks;

  // Declare program states 
  typedef enum {INIT, GAME_PLAY, GAME_OVER, GAME_WIN} program_state;

  // Holds countdown value during gameplay
  uint32_t countdown_value;

  uint32_t loop_tick;

} LoopContext;

void init_loop_context(LoopContext* ctx) {
  // Initialize text boxes
  ctx->primary_text = malloc(sizeof(TextBox));
  text_box_init(ctx->primary_text, 320, 100, "./fonts/OpenSans-Regular.ttf", 48);

  ctx->secondary_text = malloc(sizeof(TextBox));
  text_box_init(ctx->secondary_text, 320, 145, "./fonts/OpenSans-Regular.ttf", 24);

  ctx->tertiary_text = malloc(sizeof(TextBox));
  text_box_init(ctx->tertiary_text, 320, 380, "./fonts/OpenSans-Regular.ttf", 24);

  ctx->hint_text = malloc(sizeof(TextBox));
  text_box_init(ctx->hint_text, 140, 462, "./fonts/OpenSans-Regular.ttf", 14);

  ctx->timer_text = malloc(sizeof(TextBox));
  text_box_init(ctx->timer_text, 320, 20, "./fonts/OpenSans-Regular.ttf", 32);

  // Initialize muscle display
  muscle_display_init(&ctx->muscle_display);

  // Initialize motion component
  motion_component_display_init(&ctx->motion_component_display);

  // Create player worm
  ctx->player_worm = malloc(sizeof(Worm));

  // Create array of non-player AI worms
  ctx->num_worms = 20;
  ctx->worm_arr = malloc(num_worms*sizeof(Worm));
  non_player_worm_array_init(ctx->rend, ctx->worm_arr, ctx->num_worms);

  // Create blue and red worm traps
  ctx->red_trap = malloc(sizeof(Trap));
  trap_init(ctx->red_trap, RED, 200, 240, 120, 120);
  ctx->blue_trap = malloc(sizeof(Trap));
  trap_init(ctx->blue_trap, BLUE, 440, 240, 120, 120);

  ctx->player_left_muscle = 0;
  ctx->player_right_muscle = 0;

  // Flag for visualization
  ctx->show_vis = 0;

  ctx->current_state = INIT;

  ctx->loop_tick = 0;
}

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

void loop(void *arg) {
  LoopContext* ctx = (LoopContext*) arg

  // Frame drawing loop
  SDL_SetRenderDrawColor(ctx->rend, 128, 128, 128, 0);
  SDL_RenderClear(ctx->rend);

  // Check keyboard for input
  if(SDL_PollEvent(&ctx->sdl_event)) {
    if(ctx->sdl_event.type == SDL_KEYDOWN || ctx->sdl_event.type == SDL_KEYUP) {
        const uint8_t* kb_state = SDL_GetKeyboardState(NULL);
        if(kb_state[SDL_SCANCODE_ESCAPE]) {
          ctx->current_state = INIT;
        }

        if(kb_state[SDL_SCANCODE_RETURN]) {
          if( (ctx->current_state == INIT) || (ctx->current_state == GAME_WIN) || (ctx->current_state == GAME_OVER)) {
            ctx->current_state = GAME_PLAY;

            // Init AI worms
            non_player_worm_array_init(ctx->rend, ctx->worm_arr, ctx->num_worms);

            // Init player worm
            player_worm_init(ctx->player_worm, ctx->rend);

            // Record start time of gameplay
            ctx->init_ticks = SDL_GetTicks();
          }
        }

        static const uint16_t player_speed = 300;

        if(kb_state[SDL_SCANCODE_UP] && kb_state[SDL_SCANCODE_LEFT]) {
          ctx->player_left_muscle = player_speed/1.5;
          ctx->player_right_muscle = player_speed;
        }
        else if(kb_state[SDL_SCANCODE_UP] && kb_state[SDL_SCANCODE_RIGHT]) {
          ctx->player_left_muscle = player_speed;
          ctx->player_right_muscle = player_speed/1.5;
        }
        else if(kb_state[SDL_SCANCODE_DOWN] && kb_state[SDL_SCANCODE_LEFT]) {
          ctx->player_left_muscle = -player_speed/1.5;
          ctx->player_right_muscle = -player_speed;
        }
        else if(kb_state[SDL_SCANCODE_DOWN] && kb_state[SDL_SCANCODE_RIGHT]) {
          ctx->player_left_muscle = -player_speed;
          ctx->player_right_muscle = -player_speed/1.5;
        }
        else if(kb_state[SDL_SCANCODE_UP]) {
          ctx->player_left_muscle = player_speed;
          ctx->player_right_muscle = player_speed;
        }
        else if(kb_state[SDL_SCANCODE_DOWN]) {
          ctx->player_left_muscle = -player_speed;
          ctx->player_right_muscle = -player_speed;
        }
        else if(kb_state[SDL_SCANCODE_LEFT]) {
          ctx->player_left_muscle = 0;
          ctx->player_right_muscle = player_speed/2;
        }
        else if(kb_state[SDL_SCANCODE_RIGHT]) {
          ctx->player_left_muscle = player_speed/2;
          ctx->player_right_muscle = 0;
        }
        else if(kb_state[SDL_SCANCODE_V]) {
          ctx->show_vis = !ctx->show_vis;
        }
        else {
          ctx->player_left_muscle = 0;
          ctx->player_right_muscle = 0;
        }
    }
  }

  // Draw traps
  if((ctx->current_state == GAME_PLAY) || (ctx->current_state == GAME_WIN) || (ctx->current_state == GAME_OVER)) {
    trap_draw(ctx->rend, ctx->red_trap);
    trap_draw(ctx->rend, ctx->blue_trap);
  }

  // Update player worm
  if(ctx->current_state == GAME_PLAY) {
    player_worm_update(ctx->player_worm, ctx->player_left_muscle, ctx->player_right_muscle);
    worm_phys_state_update(ctx->player_worm);
    collide_with_wall(ctx->player_worm);
    collide_with_worm(ctx->player_worm, -1, ctx->worm_arr, ctx->num_worms);
    sprite_update(ctx->player_worm);
    worm_draw(ctx->rend, ctx->player_worm);
  }

  // Update worm AIs
  for(uint8_t n=0; n<ctx->num_worms; n++) {
    if(ctx->loop_tick % 120 == 0) {
      if(ctx->worm_arr[n].nose_touching) {
        worm_update(&ctx->worm_arr[n], NOSE_TOUCH, NOSE_TOUCH_LEN);
      }
      else {
        worm_update(&ctx->worm_arr[n], CHEMOTAXIS, CHEMOTAXIS_LEN);
      }
    }
    worm_phys_state_update(&ctx->worm_arr[n]);

    ctx->worm_arr[n].nose_touching = 0;

    worm_update_trapped(&ctx->worm_arr[n], ctx->red_trap);
    worm_update_trapped(&ctx->worm_arr[n], ctx->blue_trap);

    // Collide with blue and red traps
    if((ctx->current_state == GAME_PLAY) || (ctx->current_state == GAME_WIN) || (ctx->current_state == GAME_OVER)) {
      ctx->worm_arr[n].nose_touching = ctx->worm_arr[n].nose_touching || collide_with_trap(&ctx->worm_arr[n], ctx->red_trap);
      ctx->worm_arr[n].nose_touching = ctx->worm_arr[n].nose_touching || collide_with_trap(&ctx->worm_arr[n], ctx->blue_trap);
    }

    // Collide with walls
    ctx->worm_arr[n].nose_touching = ctx->worm_arr[n].nose_touching || collide_with_wall(&ctx->worm_arr[n]);

    if (ctx->current_state != INIT) {
      // Collide with other AI worms
      ctx->worm_arr[n].nose_touching = ctx->worm_arr[n].nose_touching || collide_with_worm(&ctx->worm_arr[n], n, ctx->worm_arr, ctx->num_worms);
      // Collide with player worm
      Worm player_worm_arr[] = {*ctx->player_worm};
      ctx->worm_arr[n].nose_touching = ctx->worm_arr[n].nose_touching || collide_with_worm(&ctx->worm_arr[n], -1, player_worm_arr, 1);
    }

    sprite_update(&ctx->worm_arr[n]);

    worm_draw(ctx->rend, &ctx->worm_arr[n]);

    if(n==0) {
      motion_component_display_update(&ctx->motion_component_display, &ctx->worm_arr[n]);
      muscle_display_update(&ctx->muscle_display, &ctx->worm_arr[n]);

      if (ctx->current_state == INIT) {
        break;
      }
    }
  }

  // Update text boxes and check if game has finished
  if(ctx->current_state == INIT) {
    //text_box_draw(rend, primary_text, "n e m a t o d e . f a r m");
    text_box_draw(ctx->rend, ctx->primary_text, "nematode.farm");

    if(!ctx->show_vis) {
      text_box_draw(ctx->rend, ctx->secondary_text, "Press [Enter] to Play");
      text_box_draw(ctx->rend, ctx->hint_text, "Press [V] to toggle muscle visualization");
    }
  }
  else if(ctx->current_state == GAME_PLAY) {
    uint8_t time_limit = 200;
    uint32_t elapsed_time = ((SDL_GetTicks() - init_ticks) / 1000);
    ctx->countdown_value = time_limit - elapsed_time;
    if(elapsed_time <= time_limit) {
      char countdown_string[4];
      sprintf(countdown_string, "%d", ctx->countdown_value);
      text_box_draw(ctx->rend, ctx->timer_text, countdown_string);
    }
    // Game over, man
    else {
      ctx->current_state = GAME_OVER;
    }

    // Check if you've won
    uint8_t all_trapped = 1;
    for(uint8_t n = 0; n<ctx->num_worms; n++) {
      if(!ctx->worm_arr[n].trapped) {
        all_trapped = 0;
        break;
      }
    }
    if(all_trapped) {
      ctx->current_state = GAME_WIN;
    }
  }
  else if(ctx->current_state == GAME_WIN) {
    char score_message[11];
    sprintf(score_message, "Score: %d", ctx->countdown_value);
    text_box_draw(ctx->rend, ctx->primary_text, "You Put the 'Todes Away :D");
    text_box_draw(ctx->rend, ctx->secondary_text, score_message);
    text_box_draw(ctx->rend, ctx->tertiary_text, "Press [Enter] to Play Again");
  }
  else if(ctx->current_state == GAME_OVER) {
    text_box_draw(ctx->rend, ctx->primary_text, "Try Again :(");
    text_box_draw(ctx->rend, ctx->secondary_text, "Press [Enter] to Play Again");
  }

  if(ctx->show_vis && (ctx->current_state == INIT)) {
    motion_component_display_draw(ctx->rend, &ctx->motion_component_display);
    muscle_display_draw(ctx->rend, &ctx->muscle_display);
  }

  if(i > 1000) {
    SDL_RenderPresent(ctx->rend);
  }

  ctx->loop_tick++;
}

int main(int argc, char* argv[]) {
  // Seed RNG
  srand(time(NULL));

  // Create loop context
  LoopContext loop_context;
  init_loop_context(*loop_context);

  // Initialize graphics window
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_X, WINDOW_Y, 0, &loop_context.win, &loop_context.rend);

  TTF_Init();

  SDL_SetRenderDrawBlendMode(loop_context.rend, SDL_BLENDMODE_BLEND);

  const int default_fps = -1;
  const int loop_forever = 1;

  emscripten_set_main_loop_arg(loop, &loop_context, default_fps, loop_forever);

  TTF_Quit();
  SDL_Quit();
}
