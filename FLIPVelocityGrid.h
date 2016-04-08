//
// Created by awbrenn on 4/3/16.
//

#ifndef FLIP_SIMULATION_FLIPVELOCITYGRID_H
#define FLIP_SIMULATION_FLIPVELOCITYGRID_H


#include <vector>
#include "FLIPVelocityGridPoint.h"
#include "FLIPParticle.h"
#include <math.h>

class FLIPVelocityGrid {
  private:
    void advectDensity(float dt);

  public:
    std::vector<FLIPVelocityGridPoint> grid;
    int grid_width;
    int grid_height;
    int nloops;
    int oploops;
    float dx;
    float *density_copy;

    const float getDensity(int i, int j);
    vector2 getVelocity(int i, int j);
    const float getPressure(int i, int j);
    const float getDivergence(int i, int j);
    void computeDivergence();
    void computePressure();
    void computePressureForces(int i, int j, float* force_x, float* force_y);
    void computeVelocityBasedOnPressureForces();
    void computeVelocity(vector2 constant_force, float dt);
    void updateGrid(vector2 constant_force, float dt);
    vector2 interpolateVelocityFromGridToParticle(FLIPParticle *particle);
    void bilinearlyInterpolate(const int ii, const int jj, const float x, const float y);
    const float InterpolateDensity(int i, int j, float w1, float w2, float w3, float w4);
    const float getDensityCopy(int i, int j);

    int gridIndex(int i, int j) const { return i+grid_width*j; }
};


#endif //FLIP_SIMULATION_FLIPVELOCITYGRID_H
