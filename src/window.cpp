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

    //why do we need that
    cameraNode = manager->getRootSceneNode()->createChildSceneNode("CameraNode/" + title);
    cameraNode->attachObject(camera);
    cameraNode->setPosition(Ogre::Vector3::ZERO);

    //why do we need that
    cameraFocusNode = cameraNode->createChildSceneNode();
    Ogre::Entity *ent = manager->createEntity(Ogre::SceneManager::PrefabType::PT_SPHERE);
    cameraFocusNode->attachObject(ent);
    cameraFocusNode->setScale(0.001, 0.001, 0.0001);
    cameraFocusNode->setPosition(Ogre::Vector3::ZERO);
    cameraFocusNode->setVisible(false);

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
    resetCamera();
}



Window::~Window(){
    //TODO
    delete ogreWindow;
}

void Window::resetCamera() {
    distance = 1;
    yaw = 0;
    pitch = 0;
    cameraNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TransformSpace::TS_LOCAL);
    cameraNode->setPosition(Ogre::Vector3::NEGATIVE_UNIT_Z);
}

void Window::update(){
    ogreWindow->update(true);
}

void Window::updateCamera() {

    Ogre::Vector3 focal_point(Ogre::Vector3::ZERO);

    float x = distance * cos( yaw ) * cos( pitch ) + focal_point.x;
    float y = distance * sin( yaw ) * cos( pitch ) + focal_point.y;
    float z = distance *              sin( pitch ) + focal_point.z;

    //TODO why has x set to 1?
    Ogre::Vector3 pos( x, y, z );
    
    printf("Camera: %f | %f | %f   Y %f P  %f \n", x, y, z, yaw, pitch);
    camera->setPosition(pos);
    camera->setFixedYawAxis(true, manager->getRootSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z);
    camera->setDirection(manager->getRootSceneNode()->getOrientation() * (focal_point - pos));

}

} //End namespace
