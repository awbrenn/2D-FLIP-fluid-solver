//
// Created by awbrenn on 4/3/16.
//

#ifndef FLIP_SIMULATION_FLIPVELOCITYGRIDPOINT_H
#define FLIP_SIMULATION_FLIPVELOCITYGRIDPOINT_H

#include "vector2.h"

class FLIPVelocityGridPoint {
  public:
    vector2 velocity;
    vector2 position;
    float density;
    float pressure;
    float divergence;
};


#endif //FLIP_SIMULATION_FLIPVELOCITYGRIDPOINT_H
