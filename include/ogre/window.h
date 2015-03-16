#ifndef SHARED_VISUAL_WINDOW_H
#define SHARED_VISUAL_WINDOW_H

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreRay.h>

#include <OIS/OIS.h>

#include <map>
#include <functional>
#include <lms/logger.h>

class VisualManager;

namespace visual
{

class Window : private OIS::MouseListener, private OIS::KeyListener
{
public:

    typedef OIS::KeyCode KeyCode;

    Window();
    /**
     * @brief Window::init only call this method if you know what you are doing :)
     * @param vm
     * @param width
     * @param height
     * @param title
     * @param movement_enabbled
     */
    void init(lms::logging::Logger& rootLogger, VisualManager* vm,int width,int height, const std::string &title,bool movement_enabbled);
    ~Window();

    Ogre::SceneManager *getSceneManager() {
        return manager;
    }

    Ogre::Camera *getCamera() {
        return camera;
    }

    Ogre::RenderWindow *getGfxWindow() {
        return ogreWindow;
    }

    void update();

    void enableMovement(bool enabled) {
        movement_enabled = enabled;
    }

    void addKeyFunction(KeyCode key, std::function<void(void)> func);
    bool isKeyDown(KeyCode key);
    
    void resetCamera();
    
private:
    Ogre::SceneManager *manager;
    Ogre::Camera *camera;
    Ogre::Viewport *viewport;
    Ogre::RenderWindow *ogreWindow;

    VisualManager *creator;

    Ogre::SceneNode *cameraNode;
    Ogre::SceneNode *cameraFocusNode;

    OIS::InputManager *inputManager;
    OIS::Mouse *mouse;
    OIS::Keyboard *keyboard;

    bool movement_enabled;
    lms::logging::Logger* logger;


private:
    bool intersectGroundPlane(Ogre::Ray mouse_ray, Ogre::Vector3 *intersection);
    void updateCamera();
protected:
    OIS::MouseState old_state;
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    enum mouse_state_t {
        STATE_TRANSLATE, ///RMB
        STATE_ROTATE,    ///LMB
        STATE_NONE       ///No MB
    } mouse_state;

    float yaw, pitch;
    float distance = 1;
protected:

    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);

    std::multimap<KeyCode, std::function<void(void)> > keymap;
};

}

#endif
