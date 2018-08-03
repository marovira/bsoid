#include "bsoid/Bsoid.hpp"

#include "bsoid/visualizer/ModelView.hpp"
#include "bsoid/visualizer/ModelVisualizer.hpp"
#include "bsoid/models/Models.hpp"

#include <atlas/core/Log.hpp>
#include <atlas/utils/Application.hpp>
#include <atlas/utils/WindowSettings.hpp>
#include <atlas/gl/ErrorCheck.hpp>

#include <atlas/tools/ModellingScene.hpp>

#include <fstream>

std::vector<bsoid::models::ModelFn> getModels()
{
    using namespace bsoid::models;

    std::vector<ModelFn> result;

    //result.push_back(makeSphere);
    //result.push_back(makeTorus);

    //result.push_back(makeBlend);
    //result.push_back(makeIntersection);
    //result.push_back(makeUnion);
    //result.push_back(makeTransform);

    result.push_back(makeButterfly);

    return result;
}

std::vector<bsoid::models::MCModelFn> getMCModels()
{
    using namespace bsoid::models;
    
    std::vector<MCModelFn> result;
    //result.push_back(makeMCSphere);
    //result.push_back(makeMCPeanut);
    //result.push_back(makeMCTorus);

    return result;
}


#if (BSOID_USE_GUI)
int main()
{
    // Atlas using.
    using atlas::utils::WindowSettings;
    using atlas::utils::ContextVersion;
    using atlas::utils::Application;
    using atlas::utils::ScenePointer;

    using bsoid::polygonizer::Bsoid;
    using bsoid::polygonizer::MarchingCubes;
    using bsoid::visualizer::ModelVisualizer;

    std::vector<Bsoid> models;
    std::vector<MarchingCubes> mcModels;

    auto modelFns = getModels();
    auto mcModelFns = getMCModels();

    for (auto& modelFn : modelFns)
    {
        models.emplace_back(modelFn());
    }
    
    for (auto& mcFn : mcModelFns)
    {
        mcModels.emplace_back(mcFn());
    }


    atlas::gl::setGLErrorSeverity(ATLAS_GL_ERROR_SEVERITY_HIGH |
        ATLAS_GL_ERROR_SEVERITY_MEDIUM);

    WindowSettings settings;
    settings.contextVersion = ContextVersion(4, 5);
    settings.isForwardCompat = TRUE;
    settings.isMaximized = true;
    settings.title = "Bsoid " + std::string(BSOID_VERSION_STRING);

    Application::getInstance().createWindow(settings);
    Application::getInstance().addScene(ScenePointer(
        new ModelVisualizer(models, mcModels)));
    Application::getInstance().runApplication();

    return 0;
}

#else

int main()
{
    INFO_LOG_V("Welcome to Bsoid %s", BSOID_VERSION_STRING);

    return 0;
}

#endif