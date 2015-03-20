#include <ogre/window.h>
#include <ogre/visualmanager.h>
#include <lms/logging/logger.h>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreEntity.h>

namespace visual{
Window::Window(){
}

void Window::init(lms::logging::Logger& rootLogger, VisualManager* vm, int w, int h, const std::string &title, bool move, bool fullscreen){
    logger = new lms::logging::ChildLogger("window-"+title,&rootLogger);
    creator = vm;
    movement_enabled = move;

    logger->debug("init") << "ceate new Window "<< title;
    
    Ogre::NameValuePairList params;
    //TODO load from config
    params["FSAA"] =  "8";
    params["vsync"] = "false";

    try {
        ogreWindow = creator->getRoot()->createRenderWindow( title, w, h,
                                                             false, &params );
        if(fullscreen){
            //TODO get resolution
            //ogreWindow->setFullscreen(true, 1920, 1080);
        }
    } catch( std::exception &ex ) {
        logger->error("create window")<< "rviz::RenderSystem: error creating render window: "
                  << ex.what() << std::endl;
        ogreWindow = nullptr;
        //TODO errorhandling
        return;
    }

    ogreWindow->setActive(true);
    ogreWindow->setAutoUpdated(false);

    manager = creator->getRoot()->createSceneManager(Ogre::ST_GENERIC, "SceneManager/" + title);
    manager->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
    //-------------------------------------------------------------------------------------
    // Create the camera
    camera = manager->createCamera("Camera/" + title);
    camera->setNearClipDistance(0.1);
    camera->setFarClipDistance(200);
    camera->setAutoAspectRatio(true);

    //-------------------------------------------------------------------------------------
    // create viewports
    // Create one viewport, entire window
    ogreWindow->windowMovedOrResized();
    viewport = ogreWindow->addViewport(camera,
                                       100, //Z-Order
                                       0, 0, //Left, Top
                                       1, 1); //Width, Height
    viewport->setBackgroundColour(Ogre::ColourValue(0.2,0.2,0.2));
    viewport->setAutoUpdated(true);

    //set to default values
    //resetCamera();
}
Window::~Window(){
    //TODO
    delete ogreWindow;
}

void Window::update(){
    ogreWindow->update(true);
}
} //End namespace
