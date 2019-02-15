#include "motion_component_display.h"

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
