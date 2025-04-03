#ifndef LINEAR_QUADTREE_H
#define LINEAR_QUADTREE_H

#include <vector>
#include <cstdint>
#include <unordered_map>

#include "particle.h"

class LinearQuadTree
{
public:
    struct QuadtreeNode
    {
        uint32_t mortonCode;
        size_t particleIndex;
    };

    LinearQuadTree(std::vector<Particle> &aParticles);

    void initialize();
    void update();
    std::vector<size_t> findNeighbors(const Particle &aQueryParticle, float aSearchRadius) const;

private:
    void build();
    uint32_t interleaveBits(uint32_t x) const;
    uint32_t mortonEncode(float x, float y, float gridSize) const;

    std::vector<QuadtreeNode> mNodes;
    std::unordered_map<size_t, uint32_t> mMortonMap;
    std::vector<Particle> &mParticles;

    float mGridSize;
};

#endif // LINEAR_QUADTREE_H
