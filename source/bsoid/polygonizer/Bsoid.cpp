#include "bsoid/polygonizer/Bsoid.hpp"
#include "bsoid/polygonizer/Hash.hpp"
#include "bsoid/polygonizer/Tables.hpp"

#include <atlas/core/Timer.hpp>
#include <atlas/core/Macros.hpp>
#include <atlas/core/Assert.hpp>
#include <atlas/core/Log.hpp>
#include <atlas/core/Float.hpp>

#include <numeric>
#include <functional>
#include <unordered_set>
#include <fstream>
#include <queue>

#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <tbb/parallel_invoke.h>


namespace bsoid
{
    namespace polygonizer
    {
        Bsoid::Bsoid() :
            mName("model")
        { }

        Bsoid::Bsoid(tree::BlobTree const& model, std::string const& name,
            float isoValue) :
            mTree(std::make_unique<tree::BlobTree>(model)),
            mMagic(isoValue),
            mName(name)
        { }

        Bsoid::Bsoid(Bsoid&& b) :
            mLattice(std::move(b.mLattice)),
            mTree(std::move(b.mTree)),
            mMesh(std::move(b.mMesh)),
            mMagic(b.mMagic),
            mLog(std::move(b.mLog)),
            mName(b.mName)
        { }

        void Bsoid::setModel(tree::BlobTree const& model)
        {
            mTree = std::make_unique<tree::BlobTree>(model);
        }

        void Bsoid::setIsoValue(float isoValue)
        {
            mMagic = isoValue;
        }

        tree::BlobTree* Bsoid::tree() const
        {
            return mTree.get();
        }

        void Bsoid::constructLattice()
        {
            using atlas::math::Point;
            using atlas::utils::BBox;

            atlas::core::Timer<float> global;
            atlas::core::Timer<float> t;

            // First construct the grid of super-voxels.
            tbb::parallel_for(static_cast<std::uint64_t>(0), mSvSize, 
                [this](std::uint64_t x) {
                tbb::parallel_for(static_cast<std::uint64_t>(0), mSvSize,
                    [this, x](std::uint64_t y) {
                    tbb::parallel_for(static_cast<std::uint64_t>(0), mSvSize,
                        [this, x, y](std::uint64_t z)
                    {
                        auto pt = createCellPoint(x, y, z, mSvDelta);
                        BBox cell(pt, pt + mSvDelta);

                        SuperVoxel sv;
                        sv.field = mTree->getSubTree(cell);
                        sv.id = {x, y, z};

                        if (sv.field)
                        {
                            // critical section.
                            std::lock_guard<std::mutex> lock(mSvMutex);
                            auto idx = BsoidHash64::hash(x, y, z);
                            mSuperVoxels[idx] = sv;
                        }
                    });
                });
            });

            // Now that we have the grid of super-voxels, we can grab the seeds
            // and convert them into voxels in parallel.
            auto seedPoints = mTree->getSeeds();
            std::vector<Voxel> seedVoxels(seedPoints.size());
            tbb::parallel_for(static_cast<std::size_t>(0), seedVoxels.size(),
                [this, seedPoints, &seedVoxels](std::size_t i) 
            {
                auto pt = seedPoints[i];
                auto v = (pt - mMin) / mGridDelta;
                PointId id;
                id.x = static_cast<std::uint64_t>(v.x);
                id.y = static_cast<std::uint64_t>(v.y);
                id.z = static_cast<std::uint64_t>(v.z);
                seedVoxels[i] = Voxel(id);
            });

            // We have the seed voxels now, so we have to setup the 

        }

        
        void Bsoid::constructMesh()
        {
            using atlas::math::Point;

            atlas::core::Timer<float> global;
            atlas::core::Timer<float> t;
        }

