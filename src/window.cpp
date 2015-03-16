#include <ogre/window.h>
#include <ogre/visualmanager.h>
#include <lms/logging/logger.h>>

#include <sstream>

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


void Window::init(lms::logging::Logger& rootLogger, VisualManager* vm, int w, int h, const std::string &title, bool move){
    logger = new lms::logging::ChildLogger("window-"+title,&rootLogger);
    creator = vm;
    movement_enabled = move;
    mouse_state = STATE_NONE;
    yaw = 0;
    pitch = M_PI/2.;
    distance = 3;

    logger->debug("init") << "ceate new Window "<< title;
    
    Ogre::NameValuePairList params;

    params["FSAA"] =  "8";
    params["vsync"] = "false";

    try {
        ogreWindow = creator->getRoot()->createRenderWindow( title, w, h,
                                                             false, &params );
        ///TODO Config parameter to read "Fullscreen"
        //ogreWindow->setFullscreen(true, 1920, 1080);
    } catch( std::exception &ex ) {
        std::cerr << "rviz::RenderSystem: error creating render window: "
                  << ex.what() << std::endl;
        ogreWindow = NULL;
    }

    if( ogreWindow == NULL ) {
        printf( "Unable to create the rendering window after 100 tries." );
        assert(false);
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
    camera->setPosition(1, 0.3, 1);
    camera->setAutoAspectRatio(true);
    cameraNode = manager->getRootSceneNode()->createChildSceneNode("CameraNode/" + title);
    cameraNode->attachObject(camera);
    cameraNode->setPosition(Ogre::Vector3::ZERO);


    cameraFocusNode = cameraNode->createChildSceneNode();
    Ogre::Entity *ent = manager->createEntity(Ogre::SceneManager::PrefabType::PT_SPHERE);
    cameraFocusNode->attachObject(ent);
    cameraFocusNode->scale(0.001, 0.001, 0.0001);
    cameraFocusNode->setPosition(Ogre::Vector3::ZERO);
    cameraFocusNode->setVisible(false);

    Ogre::Light *light = manager->createLight();
    cameraNode->attachObject(light);
    light->setPosition(0,0,2);
    light->setDiffuseColour(Ogre::ColourValue(0.6, 0.6, 0.6));
    light->setDirection(0, 1, -1);
    light->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
    //    light->setDebugDisplayEnabled(1);

    //-------------------------------------------------------------------------------------
    // create viewports
    // Create one viewport, entire window
    //        float ratio = Ogre::Real(viewport->getActualWidth()) /(float)Ogre::Real(viewport->getActualHeight());
    ogreWindow->windowMovedOrResized();
    float ratio = ogreWindow->getWidth() /(float) ogreWindow->getHeight();
    viewport = ogreWindow->addViewport(camera,
                                       100, //Z-Order
                                       0, 0, //Left, Top
                                       1, 1); //Width, Height
    viewport->setBackgroundColour(Ogre::ColourValue(0.2,0.2,0.2));
    viewport->setAutoUpdated(true);
    // Alter the camera aspect ratio to match the viewport
    camera->setAspectRatio( ratio );

    //--------------------------------------------------------------------------
    // Setup OIS
    //--------------------------------------------------------------------------
    std::string windowHandleString;
    {
        size_t windowHandle = 0;
        ogreWindow->getCustomAttribute("WINDOW", &windowHandle);
        std::stringstream ss;
        ss << windowHandle;
        windowHandleString = ss.str();
    }

    OIS::ParamList oisParameters;
    oisParameters.insert(std::make_pair(std::string("WINDOW"),
                                        windowHandleString));
    oisParameters.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    oisParameters.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
    oisParameters.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
    oisParameters.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));


    inputManager = OIS::InputManager::createInputSystem(oisParameters);
    keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));

    mouse->setEventCallback(this);
    keyboard->setEventCallback(this);
    
    updateCamera();
}



Window::~Window()
{

}

void Window::resetCamera() {
    distance = 1;
    yaw = 0;
    pitch = 0;
    cameraNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TransformSpace::TS_LOCAL);
    cameraNode->setPosition(Ogre::Vector3::NEGATIVE_UNIT_Z);
    
    updateCamera();
}

void Window::update()
{
    const OIS::MouseState &ms = mouse->getMouseState();
    ms.width = ogreWindow->getWidth();
    ms.height = ogreWindow->getHeight();

    mouse->capture();
    keyboard->capture();
    ogreWindow->update(true);
}

/** Input Stuff */

bool Window::intersectGroundPlane( Ogre::Ray mouse_ray, Ogre::Vector3 *intersection_3d )
{
    //convert rays into reference frame //TODO What is the Target Reference Frame!
    mouse_ray.setOrigin( manager->getRootSceneNode()->convertWorldToLocalPosition( mouse_ray.getOrigin() ) );
    mouse_ray.setDirection( manager->getRootSceneNode()->convertWorldToLocalOrientation( Ogre::Quaternion::IDENTITY ) * mouse_ray.getDirection() );

    Ogre::Plane ground_plane( Ogre::Vector3::UNIT_Z, 0 );

    std::pair<bool, Ogre::Real> intersection = mouse_ray.intersects(ground_plane);
    if (!intersection.first) {
        return false;
    }

    *intersection_3d = mouse_ray.getPoint(intersection.second);
    return true;
}

