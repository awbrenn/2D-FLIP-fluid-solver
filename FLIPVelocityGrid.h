//
// Created by awbrenn on 4/3/16.
//

#ifndef FLIP_SIMULATION_FLIPVELOCITYGRID_H
#define FLIP_SIMULATION_FLIPVELOCITYGRID_H


#include <vector>
#include "FLIPVelocityGridPoint.h"

class FLIPVelocityGrid {
  public:
    std::vector<FLIPVelocityGridPoint> grid;
    int grid_width;
    int grid_height;
    int nloops;
    int oploops;
    float dx;

    const float getDensity(int i, int j);
    const vector2 getVelocity(int i, int j);
    const float getPressure(int i, int j);
    const float getDivergence(int i, int j);
    void computeDivergence();
    void computePressure();
    void computePressureForces(int i, int j, float* force_x, float* force_y);
    void computeVelocityBasedOnPressureForces();
    void computeVelocity(vector2 constant_force, float dt);
    void updateGrid(vector2 constant_force, float dt);

    int gridIndex(int i, int j) const { return i+grid_width*j; }
};


#endif //FLIP_SIMULATION_FLIPVELOCITYGRID_H
