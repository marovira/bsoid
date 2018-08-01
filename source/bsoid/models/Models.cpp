#include "bsoid/models/Models.hpp"

#include "bsoid/fields/Sphere.hpp"
#include "bsoid/fields/Torus.hpp"

#include "bsoid/operators/Blend.hpp"

#include "bsoid/tree/BlobTree.hpp"
#include "bsoid/polygonizer/Bsoid.hpp"

#include <tuple>

namespace bsoid
{
    namespace models
    {
        // Global usings.
        using fields::ImplicitFieldPtr;
        using operators::ImplicitOperatorPtr;
        using tree::BlobTree;
        using polygonizer::Bsoid;
        using polygonizer::MarchingCubes;

        using Resolution = std::tuple<std::size_t, std::size_t>;

        constexpr Resolution lowResolution = { 32, 8 };
        constexpr Resolution midResolution = { 512, 128 };
        constexpr Resolution highResolution = { 2048, 512 };

        constexpr Resolution currentResolution = lowResolution;

        polygonizer::Bsoid makeSphere()
        {
            using fields::Sphere;

            ImplicitFieldPtr sphere = std::make_shared<Sphere>();
            BlobTree tree;
            tree.insertField(sphere);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(sphere);

            Bsoid soid(tree, "sphere");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCSphere()
        {
            using fields::Sphere;

            ImplicitFieldPtr sphere = std::make_shared<Sphere>();
            BlobTree tree;
            tree.insertField(sphere);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(sphere);

            MarchingCubes mc(tree, "sphere");
            mc.setResolution(std::get<0>(currentResolution));
            return mc;
        }

        polygonizer::Bsoid makePeanut()
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Blend;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(1.0f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-1.0f, 0, 0));
            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, blend });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(blend);

            Bsoid soid(tree, "peanut");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCPeanut()
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Blend;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(1.0f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-1.0f, 0, 0));
            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, blend });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(blend);

            MarchingCubes mc(tree, "peanut");
            mc.setResolution(std::get<0>(currentResolution));

            return mc;
        }

        polygonizer::Bsoid makeTorus()
        {
            using atlas::math::Point;
            using fields::Torus;

            ImplicitFieldPtr torus = std::make_shared<Torus>();
            BlobTree tree;
            tree.insertField(torus);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(torus);

            Bsoid soid(tree, "torus");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCTorus()
        {
            using atlas::math::Point;
            using fields::Torus;

            ImplicitFieldPtr torus = std::make_shared<Torus>();
            BlobTree tree;
            tree.insertField(torus);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(torus);

            MarchingCubes mc(tree, "torus");
            mc.setResolution(std::get<0>(currentResolution));
            return mc;
        }
    }
}
