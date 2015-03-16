#include <ogre/ogre_frame_handler.h>
#include <ogre/visualmanager.h>
#include <lms/framework.h>
bool OgreFrameHandler::initialize(){
    std::string pathToConfigs = lms::Framework::programDirectory()+ "external/modules/ogre_window_manager/configs/";
    visualmanager = new VisualManager(this,this->datamanager(),&logger,pathToConfigs);
    return true;
}

bool OgreFrameHandler::deinitialize(){
    delete visualmanager;
    return true;
}

bool OgreFrameHandler::cycle(){
    //TODO render stuff
    return true;
}
