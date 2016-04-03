/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/8/2016
 */

#ifndef FLIPFORCE_H
#define FLIPFORCE_H
#include "FLIPParticle.h"
#include "FLIPOccupancyVolume.h"
#include <math.h>
#include <vector>
#include <omp.h>

class FLIPForce {
  private:
    float calculatePressure(FLIPParticle *p);
    float calculateViscocityForce(FLIPParticle *b, FLIPParticle *a, float h);

  public:
    vector2 gravity = vector2(0.0f, -9.8f);
    float density_base;
    float gamma;
    float beta;
    float viscosity;
    float epsilon;

    vector2 evaluateForce(std::vector<FLIPParticle> *particles, FLIPParticle *b,
                              FLIPOccupancyVolume *occupancy_volume, float h);
};

#endif //FLIPFORCE_H
