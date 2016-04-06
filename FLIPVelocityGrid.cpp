//
// Created by awbrenn on 4/3/16.
//

#include <complex>
#include "FLIPVelocityGrid.h"

const float FLIPVelocityGrid::getDensity(int i, int j) {
  if (i < grid_width && i >= 0 && j < grid_height && j >= 0)
    return grid[gridIndex(i, j)].density;
  else
    return 0.0;
}


vector2 FLIPVelocityGrid::getVelocity(int i, int j) {
  if (i < grid_width && i >= 0 && j < grid_height && j >= 0)
    return grid[gridIndex(i, j)].velocity;
  else
    return 0.0;
}

const float FLIPVelocityGrid::getPressure(int i, int j) {
  if (i < grid_width && i >= 0 && j < grid_height && j >= 0)
    return grid[gridIndex(i, j)].pressure;
  else
    return 0.0;
}


const float FLIPVelocityGrid::getDivergence(int i, int j) {
  if (i < grid_width && i >= 0 && j < grid_height && j >= 0)
    return grid[gridIndex(i, j)].divergence;
  else
    return 0.0;
}


void FLIPVelocityGrid::computeDivergence() {
  int index;
  for (int j = 0; j < grid_height; ++j) {
    for (int i = 0; i < grid_width; ++i) {
      index = gridIndex(i, j);
      grid[index].divergence = (getVelocity(i + 1, j).x -
                                getVelocity(i - 1, j).x) / (2 * dx) +
                               (getVelocity(i, j + 1).y -
                                getVelocity(i, j - 1).y) / (2 * dx);


    }
  }
}


void FLIPVelocityGrid::computePressure() {
  for (int j = 0; j < grid_height; ++j) {
    for (int i = 0; i < grid_width; ++i) {
      grid[gridIndex(i, j)].pressure = 0.0f;
    }
  }

  for (int k = 0; k < nloops; ++k) {
    for (int j = 0; j < grid_height; ++j) {
      for (int i = 0; i < grid_width; ++i) {
        grid[gridIndex(i, j)].pressure = ((getPressure(i + 1, j) +
                                           getPressure(i - 1, j) +
                                           getPressure(i, j + 1) +
                                           getPressure(i, j - 1)) *
                                           0.25f) - ((dx * dx / 4.0f) * getDivergence(i, j));
      }
    }
  }
}


void FLIPVelocityGrid::computePressureForces(int i, int j, float *force_x, float *force_y) {
  *force_x = (getPressure(i + 1, j) - getPressure(i - 1, j)) / (2 * dx);
  *force_y = (getPressure(i, j + 1) - getPressure(i, j - 1)) / (2 * dx);
}


void FLIPVelocityGrid::computeVelocityBasedOnPressureForces() {
  float force_x, force_y;

  for (int j = 0; j < grid_height; ++j) {
    for (int i = 0; i < grid_width; ++i) {
      computePressureForces(i, j, &force_x, &force_y);
      grid[gridIndex(i, j)].velocity.x -= force_x;
      grid[gridIndex(i, j)].velocity.y -= force_y;
    }
  }
}


void FLIPVelocityGrid::computeVelocity(vector2 constant_force, float dt) {
  for (int j=0; j<grid_height; ++j) {
    for (int i=0; i<grid_width; ++i) {
      grid[gridIndex(i, j)].velocity.x += (constant_force.x * grid[gridIndex(i, j)].density * dt);
      grid[gridIndex(i, j)].velocity.y += (constant_force.y * grid[gridIndex(i, j)].density * dt);

//      if (getDensity(i, j) > 0.0f) {
//        printf("got here\n");
//      }
    }
  }
}


void FLIPVelocityGrid::updateGrid(vector2 constant_force, float dt) {
  // compute sources
  computeVelocity(constant_force, dt);

  for (int i = 0; i < oploops; ++i) {
    computeDivergence();
    computePressure();
    computeVelocityBasedOnPressureForces();
  }
}


vector2 FLIPVelocityGrid::interpolateVelocityFromGridToParticle(FLIPParticle *particle) {
    vector2 velocity = vector2(0.0f, 0.0f);

    // get index of sample
    const int i = (int) (particle->position.x / dx);
    const int j = (int) (particle->position.y / dx);

    // get weights of samples
    const float ax = std::abs(particle->position.x / dx - i);
    const float ay = std::abs(particle->position.y / dx - j);
    const float w1 = (1-ax) * (1-ay);
    const float w2 =    ax  * (1-ay);
    const float w3 = (1-ax) *    ay;
    const float w4 =    ax  *    ay;

    velocity = getVelocity(i    , j    ).scale(w1) +
               getVelocity(i + 1, j    ).scale(w2) +
               getVelocity(i    , j + 1).scale(w3) +
               getVelocity(i + 1, j + 1).scale(w4);

    return velocity;
}