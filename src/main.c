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

extern const uint16_t NOSE_TOUCH[];
extern const uint16_t CHEMOTAXIS[];

//
// Structs for muscle cell state display
//

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

//
// Structs movement component display
//

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

// Set position and colors of motion component display
void motion_component_display_init(MotionComponentDisplay* const motion_comp_disp) {
  static const uint32_t x_anchor = 20;
  static const uint32_t y_anchor = 160;
  static const uint8_t w = 22;
  static const uint8_t h = 0;
  static const uint8_t pad = 2;

  motion_comp_disp->x_anchor = x_anchor;
  motion_comp_disp->y_anchor = y_anchor;

  motion_comp_disp->left_body = (SDL_Rect) {x_anchor, y_anchor, w, h};
  motion_comp_disp->left_neck = (SDL_Rect) {x_anchor, y_anchor, w, h};

  motion_comp_disp->right_body = (SDL_Rect) {x_anchor+w+pad, y_anchor, w, h};
  motion_comp_disp->right_neck = (SDL_Rect) {x_anchor+w+pad, y_anchor, w, h};

  motion_comp_disp->color_neck[0] = 180;
  motion_comp_disp->color_neck[1] = 76;
  motion_comp_disp->color_neck[2] = 211;
  motion_comp_disp->color_neck[3] = 255;

  motion_comp_disp->color_body[0] = 140;
  motion_comp_disp->color_body[1] = 59;
  motion_comp_disp->color_body[2] = 165;
  motion_comp_disp->color_body[3] = 255;
}

void motion_component_display_update(MotionComponentDisplay* const motion_component_disp, Worm* const worm) {
  const double scale = 0.5;

  double left_body_comp = worm->bio_state.muscle.meta_body;
  double right_body_comp = worm->bio_state.muscle.meta_body;
  double left_neck_comp = worm->bio_state.muscle.meta_left_neck;
  double right_neck_comp = worm->bio_state.muscle.meta_right_neck;

  const uint32_t y_anchor = motion_component_disp->y_anchor;

  motion_component_disp->left_body.h = (int) (left_body_comp*scale);
  motion_component_disp->left_body.y = y_anchor - (int) (left_body_comp*scale);

  motion_component_disp->right_body.h = (int) (right_body_comp*scale);
  motion_component_disp->right_body.y = y_anchor - (int) (right_body_comp*scale);

  motion_component_disp->left_neck.h = (int) left_neck_comp*scale;
  motion_component_disp->left_neck.y = y_anchor - (((int) left_neck_comp*scale) + motion_component_disp->left_body.h);

  motion_component_disp->right_neck.h = (int) right_neck_comp*scale;
  motion_component_disp->right_neck.y = y_anchor - (((int) right_neck_comp*scale) + motion_component_disp->right_body.h);
}
void motion_component_display_draw(SDL_Renderer* rend, MotionComponentDisplay* const motion_component_disp) {
  uint8_t* rgba;

  rgba = motion_component_disp->color_body;
  SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
  SDL_RenderFillRect(rend, &(motion_component_disp->left_body));
  SDL_RenderFillRect(rend, &(motion_component_disp->right_body));

  rgba = motion_component_disp->color_neck;
  SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
  SDL_RenderFillRect(rend, &(motion_component_disp->left_neck));
  SDL_RenderFillRect(rend, &(motion_component_disp->right_neck));
}

// Set positions and default colors of muscle display cells
void muscle_display_init(MuscleDisplay* const muscle_disp) {
  int x = 20;
  int y = 180;
  static const uint8_t w = 10;
  static const uint8_t h = 10;
  // Pad doesn't quite mean what you think it did (it's really "2")
  static const uint8_t pad = 2;

  // For neck muscles
  for(uint8_t i = 0; i < 4; i++) {
    muscle_disp->left_d_cell[i].rect = (SDL_Rect) {x, y, w, h};
    muscle_disp->left_v_cell[i].rect = (SDL_Rect) {x+1*(w+pad), y, w, h};
    muscle_disp->right_v_cell[i].rect = (SDL_Rect) {x+2*(w+pad), y, w, h};
    muscle_disp->right_d_cell[i].rect = (SDL_Rect) {x+3*(w+pad), y, w, h};
    y += (h+pad);
  }
  
  // For body muscles
  y += h;
  for(uint8_t i = 4; i < 19; i++) {
    muscle_disp->left_d_cell[i].rect = (SDL_Rect) {x, y, w, h};
    muscle_disp->left_v_cell[i].rect = (SDL_Rect) {x+1*(w+pad), y, w, h};
    muscle_disp->right_v_cell[i].rect = (SDL_Rect) {x+2*(w+pad), y, w, h};
    muscle_disp->right_d_cell[i].rect = (SDL_Rect) {x+3*(w+pad), y, w, h};
    y += (h+pad);
  }

  // Assign default colors
  for(uint8_t i = 0; i < 19; i++) {
    for(uint8_t j = 0; j < 4; j++) {
      muscle_disp->left_d_cell[i].rgba[j] = 255;
      muscle_disp->left_v_cell[i].rgba[j] = 255;
      muscle_disp->right_d_cell[i].rgba[j] = 255;
      muscle_disp->right_v_cell[i].rgba[j] = 255;
    }
  }
}

void muscle_display_cell_color(MuscleDisplayCell* const cell, uint8_t i, float weight) {
  if(weight < 0) {
    cell[i].rgba[0] = 255 + weight;
    cell[i].rgba[1] = 255 + weight;
    cell[i].rgba[2] = 255;
  }
  else {
    cell[i].rgba[0] = 255;
    cell[i].rgba[1] = 255 - weight;
    cell[i].rgba[2] = 255 - weight;
  }
}

