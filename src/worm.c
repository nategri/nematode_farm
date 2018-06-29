#include "worm.h"

static double dot(double const* a, double const* b) {
  return a[0]*b[0] + a[1]*b[1];
}

void worm_init(Worm* const worm) {
  uint32_t worm_x_init = (rand() % (int)(0.75*WINDOW_X)) + (int)(.125*WINDOW_X);
  uint32_t worm_y_init = (rand() % (int)(0.75*WINDOW_Y)) + (int)(.125*WINDOW_Y);
  double worm_theta_init = rand() % 360;
  worm->phys_state = (WormPhysicalState) {worm_x_init, worm_y_init, 0.0, 0.0, worm_theta_init};
  ctm_init(&worm->bio_state.connectome);
  worm->bio_state.muscle = (MuscleState) {0, 0, 0, 0, 0};
  worm->bio_state.motor_ab_fire_avg = 5.25;
  worm->nose_touching = 0;
  worm->sprite = (Sprite) {
    (int)worm->phys_state.x,
    (int)worm->phys_state.y,
    SPRITE_W,
    SPRITE_H,
    worm->phys_state.theta
  };

  // Burn in worm state
  int rand_int = (rand() % 1000) + 500;
  for(int i = 0; i < rand_int; i++) {
      worm_update(worm, CHEMOTAXIS, CHEMOTAXIS_LEN);
  }
}

// Change to accept just worm super-struct
void worm_phys_state_update(Worm* const worm) {
  const double dt = 0.0005;

  const double velocity = (worm->bio_state.muscle.left + worm->bio_state.muscle.right)/2.0;

  const double dtheta_scale = 8.0;
  const double dtheta = (worm->bio_state.muscle.left - worm->bio_state.muscle.right)*dtheta_scale;

  worm->phys_state.theta += dtheta*dt;

  worm->phys_state.vx = velocity*cos(worm->phys_state.theta*M_PI/180.0);
  worm->phys_state.vy = velocity*sin(worm->phys_state.theta*M_PI/180.0);

  worm->phys_state.x += worm->phys_state.vx*dt;
  worm->phys_state.y += worm->phys_state.vy*dt;
}

void sprite_update(Worm* const worm) {
  worm->sprite.x = (int)worm->phys_state.x;
  worm->sprite.y = (int)worm->phys_state.y;

  if(!(worm->phys_state.vx == 0 && worm->phys_state.vy == 0)) {
    worm->sprite.theta = 90.0 + atan2(worm->phys_state.vy, worm->phys_state.vx)*180.0/M_PI;
  }
  if((worm->bio_state.muscle.left < 0) && (worm->bio_state.muscle.right < 0)) {
    worm->sprite.theta += 180;
  }

  worm->sprite_rect.x = worm->sprite.x - SPRITE_W/2;
  worm->sprite_rect.y = worm->sprite.y - SPRITE_H/2;
  worm->sprite_rect.w = worm->sprite.w;
  worm->sprite_rect.h = worm->sprite.h;
}

void worm_update(Worm* const worm, const uint16_t* stim_neuron, int len_stim_neuron) {
  Connectome* ctm = &worm->bio_state.connectome;

  //
  // Run one tick of neural emulation
  //

  ctm_neural_cycle(ctm, stim_neuron, len_stim_neuron);

  //
  // Aggregate muscle states
  //

  uint16_t body_total = 0;
  // Gather totals on body muscles
  for(int i = 0; i < BODY_MUSCLES; i++) {
    uint16_t left_id = READ_WORD(left_body_muscle, i);
    uint16_t right_id = READ_WORD(right_body_muscle, i);

    int16_t left_val = ctm_get_weight(ctm, left_id);
    int16_t right_val = ctm_get_weight(ctm, right_id);

    if(left_val < 0) {
      left_val = 0;
    }

    if(right_val < 0) {
      right_val = 0;
    }

    body_total += (left_val + right_val);
  }

  uint16_t norm_body_total = 255.0 * ((float) body_total) / 600.0;

  // Gather total for neck muscles
  uint16_t left_neck_total = 0;
  uint16_t right_neck_total = 0;
  for(int i = 0; i < NECK_MUSCLES; i++) {
    uint16_t left_id = READ_WORD(left_neck_muscle, i);
    uint16_t right_id = READ_WORD(right_neck_muscle, i);

    int16_t left_val = ctm_get_weight(ctm, left_id);
    int16_t right_val = ctm_get_weight(ctm, right_id);

    if(left_val < 0) {
      left_val = 0;
    }

    if(right_val < 0) {
      right_val = 0;
    }

    left_neck_total += left_val;
    right_neck_total += right_val;
  }

  // Combine neck contribution with body and log meta information
  // for motion component visualizer
  int32_t neck_contribution = left_neck_total - right_neck_total;
  int32_t left_total;
  int32_t right_total;
  if(neck_contribution < 0) {
    left_total = 6*abs(neck_contribution) + norm_body_total;
    right_total = norm_body_total;
    worm->bio_state.muscle.meta_left_neck = 6*abs(neck_contribution);
    worm->bio_state.muscle.meta_right_neck = 0;
  }
  else {
    left_total = norm_body_total;
    right_total = 6*abs(neck_contribution) + norm_body_total;
    worm->bio_state.muscle.meta_left_neck = 0;
    worm->bio_state.muscle.meta_right_neck = 6*abs(neck_contribution);
  }

  worm->bio_state.muscle.meta_body = norm_body_total;

  // Log A and B type motor neuron activity
  double motor_neuron_sum = 0;

  for(int i = 0; i < MOTOR_B; i++) {
    uint16_t id = READ_WORD(motor_neuron_b, i);
    motor_neuron_sum += ctm_get_discharge(ctm, id);
    //printf("%d\n", ctm_get_discharge(ctm, id));
  }

  for(int i = 0; i < MOTOR_A; i++) {
    uint16_t id = READ_WORD(motor_neuron_a, i);
    motor_neuron_sum += ctm_get_discharge(ctm, id);
    //printf("%d\n", ctm_get_discharge(ctm, id));
  }

  const double motor_total = MOTOR_A + MOTOR_B;

  worm->bio_state.motor_ab_fire_avg = (motor_neuron_sum + (motor_total*worm->bio_state.motor_ab_fire_avg))/(motor_total + 1.0);

  if(worm->bio_state.motor_ab_fire_avg > 5.5) { // Magic number read off from c_matoduino simulation
    //printf("%f\n", worm->bio_state.motor_ab_fire_avg);
    //printf("reverse");
    left_total *= -1;
    right_total *= -1;
  }

  worm->bio_state.muscle.left = left_total / 0.5;
  worm->bio_state.muscle.right = right_total / 0.5;

}

