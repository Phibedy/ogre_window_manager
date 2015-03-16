#ifndef SHARED_VISUAL_VISUALMANAGER_H
#define SHARED_VISUAL_VISUALMANAGER_H

#include <ogre/window.h>
#include <lms/logger.h>
#include <string>

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneManager.h>

#include <pthread.h>
#include <lms/framework.h>
#include <lms/datamanager.h>

class VisualManager {
    friend class OgreFrameHandler;
private: 
    //TODO give logger
    VisualManager(lms::Module* creator,lms::DataManager *dataManager, lms::logging::Logger *rootlogger,const std::string& pathToConfig);
    ~VisualManager();

    static VisualManager *_instance;
public: 
    //TODO shitty singelton
    static VisualManager* getInstance() { return _instance; }
    
    /**
     * @brief getWindow returns a Window registered as dataChannel, so you can write on it using several modules
     * @param module
     * @param title will be  title of the frame and the name of the datachannel!
     * @param create_if_not_exist
     * @return
     */
    visual::Window* getWindow(lms::Module* module,const std::string &title, bool create_if_not_exist=true);

    void render();
    
    void startRenderJob();
    void stopRenderJob();
    bool isValid();
    void invalidate();
    Ogre::Root *getRoot(){ return root; }
    
private:
    Ogre::Root* root;
    Ogre::SceneManager *default_scenemanager;
    visual::Window *aciveWindow;
    Ogre::LogManager* ogreLogManager;

    std::map<std::string, visual::Window*> windowmap;
    
    void setupMaterials();

    //ConfigurationManager* cfg_manager;
    lms::DataManager* dataManager;
    lms::logging::ChildLogger logger;

    bool valid;
    /**
     * @brief creator module that will call render
     */
    lms::Module* creator;
};

#endif
