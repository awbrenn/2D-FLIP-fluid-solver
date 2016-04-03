/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/8/2016
 */

#ifndef SPHSOLVER_H
#define SPHSOLVER_H

#include "FLIPParticle.h"
#include "FLIPForce.h"
#include "math.h"
#include "FLIPOccupancyVolume.h"
#include "FLIPVelocityGridPoint.h"
#include <iostream>

enum UPDATE_FUNCTION {LEAP_FROG, SIXTH};

class SPHSolver {
  private:
    void randomizeColor(SPHParticle *p);
    void enforceBoundary(SPHParticle *p);
    void calculateDensity (SPHParticle *b);
    float getInfluence(vector2 xb, vector2 xa);
    void constructOccupancyVolume(vector2 ovllc, vector2 ovurc);
    void constructVelocityGrid();
    void leapFrog(float dt);
    void sixth(float dt);

  public:
    UPDATE_FUNCTION update_function;
    float upper_bound;
    float lower_bound;
    float highest_velocity;
    float highest_density;
    float dampening;
    float h;
    bool party_mode;
    std::vector<FLIPVelocityGridPoint> velocity_grid;
    std::vector<SPHParticle> particles;
    SPHOccupancyVolume *occupancy_volume;
    SPHForce force;


    SPHSolver(unsigned int number_of_particles, const float upper_bound, const float lower_bound, const float h);
    void update(const float dt);
};

#endif //SPHSOLVER_H
