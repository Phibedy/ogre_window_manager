#ifndef SHARED_VISUAL_VISUALMANAGER_H
#define SHARED_VISUAL_VISUALMANAGER_H

#include <ogre/window.h>
#include <string>

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneManager.h>

#include <pthread.h>
#include <core/framework.h>
#include <core/datamanager.h>

class VisualManager {
    friend class OgreFrameHandler;
private: 
    //TODO give logger
    VisualManager(lms::DataManager* dataManager);
    ~VisualManager();

    static VisualManager *_instance;
public: 
    //TODO shitty singelton
    static VisualManager* getInstance() { return _instance; }
    
    
    visual::window* getWindow(const std::string &window, bool create_if_not_exist=true);
        
    Ogre::Root *getRoot() { return root; }
    void render();
    
    void startRenderJob();
    void stopRenderJob();
    
private: 
    Ogre::Root* root; 
    Ogre::SceneManager *default_scenemanager;
    visual::window *aciveWindow;

    std::map<std::string, visual::window*> windowmap;
    
    
    visual::window* createWindow(const std::string& window,const int width = 300,const int height = 300,const bool moveable = true);
    void setupMaterials();

    //ConfigurationManager* cfg_manager;
    lms::DataManager* dataManager;
};

#endif
