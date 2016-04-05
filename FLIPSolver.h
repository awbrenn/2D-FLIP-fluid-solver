/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/8/2016
 */

#ifndef FLIPSOLVER_H
#define FLIPSOLVER_H

#include "FLIPParticle.h"
#include "FLIPForce.h"
#include "math.h"
#include "FLIPOccupancyVolume.h"
#include "FLIPVelocityGrid.h"
#include <iostream>

enum UPDATE_FUNCTION {LEAP_FROG, SIXTH};

class FLIPSolver {
  private:
    void randomizeColor(FLIPParticle *p);
    void enforceBoundary(FLIPParticle *p);
    void calculateDensity (FLIPParticle *b);
    float getInfluence(vector2 xb, vector2 xa);
    void constructOccupancyVolume(vector2 ovllc, vector2 ovurc);
    void constructVelocityGrid();
    void updateParticleVelocity(const float dt);
//    void leapFrog(float dt);
//    void sixth(float dt);

  public:
    UPDATE_FUNCTION update_function;
    float upper_bound;
    float lower_bound;
    float highest_velocity;
    float highest_density;
    float dampening;
    float h;
    bool party_mode;
    FLIPVelocityGrid velocity_grid;
    std::vector<FLIPParticle> particles;
    FLIPOccupancyVolume *occupancy_volume;
    FLIPForce force;


    FLIPSolver(unsigned int number_of_particles, const float upper_bound, const float lower_bound,
                   const float h, const float dx, const int nloops, const int oploops);
    void update(const float dt);
};

#endif //FLIPSOLVER_H