void Window::updateCamera() {
    Ogre::Vector3 focal_point (Ogre::Vector3::ZERO);
    
    //printf("distance:%f yaw : %f , pitch: %f , focal_point.x:%f, focal_point.y:%f, focal_point.z:%f \n",distance, yaw, pitch, focal_point.x ,focal_point.y, focal_point.z);

    float x = distance * cos( yaw ) * cos( pitch ) + focal_point.x;
    float y = distance * sin( yaw ) * cos( pitch ) + focal_point.y;
    float z = distance *              sin( pitch ) + focal_point.z;

    Ogre::Vector3 pos( x, y, z );
    
    //printf("Camera: %f | %f | %f   Y %f P  %f \n", x, y, z, yaw, pitch);
    camera->setPosition(pos);
    camera->setFixedYawAxis(true, manager->getRootSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z);
    camera->setDirection(manager->getRootSceneNode()->getOrientation() * (focal_point - pos));
}

bool Window::mouseMoved( const OIS::MouseEvent &arg )
{
    if (!movement_enabled) return true;

    float dz = (arg.state.Z.rel < 0)?-1 : ((arg.state.Z.rel > 0) ? +1 : 0);

    if (dz != 0) {
        distance -= 1 * dz;

        distance = std::min(10.f, std::max(distance, 0.01f));
    }
    switch (mouse_state) {
    default:
    case STATE_NONE:
        break;
    case STATE_TRANSLATE: {
        int width = viewport->getActualWidth();
        int height = viewport->getActualHeight();

        Ogre::Ray mouse_ray = camera->getCameraToViewportRay(arg.state.X.abs / (float) width,
                                                             arg.state.Y.abs / (float) height);

        Ogre::Ray last_mouse_ray = camera->getCameraToViewportRay(old_state.X.abs / (float) width,
                                                                  old_state.Y.abs / (float) height);

        Ogre::Vector3 last_intersect, intersect;
        if (intersectGroundPlane(mouse_ray, &intersect) &&
                intersectGroundPlane(last_mouse_ray, &last_intersect)) {
            Ogre::Vector3 motion = last_intersect - intersect;

            float motion_distance_limit = 1; /*meter*/
            if( motion.length() > motion_distance_limit ) {
                motion.normalise();
                motion *= motion_distance_limit;
            }

            motion.z = 0;
            cameraNode->translate(motion);

            Ogre::Vector3 cpos = cameraNode->getPosition();
            float camera_limit_pos = 10;
            cpos.x = std::min(camera_limit_pos, std::max(cpos.x, -camera_limit_pos));
            cpos.y = std::min(camera_limit_pos, std::max(cpos.y, -camera_limit_pos));
        }
    }
        break;
    case STATE_ROTATE: {
        yaw += arg.state.X.rel * 0.005;
        pitch += arg.state.Y.rel * 0.005;

        if (pitch < -M_PI_2 + 0.001)
            pitch = -M_PI_2 + 0.001;
        if (pitch > M_PI_2  - 0.001)
            pitch = M_PI_2  - 0.001;
    }
        break;
    }


    ///Update Camera Position
    updateCamera();
    
    
    old_state = arg.state;

    return true;
}

bool Window::mousePressed( const OIS::MouseEvent &, OIS::MouseButtonID id )
{
    if (!movement_enabled) return true;
    cameraFocusNode->setVisible(true);
    if (mouse_state == STATE_NONE) {
        switch (id) {
        case OIS::MB_Left:
            mouse_state = STATE_TRANSLATE;
            break;
        case OIS::MB_Right:
            mouse_state = STATE_ROTATE;
            break;
        default:
            break;
        }
    } else {
        mouse_state = STATE_NONE;
    }
    return true;
}

bool Window::mouseReleased( const OIS::MouseEvent &, OIS::MouseButtonID  )
{
    if (!movement_enabled) return true;

    ///TODO Deactivate the right node
    mouse_state = STATE_NONE;
    cameraFocusNode->setVisible(false);
    return true;
}

void Window::addKeyFunction(Window::KeyCode key, std::function<void(void)> func)
{
    keymap.insert(std::make_pair(key, func));
}

bool Window::isKeyDown(Window::KeyCode key)
{
    return keyboard->isKeyDown(key);
}

bool Window::keyPressed(const OIS::KeyEvent &arg)
{
    auto res = keymap.equal_range(arg.key);
    for (auto it = res.first; it != res.second; ++it) {
        it->second();
    }
    return true;
}

bool Window::keyReleased(const OIS::KeyEvent &){
    ///Ignore
    return true;
}
} //End namespace
