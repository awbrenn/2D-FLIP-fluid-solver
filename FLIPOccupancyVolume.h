//
// Created by awbrenn on 3/22/16.
//

#ifndef SPH_PARTICLE_SPHOCCUPANCYVOLUME_H
#define SPH_PARTICLE_SPHOCCUPANCYVOLUME_H

#include <vector>
#include <stdlib.h>
#include "vector2.h"
#include "FLIPParticle.h"

class SPHOccupancyVolume {
    public:
        vector2 ovllc;
        vector2 ovurc;
        int nx;
        int ny;
        float dx;
        float dy;
        std::vector<std::vector<size_t>> cells;

        // functions
        std::vector<size_t> *getCell(SPHParticle *p);
        void populateOccupancyVolume(std::vector<SPHParticle> *particles);
        void getIndicesOfAllPossibleCollisions(SPHParticle *p, std::vector<size_t> *cell_indices);

    private:
        unsigned int getCellIndex(SPHParticle *p);
};

#endif //SPH_PARTICLE_SPHOCCUPANCYVOLUME_H
