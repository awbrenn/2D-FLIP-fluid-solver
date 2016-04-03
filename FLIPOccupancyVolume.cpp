//
// Created by awbrenn on 3/22/16.
//

#include "FLIPOccupancyVolume.h"



unsigned int SPHOccupancyVolume::getCellIndex(SPHParticle *p) {
    unsigned int q_i, q_j, cell_index;
    vector2 q;

    q = p->position - ovllc;
    q_i = (unsigned int) (q.x/dx);
    q_j = (unsigned int) (q.y/dy);
    cell_index = q_i + (nx * q_j);

    return cell_index;
}


std::vector<size_t> *SPHOccupancyVolume::getCell(SPHParticle *p) {
    return &cells[getCellIndex(p)];
}

void SPHOccupancyVolume::getIndicesOfAllPossibleCollisions(SPHParticle *p, std::vector<size_t> *check_indices) {
    int q_i, q_j, cell_index;
    vector2 q;
    std::vector<size_t> cell_indices;

    q = p->position - ovllc;
    q_i = (unsigned int) (q.x/dx);
    q_j = (unsigned int) (q.y/dy);
    cell_index = q_i + (nx * q_j);

    // push this cell
    cell_indices.push_back((size_t) cell_index);

    // check / push top row
    if (q_i-1 >= 0 && q_j+1 <  ny ) { cell_indices.push_back((size_t) q_i-1 + (nx * (q_j+1))); }
    if (              q_j+1 <  ny ) { cell_indices.push_back((size_t) q_i   + (nx * (q_j+1))); }
    if (q_i+1 < nx && q_j+1 <  ny ) { cell_indices.push_back((size_t) q_i+1 + (nx * (q_j+1))); }

    // check / push middle row
    if (q_i-1 >= 0) { cell_indices.push_back((size_t) q_i-1 + (nx * (q_j))); }
    if (q_i+1 < nx) { cell_indices.push_back((size_t) q_i+1 + (nx * (q_j))); }

    // check / push bottem row
    if (q_i-1 >= 0 && q_j-1 >= 0 ) { cell_indices.push_back((size_t) q_i-1 + (nx * (q_j-1))); }
    if (              q_j-1 >= 0 ) { cell_indices.push_back((size_t) q_i   + (nx * (q_j-1))); }
    if (q_i+1 < nx && q_j-1 >= 0 ) { cell_indices.push_back((size_t) q_i+1 + (nx * (q_j-1))); }

    for(int i=0; i < cell_indices.size(); ++i) {
        for(int j=0; j < cells.at( cell_indices[i] ).size(); ++j) {
            check_indices->push_back(cells.at( cell_indices[i] ).at(j));
        }
    }
}

void SPHOccupancyVolume::populateOccupancyVolume(std::vector<SPHParticle> *particles) {
    std::vector<SPHParticle>::iterator pi = particles->begin();
    size_t i = 0;
    while(pi != particles->end()) {
        getCell(&(*pi))->push_back(i);
        ++i;
        ++pi;
    }
}