        void Bsoid::polygonize()
        {
            using atlas::core::Timer;

            Timer<float> global;

            global.start();
            mLog << "Lattice generation.\n";
            mLog << "#===========================#\n";
            // Generate lattices.
            {
                constructLattice();
            }

            mLog << "\nMesh generation.\n";
            mLog << "#===========================#\n";
            {
                constructMesh();
            }

            mLog << "\nSummary:\n";
            mLog << "#===========================#\n";
            mLog << "Total runtime: " << global.elapsed() << " seconds\n";
            mLog << "Total vertices generated: " << mMesh.vertices().size() << "\n";
        }

        Lattice const& Bsoid::getLattice() const
        {
            return mLattice;
        }

        atlas::utils::Mesh& Bsoid::getMesh()
        {
            return mMesh;
        }

        void Bsoid::setName(std::string const& name)
        {
            mName = name;
        }

        std::string Bsoid::getName() const
        {
            return mName;
        }

        std::string Bsoid::getLog() const
        { 
            return mLog.str();
        }

        void Bsoid::clearLog()
        {
            mLog.str(std::string());
        }

        void Bsoid::saveMesh()
        {
            mMesh.saveToFile(mName + ".obj");
        }


        atlas::math::Point Bsoid::createCellPoint(std::uint64_t x,
            std::uint64_t y, std::uint64_t z, atlas::math::Point const& delta)
        {
            atlas::math::Point pt;
            atlas::math::Point p(x, y, z);
            auto const& start = mMin;

            pt = start + p * delta;
            return pt;
        }

        atlas::math::Point Bsoid::createCellPoint(glm::u64vec3 const& p,
            atlas::math::Point const& delta)
        {
            return createCellPoint(p.x, p.y, p.z, delta);
        }

        FieldPoint Bsoid::findVoxelPoint(PointId const& id)
        {
            using atlas::math::Point4;
            using atlas::math::Point;

            // First check if we have seen this point before.
            auto entry = mSeenPoints.find(BsoidHash64::hash(id.x, id.y, id.z));
            if (entry != mSeenPoints.end())
            {
                return (*entry).second;
            }
            else
            {
                auto pt = createCellPoint(id, mGridDelta);

                PointId svId;
                {
                    auto v = (pt - mMin) / mSvDelta;
                    svId.x = static_cast<std::uint64_t>(v.x);
                    svId.y = static_cast<std::uint64_t>(v.y);
                    svId.z = static_cast<std::uint64_t>(v.z);

                    // Check any of the coordinates of the id are beyond the edge
                    // of the grid.
                    svId.x = (svId.x < mSvSize) ? svId.x : svId.x - 1;
                    svId.y = (svId.y < mSvSize) ? svId.y : svId.y - 1;
                    svId.z = (svId.z < mSvSize) ? svId.z : svId.z - 1;
                }

                FieldPoint fp;
                {
                    auto svHash = BsoidHash64::hash(svId.x, svId.y, svId.z);
                    SuperVoxel sv = mSuperVoxels[svHash];
                    auto val = sv.eval(pt);
                    auto g = sv.grad(pt);
                    fp = { pt, val, g, svHash };
                }

                // Now that we have the point, let's add it to our list and
                // return it.
                {
                    std::lock_guard<std::mutex> lock(mSeenPointsMutex);
                    auto hash = BsoidHash64::hash(id.x, id.y, id.z);
                    mSeenPoints.insert(
                        std::pair<std::uint64_t, FieldPoint>(hash, fp));
                }

                return fp;
            }
        }

        void Bsoid::fillVoxel(Voxel& v)
        {
            tbb::parallel_for(static_cast<std::size_t>(0), 
                VoxelDecals.size(), [this, &v](std::size_t d) {
                auto decalId = v.id + VoxelDecals[d];
                v.points[d] = findVoxelPoint(decalId);
            });
        }

        bool Bsoid::seenVoxel(VoxelId const& id)
        {
            if (mSeenVoxels.find(BsoidHash64::hash(id.x, id.y, id.z)) !=
                mSeenVoxels.end())
            {
                return true;
            }
            else
            {
                std::lock_guard<std::mutex> lock(mSeenVoxelsMutex);
                mSeenVoxels.insert(
                    std::pair<std::uint64_t, VoxelId>(
                        BsoidHash64::hash(id.x, id.y, id.z), id));
                return false;
            }
        }

