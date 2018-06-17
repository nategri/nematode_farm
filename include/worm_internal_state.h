#ifndef WORM_INTERNAL_STATE_H
#define WORM_INTERNAL_STATE_H

#include "defines.h"
#include "connectome.h"
#include "muscles.h"

typedef struct {
  int x;
  int y;
  int w;
  int h;
  double theta;
} Sprite;

typedef struct {
  double x;
  double y;
  double vx;
  double vy;
  double theta;
} WormPhysicalState;

typedef struct {
  int left;
  int right;
  int meta_left_neck;
  int meta_right_neck;
  int meta_body;
} MuscleState;

typedef struct {
  Connectome connectome;
  MuscleState muscle;
  double motor_ab_fire_avg;
} WormBioState;

#endif
