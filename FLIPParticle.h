/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       3/8/2016
 */

#ifndef SPHPARTICLE_H
#define SPHPARTICLE_H
#include "vector2.h"

typedef struct vector3 {
  float x, y, z;
} vector3;

class SPHParticle {
  public:
    vector2 position;
    vector2 velocity;
    vector2 acceleration;
    vector3 color;
    float mass;
    float density;
    float pressure;

    SPHParticle(const vector2 _position, const vector2 _velocity);
};


#endif //SPHPARTICLE_H
