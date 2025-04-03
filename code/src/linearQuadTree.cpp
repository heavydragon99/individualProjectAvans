#include "linearQuadTree.h"

#include "simulationConfig.h"

#include <algorithm>
#include <cmath>

LinearQuadTree::LinearQuadTree(std::vector<Particle> &aParticles) : mParticles(aParticles)
{
    float gameSizeX = SimulationConfig::getInstance().gameSize().x;
}

void LinearQuadTree::initialize()
{
    mNodes.clear();
    mMortonMap.clear();
    mNodes.reserve(mParticles.size());

    mGridSize = SimulationConfig::getInstance().smoothingRadius();

    // Build the initial quadtree
    build();
}

void LinearQuadTree::build()
{
    mNodes.clear();
    mMortonMap.clear();
    mNodes.reserve(mParticles.size());

    for (size_t i = 0; i < mParticles.size(); i++)
    {
        uint32_t code = mortonEncode(mParticles[i].mPosition.x, mParticles[i].mPosition.y, mGridSize);
        mNodes.push_back({code, i});
        mMortonMap[i] = code;
    }

    // Sort mNodes based on Morton code (ensuring spatial locality)
    std::sort(mNodes.begin(), mNodes.end(), [](const QuadtreeNode &a, const QuadtreeNode &b)
              { return a.mortonCode < b.mortonCode; });
}

void LinearQuadTree::update()
{
    bool needsSorting = false;

    for (auto &node : mNodes)
    {
        size_t index = node.particleIndex;
        uint32_t newCode = mortonEncode(mParticles[index].mPosition.x, mParticles[index].mPosition.y, mGridSize);

        if (newCode != mMortonMap[index])
        {
            node.mortonCode = newCode;
            mMortonMap[index] = newCode;
            needsSorting = true;
        }
    }

    if (needsSorting)
    {
        std::sort(mNodes.begin(), mNodes.end(), [](const QuadtreeNode &a, const QuadtreeNode &b)
                  { return a.mortonCode < b.mortonCode; });
    }
}

std::vector<size_t> LinearQuadTree::findNeighbors(const Particle &aQueryParticle, float aSearchRadius) const
{
    uint32_t mortonCode = mortonEncode(aQueryParticle.mPosition.x, aQueryParticle.mPosition.y, mGridSize);
    std::vector<size_t> neighbors;

    // Search the particle's cell and adjacent cells
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            QuadtreeNode neighborNode = {mortonCode + dx + dy * mGridSize, 0};
            auto range = std::equal_range(
                mNodes.begin(), mNodes.end(), neighborNode,
                [](const QuadtreeNode &a, const QuadtreeNode &b)
                { return a.mortonCode < b.mortonCode; });

            for (auto it = range.first; it != range.second; ++it)
            {
                int neighborIndex = it->particleIndex;
                float dist = std::hypot(
                    mParticles[neighborIndex].mPosition.x - aQueryParticle.mPosition.x,
                    mParticles[neighborIndex].mPosition.y - aQueryParticle.mPosition.y);
                if (dist < aSearchRadius)
                {
                    // Check if the particle is already in the neighbors list
                    if (std::find(neighbors.begin(), neighbors.end(), neighborIndex) == neighbors.end())
                    {
                        neighbors.push_back(neighborIndex);
                    }
                }
            }
        }
    }

    return neighbors;
}

// Helper function to interleave bits for Morton code calculation
uint32_t LinearQuadTree::interleaveBits(uint32_t x) const
{
    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;
    return x;
}

// Convert 2D coordinates to Morton code
uint32_t LinearQuadTree::mortonEncode(float x, float y, float mGridSize) const
{
    uint32_t xi = static_cast<uint32_t>(x / mGridSize);
    uint32_t yi = static_cast<uint32_t>(y / mGridSize);
    return (interleaveBits(xi) | (interleaveBits(yi) << 1));
}