void muscle_display_update(MuscleDisplay* const muscle_disp, Worm* const worm) {
  Connectome* ctm = &worm->bio_state.connectome;

  static const double scale = 30.0;

  //
  // Neck muscles
  //

  // Dorsal
  for(int8_t i = 0; i < 4; i++) {
    double l_wt = ctm_get_weight(ctm, left_neck_muscle[i])*scale;
    double r_wt = ctm_get_weight(ctm, right_neck_muscle[i])*scale;

    muscle_display_cell_color(muscle_disp->left_d_cell, i, l_wt);
    muscle_display_cell_color(muscle_disp->right_d_cell, i, r_wt);
  }

  // Ventral
  uint8_t cell_idx;
  for(uint8_t i = 4; i < 8; i++) {
    double l_wt = ctm_get_weight(ctm, left_neck_muscle[i])*scale;
    double r_wt = ctm_get_weight(ctm, right_neck_muscle[i])*scale;

    cell_idx = i - 4;

    muscle_display_cell_color(muscle_disp->left_v_cell, cell_idx, l_wt);
    muscle_display_cell_color(muscle_disp->right_v_cell, cell_idx, r_wt);
  }

  //
  // Body muscles
  //

  // Dorsal
  for(int8_t i = 0; i < 15; i++) {
    double l_wt = ctm_get_weight(ctm, left_body_muscle[i])*scale;
    double r_wt = ctm_get_weight(ctm, right_body_muscle[i])*scale;

    cell_idx = i + 4;

    muscle_display_cell_color(muscle_disp->left_d_cell, cell_idx, l_wt);
    muscle_display_cell_color(muscle_disp->right_d_cell, cell_idx, r_wt);
  }

  // Ventral
  for(uint8_t i = 15; i < 30; i++) {
    double l_wt = ctm_get_weight(ctm, left_body_muscle[i])*scale;
    double r_wt = ctm_get_weight(ctm, right_body_muscle[i])*scale;

    cell_idx = i + 4 - 15;

    muscle_display_cell_color(muscle_disp->left_v_cell, cell_idx, l_wt);
    muscle_display_cell_color(muscle_disp->right_v_cell, cell_idx, r_wt);
  }
}

void muscle_display_draw(SDL_Renderer* rend, MuscleDisplay* const muscle_disp) {
  for(uint8_t i = 0; i < 19; i++) {
    uint8_t* rgba;

    rgba = muscle_disp->left_d_cell[i].rgba;
    SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
    SDL_RenderFillRect(rend, &(muscle_disp->left_d_cell[i].rect));

    rgba = muscle_disp->left_v_cell[i].rgba;
    SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
    SDL_RenderFillRect(rend, &(muscle_disp->left_v_cell[i].rect));

    rgba = muscle_disp->right_d_cell[i].rgba;
    SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
    SDL_RenderFillRect(rend, &(muscle_disp->right_d_cell[i].rect));

    rgba = muscle_disp->right_v_cell[i].rgba;
    SDL_SetRenderDrawColor(rend, rgba[0], rgba[1], rgba[2], rgba[3]);
    SDL_RenderFillRect(rend, &(muscle_disp->right_v_cell[i].rect));
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
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  // Pull images for worm sprites and create textures
  SDL_Surface* surf;
  SDL_Texture* worm_tex;
  SDL_Texture* worm_nose_tex;
  SDL_Texture* worm_mark_tex;
  SDL_Texture* worm_mark_nose_tex;

  surf = SDL_LoadBMP("./img/worm.bmp");
  worm_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  surf = SDL_LoadBMP("./img/worm_nose.bmp");
  worm_nose_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  surf = SDL_LoadBMP("./img/worm_mark.bmp");
  worm_mark_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  surf = SDL_LoadBMP("./img/worm_mark_nose.bmp");
  worm_mark_nose_tex = SDL_CreateTextureFromSurface(rend, surf);
  SDL_FreeSurface(surf);

  // Texture for current state of the worm
  SDL_Texture* curr_tex;

  // Create and initialize muscle display
  MuscleDisplay muscle_display;
  muscle_display_init(&muscle_display);

  // Create and initialize motion component
  MotionComponentDisplay motion_component_display;
  motion_component_display_init(&motion_component_display);

  // Create array of worms
  static const uint8_t num_worms = 25;
  Worm* worm_arr = malloc(num_worms*sizeof(Worm));

  for(uint8_t n=0; n < num_worms; n++) {
    // Initialize worms
    worm_init(&worm_arr[n]);
  }


  // Begin graphical simulation
  SDL_Event sdl_event;

  // Main animation loop
  for(int i=0; 1; i++) {

    // Check for keypress---quit if there is one
    if(SDL_PollEvent(&sdl_event)) {
      if(sdl_event.type == SDL_KEYDOWN) {
        break;
      }
    }

    SDL_SetRenderDrawColor(rend, 128, 128, 128, 0);
    SDL_RenderClear(rend);

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
      if(n == 0) {
        if(worm_arr[n].nose_touching) {
          curr_tex = worm_mark_nose_tex;
        }
        else {
          curr_tex = worm_mark_tex;
        }
      }
      else {
        if(worm_arr[n].nose_touching) {
          curr_tex = worm_nose_tex;
        }
        else {
          curr_tex = worm_tex;
        }
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
