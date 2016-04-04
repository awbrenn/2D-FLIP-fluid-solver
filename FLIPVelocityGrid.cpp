//
// Created by awbrenn on 4/3/16.
//

#include "FLIPVelocityGrid.h"

const float FLIPVelocityGrid::getDensity(int i, int j)
{
    if (i < grid_width && i >=0 && j < grid_height && j >= 0)
        return grid[gridIndex(i,j)].density;
    else
        return 0.0;
}


const vector2 FLIPVelocityGrid::getVelocity(int i, int j)
{
    if (i < grid_width && i >=0 && j < grid_height && j >= 0)
        return grid[gridIndex(i,j)].velocity;
    else
        return 0.0;
}

const float FLIPVelocityGrid::getPressure(int i, int j)
{
    if (i < grid_width && i >=0 && j < grid_height && j >= 0)
        return grid[gridIndex(i,j)].pressure;
    else
        return 0.0;
}


const float FLIPVelocityGrid::getDivergence(int i, int j)
{
    if (i < grid_width && i >=0 && j < grid_height && j >= 0)
        return grid[gridIndex(i,j)].divergence;
    else
        return 0.0;
}


void FLIPVelocityGrid::computeDivergence()  {
    int index;
    for (int j = 0; j < grid_height; ++j)
    {
        for (int i = 0; i < grid_width; ++i)
        {
            index = gridIndex(i,j);
            grid[index].divergence = (getVelocity(i+1, j  ).x -
                                      getVelocity(i-1, j  ).x) / (2*Dx) +
                                     (getVelocity(i,   j+1).y -
                                      getVelocity(i,   j-1).y) / (2*Dx);


        }
    }
}


void FLIPVelocityGrid::computePressure()
{
    for (int j = 0; j < grid_height; ++j)
    {
        for (int i = 0; i < grid_width; ++i) {
            grid[gridIndex(i,j)].pressure = 0.0f;
        }
    }

    for(int k = 0; k < nloops; ++k)
    {
        for (int j = 0; j < grid_height; ++j)
        {
            for (int i = 0; i < grid_width; ++i)
            {
                grid[gridIndex(i,j)].pressure = ((getPressure(i+1, j)    +
                                                  getPressure(i-1, j)    +
                                                  getPressure(i,   j+1)  +
                                                  getPressure(i,   j-1)) *
                                                  0.25f) - ((Dx*Dx/4.0f) * getDivergence(i,j));
            }
        }
    }
}


void FLIPVelocityGrid::computePressureForces(int i, int j, float* force_x, float* force_y)
{
    *force_x = (getPressure(i+1, j) - getPressure(i-1, j)) / (2*Dx);
    *force_y = (getPressure(i, j+1) - getPressure(i, j-1)) / (2*Dx);
}


void FLIPVelocityGrid::computeVelocityBasedOnPressureForces()
{
    float force_x, force_y;

    for (int j = 0; j < grid_height; ++j)
    {
        for (int i = 0; i < grid_width; ++i)
        {
            computePressureForces(i, j, &force_x, &force_y);
            grid[gridIndex(i,j)].velocity.x -= force_x;
            grid[gridIndex(i,j)].velocity.y -= force_y;
        }
    }
}