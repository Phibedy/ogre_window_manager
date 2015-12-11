#include <ogre/ogre_frame_handler.h>
#include <ogre/visualmanager.h>
#include <lms/internal/framework.h>

bool OgreFrameHandler::initialize() {
    std::string pathToConfigs = lms::internal::Framework::configsDirectory;
    visualmanager = new VisualManager(this,datamanager(),logger,pathToConfigs, getConfig());
    return true;
}

bool OgreFrameHandler::deinitialize(){
    delete visualmanager;
    return true;
}

bool OgreFrameHandler::cycle(){
    visualmanager->render();
    return true;
}
