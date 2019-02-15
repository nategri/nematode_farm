#include "muscle_display.h"

static void muscle_display_cell_color(MuscleDisplayCell* const cell, uint8_t i, float weight) {
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
