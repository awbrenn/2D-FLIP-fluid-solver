//
// Created by awbrenn on 3/22/16.
//

#ifndef FLIP_PARTICLE_FLIPOCCUPANCYVOLUME_H
#define FLIP_PARTICLE_FLIPOCCUPANCYVOLUME_H

#include <vector>
#include <stdlib.h>
#include "vector2.h"
#include "FLIPParticle.h"

class FLIPOccupancyVolume {
    public:
        vector2 ovllc;
        vector2 ovurc;
        int nx;
        int ny;
        float dx;
        float dy;
        std::vector<std::vector<size_t>> cells;

        // functions
        std::vector<size_t> *getCell(FLIPParticle *p);
        void populateOccupancyVolume(std::vector<FLIPParticle> *particles);
        void getIndicesOfAllPossibleCollisions(FLIPParticle *p, std::vector<size_t> *cell_indices);

    private:
        unsigned int getCellIndex(FLIPParticle *p);
};

#endif //FLIP_PARTICLE_FLIPOCCUPANCYVOLUME_H
