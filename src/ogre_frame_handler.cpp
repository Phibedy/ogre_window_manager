#include <ogre/ogre_frame_handler.h>
#include <ogre/visualmanager.h>
bool OgreFrameHandler::initialize(){
    visualmanager = new VisualManager(this->datamanager(),&logger);
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