        void Bsoid::marchVoxelOnSurface(std::vector<Voxel> const& seeds)
        {
            using atlas::math::Point4;
            using atlas::math::Point;

            auto getEdges = [this](Voxel const& v)
            {
                FieldPoint start, end;
                int edgeId = 0;
                std::vector<int> edges;
                // Could be done with a parallel_for.
                for (std::size_t i = 0; i < v.points.size(); ++i)
                {
                    start = v.points[i];
                    end = v.points[(i + 1) % v.points.size()];
                    float val1 = start.value.w - mMagic;
                    float val2 = end.value.w - mMagic;

                    if (glm::sign(val1) != glm::sign(val2))
                    {
                        edges.push_back(edgeId);
                    }
                    edgeId++;
                }

                return edges;
            };

            if (seeds.empty())
            {
                return;
            }

            std::queue<VoxelId> frontier;
            std::mutex frontierMutex;
            {
                auto containsSurface = [this, getEdges](Voxel const& v)
                {
                    Voxel voxel = v;
                    fillVoxel(voxel);
                    auto edges = getEdges(voxel);
                    return !edges.empty();
                };

                auto findSurface = [this, containsSurface](Voxel const& v)
                {
                    bool found = false;
                    Voxel last, current;
                    current = v;

                    while (!found)
                    {
                        auto cPos = (2llu * v.id) + glm::u64vec3(1, 1, 1);
                        Point origin = createCellPoint(cPos, mGridDelta / 2.0f);
                        float originVal = mTree->eval(origin);
                        auto norm = mTree->grad(origin);
                        norm = glm::normalize(norm);
                        norm = (originVal > mMagic) ? -norm : norm;

                        // Now find the voxel that we are pointing to.
                        auto absNorm = glm::abs(norm);
                        Point next;
                        // Somehow figure out which voxel to move to.

                        current.id.x += static_cast<std::uint64_t>(next.x);
                        current.id.y += static_cast<std::uint64_t>(next.y);
                        current.id.z += static_cast<std::uint64_t>(next.z);

                        // Check if the voxel hasn't run off the edge of the grid.
                        if (!validVoxel(current))
                        {
                            break;
                        }

                        if (containsSurface(current))
                        {
                            break;
                        }
                    }

                    return current;
                };

                tbb::parallel_for(seeds.begin(), seeds.end(),
                    [this, containsSurface, findSurface, &frontierMutex, 
                    &frontier](Voxel& seed) {
                    auto v = seed;
                    if (!containsSurface(seed))
                    {
                        v = findSurface(v);
                        if (!validVoxel(v))
                        {
                            return;
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lock(frontierMutex);
                        frontier.push(v.id);
                    }
                });
            }

            // See whether there is a sensible way of parallelizing this later.
            if (frontier.empty())
            {
                DEBUG_LOG("Exiting on empty queue.");
                return;
            }

            while (!frontier.empty())
            {
                auto top = frontier.front();
                frontier.pop();

                if (seenVoxel(top))
                {
                    continue;
                }

                Voxel v(top);
                fillVoxel(v);

                auto edges = getEdges(v);
                if (edges.empty())
                {
                    continue;
                }

                for (auto& edge : edges)
                {
                    auto decal = EdgeDecals.at(edge);

                    auto neighbourDecal = v.id;
                    neighbourDecal.x += decal.x;
                    neighbourDecal.y += decal.y;
                    neighbourDecal.z += decal.z;

                    if (!validVoxel(Voxel(neighbourDecal)))
                    {
                        continue;
                    }

                    frontier.push(neighbourDecal);
                }
                mVoxels.push_back(v);
            }
        }

        bool Bsoid::validVoxel(Voxel const& v)
        {
            return (
                v.isValid() &&
                v.id.x < mGridSize &&
                v.id.y < mGridSize &&
                v.id.z < mGridSize);
        }

    }
}
