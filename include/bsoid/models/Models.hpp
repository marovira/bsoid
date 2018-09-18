#ifndef ATHENA_INCLUDE_ATHENA_MODELS_MODELS_HPP
#define ATHENA_INCLUDE_ATHENA_MODELS_MODELS_HPP

#pragma once

#include "bsoid/polygonizer/Bsoid.hpp"
#include "bsoid/polygonizer/MarchingCubes.hpp"

#include <functional>

#define MAKE_FUNCTION(name) \
bsoid::polygonizer::Bsoid make##name(); \
bsoid::polygonizer::MarchingCubes makeMC##name()

namespace bsoid
{
    namespace models
    {
        using ModelFn = std::function<bsoid::polygonizer::Bsoid()>;
        using MCModelFn = std::function<bsoid::polygonizer::MarchingCubes()>;

        MAKE_FUNCTION(Sphere);
        MAKE_FUNCTION(Torus);

        MAKE_FUNCTION(Blend);
        MAKE_FUNCTION(Intersection);
        MAKE_FUNCTION(Union);
        MAKE_FUNCTION(Transform);

        MAKE_FUNCTION(Butterfly);

        MAKE_FUNCTION(Particles);

    }
}

#endif
