/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/23/2016
 */

#include "FLIPSolver.h"

void handleError(const char *error_message, bool kill) {
  std::cerr << error_message << std::endl;
  if (kill) exit(-1);
}


float getRandomFloatBetweenValues (float lower_bound, float upper_bound) {
  return lower_bound + static_cast <float> (rand()) / ((RAND_MAX / (upper_bound - lower_bound)));
}


SPHSolver::SPHSolver(unsigned int number_of_particles, const float _lower_bound, const float _upper_bound, const float _h) {
  lower_bound = _lower_bound;
  upper_bound = _upper_bound;
  dampening = 1.0f;
  update_function = SIXTH;
  party_mode = false;
  occupancy_volume = new SPHOccupancyVolume();
  h = _h;

  // initialize particles
  for (unsigned int i = 0; i < number_of_particles; ++i) {
    vector2 position;
    position.x = getRandomFloatBetweenValues(lower_bound+0.5f, upper_bound-0.5f);
    position.y = getRandomFloatBetweenValues(lower_bound+1.0f, upper_bound-0.5f);
    vector2 velocity = {0.0f, 0.0f};
    particles.push_back(SPHParticle(position, velocity));
    velocity_grid.push_back(velocity);
  }
}


void SPHSolver::randomizeColor(SPHParticle *p) {
  p->color.x = getRandomFloatBetweenValues(0.1, 0.9);
  p->color.y = getRandomFloatBetweenValues(0.1, 0.9);
  p->color.z = getRandomFloatBetweenValues(0.1, 0.9);
}


void SPHSolver::enforceBoundary(SPHParticle *p) {
  bool collision = false;

  // enforce top and bottom bounds
  if (p->position.y < lower_bound) {
    p->position.y = lower_bound - p->position.y;
    p->velocity.y = (-1.0f)*p->velocity.y*dampening;
    collision = true;
  }
  else if (p->position.y > upper_bound) {
    p->position.y = upper_bound - (p->position.y - upper_bound);
    p->velocity.y = (-1.0f)*p->velocity.y*dampening;
    collision = true;
  }

  // enforce left to right bounds
  if (p->position.x < lower_bound) {
    p->position.x = lower_bound - p->position.x;
    p->velocity.x = (-1.0f)*p->velocity.x*dampening;
    collision = true;
  }
  else if (p->position.x > upper_bound) {
    p->position.x = upper_bound - (p->position.x - upper_bound);
    p->velocity.x = (-1.0f)*p->velocity.x*dampening;
    collision = true;
  }
  if (collision && party_mode) { randomizeColor(p); }
}


float SPHSolver::getInfluence(vector2 xb, vector2 xa) {
  float q, result;

  q = ((xa-xb).length())/h;

  if (q >= 1.0f) { result = 0.0f; }
  else { result = (float)((10.0f / (M_PI * (h * h))) * ((1.0f - q) * (1.0f - q) * (1.0f - q)));}

  return result;
}


void SPHSolver::calculateDensity (SPHParticle *b) {
  SPHParticle *a;
  std::vector<size_t> check_indices;
  occupancy_volume->getIndicesOfAllPossibleCollisions(b, &check_indices);

  b->density = 0.0f;
  for (unsigned int i = 0; i < check_indices.size(); ++i) {
    a = &particles[check_indices.at(i)];
    b->density += a->mass * getInfluence(b->position, a->position);
  }
}


void SPHSolver::createOccupancyVolume(vector2 ovllc, vector2 ovurc) {
  unsigned int ovnx, ovny;
  float ovdx, ovdy;

  ovnx = (unsigned int)(((ovurc.x - ovllc.x) / h) + 1);
  ovny = (unsigned int)(((ovurc.y - ovllc.y) / h) + 1);

  ovdx = (ovurc.x - ovllc.x) / ((float)(ovnx - 1));
  ovdy = (ovurc.y - ovllc.y) / ((float)(ovny - 1));

  occupancy_volume->nx = ovnx;
  occupancy_volume->ny = ovny;
  occupancy_volume->dx = ovdx;
  occupancy_volume->dy = ovdy;

  occupancy_volume->cells.clear();
  occupancy_volume->cells.resize(ovnx*ovny);
}


void SPHSolver::leapFrog(float dt) {
  // occupancy volume lower left corner and upper right corner
  vector2 ovllc = vector2(upper_bound, upper_bound);
  vector2 ovurc = vector2(lower_bound, lower_bound);

  dt /= 2.0f;
  std::vector<SPHParticle>::iterator pi = particles.begin();

  while(pi != particles.end()) {
    pi->position.x += pi->velocity.x * dt;
    pi->position.y += pi->velocity.y * dt;
    enforceBoundary(&(*pi));

    if (ovllc.x > pi->position.x) { ovllc.x = pi->position.x; }
    if (ovllc.y > pi->position.y) { ovllc.y = pi->position.y; }
    if (ovurc.x < pi->position.x) { ovurc.x = pi->position.x; }
    if (ovurc.y < pi->position.y) { ovurc.y = pi->position.y; }

    ++pi;
  }

  occupancy_volume->ovllc = ovllc;
  occupancy_volume->ovurc = ovurc;

  // create and populate the occupancy volume
  createOccupancyVolume(ovllc, ovurc);
  occupancy_volume->populateOccupancyVolume(&particles);

  pi = particles.begin();
  while(pi != particles.end()) {
    calculateDensity(&(*pi));
    ++pi;
  }

  highest_velocity = 0.0001;
  highest_density = 0.0001;

  pi = particles.begin();
  while(pi != particles.end()) {
    pi->acceleration = force.evaluateForce(&particles, &(*pi), occupancy_volume, h);
    pi->velocity.x += pi->acceleration.x*dt*2.0f;
    pi->velocity.y += pi->acceleration.y*dt*2.0f;
    pi->position.x += pi->velocity.x*dt;
    pi->position.y += pi->velocity.y*dt;

    if (pi->velocity.length() > highest_velocity) { highest_velocity = pi->velocity.length(); }
    if (pi->density > highest_density) { highest_density = pi->density; }

    enforceBoundary(&(*pi));
    ++pi;
  }

  pi = particles.begin();
  while(pi != particles.end()) {
    float base_percentage, red_percentage, blue_percentage;
    base_percentage = pi->velocity.length() / highest_velocity;

    red_percentage = base_percentage;
    blue_percentage = 1.0f - red_percentage;

    pi->color.x = red_percentage;
    pi->color.y = 0.0f;
    pi->color.z = blue_percentage;

    ++pi;
  }
}


void SPHSolver::sixth(float dt) {
  float a, b;
  a = 1.0f / (4.0f - powf(4.0f, 1.0f/3.0f));
  b = 1.0f - 4.0f*a;

  leapFrog(a*dt);
  leapFrog(a*dt);
  leapFrog(b*dt);
  leapFrog(a*dt);
  leapFrog(a*dt);
}


void SPHSolver::update(const float dt) {
  switch (update_function) {
    case LEAP_FROG:
      leapFrog(dt);
      break;
    case SIXTH:
      sixth(dt);
      break;
    default:
      handleError("Error in SPHSolver::update(const float dt, UPDATE_FUNCTION function): Invalid UPDATE_FUNCTION. "
                        "Use LEAP_FROG or SIXTH", true);
      break;
  }
}
