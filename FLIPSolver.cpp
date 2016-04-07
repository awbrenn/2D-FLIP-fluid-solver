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


FLIPSolver::FLIPSolver(unsigned int number_of_particles, const float _lower_bound, const float _upper_bound,
                       const float _h, const float _dx, const int _nloops, const int _oploops) {
  lower_bound = _lower_bound;
  upper_bound = _upper_bound;
  dampening = 1.0f;
  update_function = SIXTH;
  party_mode = false;
  occupancy_volume = new FLIPOccupancyVolume();
  h = _h;

  velocity_grid.nloops = _nloops;
  velocity_grid.oploops = _oploops;

  // initialize particles
  for (unsigned int i = 0; i < number_of_particles; ++i) {
    vector2 position;
    position.x = getRandomFloatBetweenValues(lower_bound+0.5f, upper_bound-0.5f);
    position.y = getRandomFloatBetweenValues(lower_bound+1.0f, upper_bound-0.5f);
    vector2 velocity = {0.0f, 0.0f};
    particles.push_back(FLIPParticle(position, velocity));
  }

  velocity_grid.dx = _dx;

  velocity_grid.grid_width = (int)((upper_bound / velocity_grid.dx) + 1);
  velocity_grid.grid_height = (int)((upper_bound / velocity_grid.dx) + 1);

  // initalize grid
  for (unsigned int i = 0; i < velocity_grid.grid_width * velocity_grid.grid_width; ++i) {
    FLIPVelocityGridPoint grid_point;
    grid_point.velocity = vector2(0.0f, 0.0f);
    grid_point.position.x = (i % velocity_grid.grid_width) * velocity_grid.dx;
    grid_point.position.y = (i / velocity_grid.grid_width) * velocity_grid.dx;

    velocity_grid.grid.push_back(grid_point);
  }
}


void FLIPSolver::randomizeColor(FLIPParticle *p) {
  p->color.x = getRandomFloatBetweenValues(0.1, 0.9);
  p->color.y = getRandomFloatBetweenValues(0.1, 0.9);
  p->color.z = getRandomFloatBetweenValues(0.1, 0.9);
}


void FLIPSolver::enforceBoundary(FLIPParticle *p) {
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


float FLIPSolver::getInfluence(vector2 xb, vector2 xa) {
  float q, result;

  q = ((xa-xb).length())/h;

  if (q >= 1.0f) { result = 0.0f; }
  else { result = (float)((10.0f / (M_PI * (h * h))) * ((1.0f - q) * (1.0f - q) * (1.0f - q)));}

  return result;
}


//void FLIPSolver::calculateDensity (FLIPParticle *b) {
//  FLIPParticle *a;
//  std::vector<size_t> check_indices;
//  occupancy_volume->getIndicesOfAllPossibleCollisions(b, &check_indices);
//
//  b->density = 0.0f;
//  for (unsigned int i = 0; i < check_indices.size(); ++i) {
//    a = &particles[check_indices.at(i)];
//    b->density += a->mass * getInfluence(b->position, a->position);
//  }
//}


void FLIPSolver::constructOccupancyVolume(vector2 ovllc, vector2 ovurc) {
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

void FLIPSolver::constructVelocityGrid() {
  float influence, total_influence;
  bool particle_within_radius;
  std::vector<FLIPVelocityGridPoint>::iterator vi = velocity_grid.grid.begin();
  std::vector<FLIPParticle>::iterator pi;

  int i = 0;

  while(vi != velocity_grid.grid.end()) {
    particle_within_radius = false;
    total_influence = 0.0f;
    vi->velocity = 0.0f;
    vi->velocity = 0.0f;

    pi = particles.begin();

    while(pi != particles.end()) {
      influence = getInfluence(vi->position, pi->position);
      total_influence += influence;

      vi->velocity += pi->velocity.x * influence;
      vi->velocity += pi->velocity.y * influence;
      if (influence > 0.0f) {
        particle_within_radius = true;
      }
      ++i;
      ++pi;
    }

    // if a particle exists in that grid region add density;
    if (particle_within_radius) { vi->density = 1.0f; }

    if (total_influence != 0.0f) { vi->velocity.scale(1.0f / total_influence); }

    ++vi;
  }
}


int i = 0;

void FLIPSolver::updateParticleVelocity(const float dt) {
  std::vector<FLIPParticle>::iterator pi = particles.begin();

  while(pi != particles.end()) {

    pi->velocity = velocity_grid.interpolateVelocityFromGridToParticle(&(*pi));

    if ((i % 500) == 0)
      std::cout << pi->velocity.x << " " << pi->velocity.y << " " << i % 500 << std::endl;
    ++i;
    ++pi;
  }

  pi = particles.begin();
  while(pi != particles.end()) {
    pi->position = pi->position * pi->velocity.scale(dt);
    enforceBoundary(&(*pi));

//    std::cout << pi->position.x << " " << pi->position.y << std::endl;
    ++pi;
  }
}


void FLIPSolver::update(const float dt) {
//  switch (update_function) {
//    case LEAP_FROG:
//      leapFrog(dt);
//      break;
//    case SIXTH:
//      sixth(dt);
//      break;
//    default:
//      handleError("Error in FLIPSolver::update(const float dt, UPDATE_FUNCTION function): Invalid UPDATE_FUNCTION. "
//                        "Use LEAP_FROG or SIXTH", true);
//      break;
//  }

  // 1) construct velocity on the grid
  constructVelocityGrid();

  // 2) add forces to velocity & 3) Create incompressible velocity
  vector2 gravity = vector2(0.0f, -9.8f);
  velocity_grid.updateGrid(gravity, dt);

  // 4) simplistic update
  updateParticleVelocity(dt);

  // 5) move particles

}