uint8_t collide_with_worm(Worm* const worm, uint8_t curr_index, Worm* const worm_arr, const uint8_t len) {
  uint8_t nose_touching = 0;
  for(uint8_t i = 0; i < len; i++) {

    const static int hitbox_w = SPRITE_W/2;
    const static int hitbox_h = SPRITE_H/2;

    if(i != curr_index) {
      const uint16_t x_sep = abs(worm->sprite.x - worm_arr[i].sprite.x);
      const uint16_t y_sep = abs(worm->sprite.y - worm_arr[i].sprite.y);

      const double r[2] = {worm_arr[i].phys_state.x - worm->phys_state.x, worm_arr[i].phys_state.y - worm->phys_state.y};
      const double v[2] = {worm->phys_state.vx, worm->phys_state.vy};

      // Decide whether nose is touching
      if( (dot(r, v) > 0) && (worm->bio_state.muscle.left > 0) && (worm->bio_state.muscle.right > 0) ) {
        if( (x_sep < hitbox_w+10) && (y_sep < hitbox_h+10 )) {
          nose_touching = 1;
        }
      }

      // Do the physics of the collision
      if(x_sep <= hitbox_w) {
        if(y_sep <= hitbox_h) {
          if((abs(worm->sprite.x - worm_arr[i].sprite.x) > hitbox_w-1)) { 
            // Approach from left
            if(worm->sprite.x < worm_arr[i].sprite.x) {
              worm->phys_state.x -= 1.0;
            }
            // Approach from right
            else if(worm->sprite.x > worm_arr[i].sprite.x) {
              worm->phys_state.x += 1.0;
            }
          }
          if(abs(worm->sprite.y - worm_arr[i].sprite.y) > hitbox_h-1) {

            // Approach from (screen) top
            if(worm->sprite.y < worm_arr[i].sprite.y) {
              worm->phys_state.y -= 1.0;
            }
            // Approach from (screen) bottom
            else if(worm->sprite.y > worm_arr[i].sprite.y) {
              worm->phys_state.y += 1.0;
            }
          }
        }
      }
    }
  }
  return nose_touching;
}

// Handles effects on worm's physical state if wall collision occurs
// returns true if its nose is touching a wall
uint8_t collide_with_wall(Worm* const worm) {
  // Define normal vectors
  static const double nvec_bottom[] = {0.0, -1.0};
  static const double nvec_top[] = {0.0, 1.0};
  static const double nvec_right[] = {-1.0, 0.0};
  static const double nvec_left[] = {1.0, 0.0};

  double v[] = {worm->phys_state.vx, worm->phys_state.vy};
  double dot_prod = 0;

  uint8_t collide;

  // Right
  if(worm->phys_state.x > WINDOW_X - SPRITE_W/2) {
    worm->phys_state.x = WINDOW_X - SPRITE_W/2;
    dot_prod = dot(v, nvec_right);
    collide = 1;
  }
  // Left
  else if(worm->phys_state.x < SPRITE_W/2) {
    worm->phys_state.x = SPRITE_W/2;
    dot_prod = dot(v, nvec_left);
    collide = 1;
  }
  // Bottom
  if(worm->phys_state.y > WINDOW_Y - SPRITE_H/2) {
    worm->phys_state.y = WINDOW_Y - SPRITE_H/2;
    dot_prod = dot(v, nvec_bottom);
    collide = 1;
  }
  // Top
  else if(worm->phys_state.y < SPRITE_H/2) {
    worm->phys_state.y = SPRITE_H/2;
    dot_prod = dot(v, nvec_top);
    collide = 1;
  }

  // Remember, its won't touch if it's backing up
  if(collide && dot_prod < 0 && worm->bio_state.muscle.left > 0 && worm->bio_state.muscle.right > 0) {
    return 1;
  }
  else {
    return 0;
  }
}

// Same as NPC worm except don't need some bio info
// motion will occurr through direct muscle state manipulation
void player_worm_init(Worm* const worm) {
  uint32_t worm_x_init = (int)(0.5*WINDOW_X);
  uint32_t worm_y_init = (int)(0.5*WINDOW_Y);
  double worm_theta_init = -90.0;
  worm->phys_state = (WormPhysicalState) {worm_x_init, worm_y_init, 0.0, 0.0, worm_theta_init};

  worm->bio_state.muscle = (MuscleState) {0, 0, 0, 0, 0};
  worm->sprite = (Sprite) {
    (int)worm->phys_state.x,
    (int)worm->phys_state.y,
    SPRITE_W,
    SPRITE_H,
    worm->phys_state.theta+90.0
  };
}

void player_worm_update(Worm* const worm, int left, int right) {
  worm->bio_state.muscle.left = left;
  worm->bio_state.muscle.right = right;
}
