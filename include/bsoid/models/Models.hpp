#ifndef ATHENA_INCLUDE_ATHENA_MODELS_MODELS_HPP
#define ATHENA_INCLUDE_ATHENA_MODELS_MODELS_HPP

#pragma once

#include "bsoid/polygonizer/Bsoid.hpp"
#include "bsoid/polygonizer/MarchingCubes.hpp"

#include <functional>

#define MAKE_SOID_FUNCTION(name) \
bsoid::polygonizer::Bsoid make##name()

#define MAKE_MC_FUNCTION(name) \
bsoid::polygonizer::MarchingCubes makeMC##name()


namespace bsoid
{
    namespace models
    {
        using ModelFn = std::function<bsoid::polygonizer::Bsoid()>;
        using MCModelFn = std::function<bsoid::polygonizer::MarchingCubes()>;

        MAKE_SOID_FUNCTION(Sphere);
        MAKE_SOID_FUNCTION(Peanut);
        MAKE_SOID_FUNCTION(Torus);

        MAKE_MC_FUNCTION(Sphere);
        MAKE_MC_FUNCTION(Peanut);
        MAKE_MC_FUNCTION(Torus);
    }
}

#endif
