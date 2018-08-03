#include "bsoid/models/Models.hpp"

#include "bsoid/fields/Sphere.hpp"
#include "bsoid/fields/Torus.hpp"

#include "bsoid/operators/Blend.hpp"
#include "bsoid/operators/Intersection.hpp"
#include "bsoid/operators/Union.hpp"
#include "bsoid/operators/Transform.hpp"

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

        polygonizer::Bsoid makeBlend()
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

            Bsoid soid(tree, "blend");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCBlend()
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

            MarchingCubes mc(tree, "blend");
            mc.setResolution(std::get<0>(currentResolution));

            return mc;
        }

        polygonizer::Bsoid makeIntersection()
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Intersection;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr intersection = std::make_shared<Intersection>();
            intersection->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, intersection });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(intersection);

            Bsoid soid(tree, "intersection");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCIntersection()
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Intersection;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr intersection = std::make_shared<Intersection>();
            intersection->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, intersection });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(intersection);

            MarchingCubes mc(tree, "intersection");
            mc.setResolution(std::get<0>(currentResolution));

            return mc;
        }

        polygonizer::Bsoid makeUnion()
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Union;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr op = std::make_shared<Union>();
            op->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, op });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(op);

            Bsoid soid(tree, "union");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCUnion()
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Union;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr op = std::make_shared<Union>();
            op->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, op });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(op);

            MarchingCubes mc(tree, "union");
            mc.setResolution(std::get<0>(currentResolution));
            return mc;
        }

        polygonizer::Bsoid makeTransform()
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;
            using fields::Torus;
            using operators::Transform;

            ImplicitFieldPtr torus =
                std::make_shared<Torus>();
            ImplicitOperatorPtr op = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(0.0f, 0.0f, 3.0f)) * 
                glm::rotate(Matrix4(1.0f), glm::radians(90.0f), Vector(1, 0, 0)) * 
                glm::scale(Matrix4(1.0f), Vector(0.5f)) * 
                glm::scale(Matrix4(1.0f), Vector(2.5f, 1.0f, 1.0f)));
            op->insertField(torus);

            BlobTree tree;
            tree.insertFields({ op });
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(op);

            Bsoid soid(tree, "transform");
            soid.setResolution(std::get<0>(currentResolution),
                std::get<1>(currentResolution));
            return soid;
        }

        polygonizer::MarchingCubes makeMCTransform()
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;
            using fields::Torus;
            using operators::Transform;

            ImplicitFieldPtr torus =
                std::make_shared<Torus>();
            ImplicitOperatorPtr op = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(0.0f, 0.0f, 3.0f)) * 
                glm::rotate(Matrix4(1.0f), glm::radians(45.0f), Vector(1, 0, 0)) * 
                glm::scale(Matrix4(1.0f), Vector(0.5f)) * 
                glm::scale(Matrix4(1.0f), Vector(2.5f, 1.0f, 1.0f)));
            op->insertField(torus);

            BlobTree tree;
            tree.insertFields({ op });
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(op);

            MarchingCubes mc(tree, "transform");
            mc.setResolution(std::get<0>(currentResolution));
            return mc;

        }


    }
}
