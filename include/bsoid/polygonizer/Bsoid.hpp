#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_BSOID_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_BSOID_HPP

#pragma once

#include "Polygonizer.hpp"
#include "Lattice.hpp"
#include "SuperVoxel.hpp"
#include "bsoid/tree/BlobTree.hpp"

#include <atlas/utils/Mesh.hpp>

#include <sstream>
#include <string>
#include <cinttypes>
#include <unordered_map>
#include <mutex>

namespace bsoid
{
    namespace polygonizer
    {
        class Bsoid
        {
        public:
            Bsoid();
            Bsoid(tree::BlobTree const& model, std::string const& name,
                float isoValue = 0.5f);
            Bsoid(Bsoid&& b);

            ~Bsoid() = default;

            void setModel(tree::BlobTree const& tree);
            void setIsoValue(float isoValue);

            tree::BlobTree* tree() const;

            void constructLattice();
            void constructMesh();
            void polygonize();

            Lattice const& getLattice() const;
            atlas::utils::Mesh& getMesh();

            void setName(std::string const& name);
            std::string getName() const;

            std::string getLog() const;
            void clearLog();

            void saveMesh();

        private:
            atlas::math::Point createCellPoint(glm::u64vec3 const& p,
                atlas::math::Point const& delta);
            atlas::math::Point createCellPoint(std::uint64_t x,
                std::uint64_t y, std::uint64_t z, atlas::math::Point const& delta);

            FieldPoint findVoxelPoint(PointId const& id);
            void fillVoxel(Voxel& v);
            bool seenVoxel(VoxelId const& id);

            void marchVoxelOnSurface(std::vector<Voxel> const& seeds);
            bool validVoxel(Voxel const& v);


            atlas::math::Point mGridDelta, mSvDelta, mMin, mMax;
            std::uint64_t mGridSize, mSvSize;
            float mMagic;

            std::vector<Voxel> mVoxels;
            std::mutex mSeenVoxelsMutex;
            std::map<std::uint64_t, VoxelId> mSeenVoxels;

            std::mutex mSeenPointsMutex;
            std::map<std::uint64_t, FieldPoint> mSeenPoints;

            std::mutex mSvMutex;
            std::unordered_map<std::uint64_t, SuperVoxel> mSuperVoxels;

            Lattice mLattice;
            tree::TreePointer mTree;

            atlas::utils::Mesh mMesh;

            std::stringstream mLog;
            std::string mName;
        };
    }
}

#endif