/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/8/2016
 */

#ifndef SPHFORCE_H
#define SPHFORCE_H
#include "FLIPParticle.h"
#include "FLIPOccupancyVolume.h"
#include <math.h>
#include <vector>
#include <omp.h>

class SPHForce {
  private:
    float calculatePressure(SPHParticle *p);
    float calculateViscocityForce(SPHParticle *b, SPHParticle *a, float h);

  public:
    vector2 gravity = vector2(0.0f, -9.8f);
    float density_base;
    float gamma;
    float beta;
    float viscosity;
    float epsilon;

    vector2 evaluateForce(std::vector<SPHParticle> *particles, SPHParticle *b,
                              SPHOccupancyVolume *occupancy_volume, float h);
};

#endif //SPHFORCE_H
