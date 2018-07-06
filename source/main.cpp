#include "bsoid/Bsoid.hpp"

#include <atlas/core/Log.hpp>
#include <atlas/utils/Application.hpp>
#include <atlas/utils/WindowSettings.hpp>
#include <atlas/gl/ErrorCheck.hpp>

#include <atlas/tools/ModellingScene.hpp>

#include <fstream>


#if (BSOID_USE_GUI)
int main()
{
    // Atlas using.
    using atlas::utils::WindowSettings;
    using atlas::utils::ContextVersion;
    using atlas::utils::Application;
    using atlas::utils::ScenePointer;

    atlas::gl::setGLErrorSeverity(ATLAS_GL_ERROR_SEVERITY_HIGH |
        ATLAS_GL_ERROR_SEVERITY_MEDIUM);

    WindowSettings settings;
    settings.contextVersion = ContextVersion(4, 5);
    settings.isForwardCompat = TRUE;
    settings.isMaximized = true;
    settings.title = "Bsoid " + std::string(BSOID_VERSION_STRING);

    Application::getInstance().createWindow(settings);
    Application::getInstance().addScene(ScenePointer(
        new atlas::tools::ModellingScene));
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