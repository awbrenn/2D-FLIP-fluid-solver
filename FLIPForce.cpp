//
// Created by awbrenn on 3/2/16.
//

#include "FLIPForce.h"

vector2 calculateGradW(SPHParticle *b, SPHParticle *a, float h) {
  float q, one_minus_q, magnitude;
  vector2 result;

  magnitude = ((b->position - a->position).length());
  q = magnitude / h;
  if (q >= 1.0f or q == 0.0) { result = vector2(0.0f, 0.0f); }
  else {
    one_minus_q = 1.0f - q;

    result = (b->position - a->position).scale(1.0f / magnitude)
        .scale((float) ((-30.0f / (M_PI * (h * h * h))) * (one_minus_q * one_minus_q)));
  }

  return result;
}

float SPHForce::calculatePressure(SPHParticle *p) {
  float result;
  result = beta * (powf((p->density/density_base), gamma) -1.0f);
  return result;
}

float SPHForce::calculateViscocityForce(SPHParticle *b, SPHParticle *a, float h) {
  float viscosity_force;

  vector2 x_ba = (b->position - a->position);
  float x_ba_magnitude = x_ba.length();

  viscosity_force = -1.0f * viscosity * (b->velocity - a->velocity).scale(h).dot(x_ba) /
                                        ((x_ba_magnitude*x_ba_magnitude) + (epsilon * h * h)) *
                                        (1.0f / (b->density + a->density));

  return viscosity_force;
}

vector2 SPHForce::evaluateForce(std::vector<SPHParticle> *particles, SPHParticle *b,
                                SPHOccupancyVolume *occupancy_volume, float h) {
  vector2 force;
  vector2 grad_w;
  float pressure_a, pressure_b, viscosity_force;
  SPHParticle *a;
  std::vector<size_t> check_indices;

  occupancy_volume->getIndicesOfAllPossibleCollisions(b, &check_indices);

  force = vector2(0.0f, 0.0f);
  for (unsigned int i = 0; i < check_indices.size(); ++i) {
    a = &particles->at(check_indices.at(i));
    pressure_a = calculatePressure(&(*a));
    pressure_b = calculatePressure(b);
    grad_w = calculateGradW(b, &(*a), h);
    viscosity_force = calculateViscocityForce(b, &(*a), h);
    force += grad_w.scale((a->mass) *  ((pressure_a / ( a->density * a->density)) +
                                        (pressure_b / ( b->density * b->density)) +
                                         viscosity_force));
  }
  force = force.scale(-1.0f/b->mass);

  force += gravity;
  return force;
}
