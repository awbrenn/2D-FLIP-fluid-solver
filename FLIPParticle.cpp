/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/8/2016
 */

#include "FLIPParticle.h"

FLIPParticle::FLIPParticle(const vector2 _position, const vector2 _velocity) {
  position = _position;
  velocity = _velocity;
  mass = 1.0f;
  color = {1.0, 0.0, 0.0};
}
