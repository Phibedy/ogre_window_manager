
#include <ogre/visualmanager.h>
#include <lms/datamanager.h>

#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreWindowEventUtilities.h>
#include <OGRE/OgrePlugin.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

#include <pthread.h>
#include <X11/Xlib.h>

VisualManager *VisualManager::_instance = NULL;

VisualManager::VisualManager(lms::DataManager* dataManager):dataManager(dataManager){
    XInitThreads();
    _instance = this;

    // construct Ogre::Root : no plugins filename, no config filename, using a custom log filename
    
    Ogre::LogManager * lm = new Ogre::LogManager();
    lm->createLog("ogre.log", true, false, false); //TODO: redirect to our own logger.

    //TODO
    //root = new Ogre::Root(cfg_manager->path() + "plugins.cfg", cfg_manager->path() + "default.cfg", "ogre.log");

    // A list of required plugins
    Ogre::StringVector required_plugins;
    required_plugins.push_back("GL RenderSystem");
    required_plugins.push_back("Octree Scene Manager");

    // Check if the required plugins are installed and ready for use
    // If not: exit the application
    Ogre::Root::PluginInstanceList ip = root->getInstalledPlugins();
    for (Ogre::StringVector::iterator j = required_plugins.begin(); j != required_plugins.end(); j++) {
        bool found = false;
        // try to find the required plugin in the current installed plugins
        for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++) {
            if ((*k)->getName() == *j) {
                found = true;
                break;
            }
        }
        if (!found) { // return false because a required plugin is not available
            //TODO errorhandling
            //printf("ERROR! Loading plugin\n");
            return;
        }
    }

    // setup resources
    //Ogre::ResourceGroupManager::getSingleton().addResourceLocation(cfg_manager->path()  + "../res/materials", "FileSystem", "General");

    // configure
    // Grab the OpenGL RenderSystem, or exit
    Ogre::RenderSystem* rs = root->getRenderSystemByName("OpenGL Rendering Subsystem");
    if(!(rs->getName() == "OpenGL Rendering Subsystem")) {
        //TODO errorhandling
        return; //No RenderSystem found
    }

    // configure our RenderSystem
    rs->setConfigOption("Full Screen", "No");

    root->setRenderSystem(rs);

    root->initialise(false);

    setupMaterials();
}

VisualManager::~VisualManager()
{
    for(auto it = windowmap.begin(); it != windowmap.end(); ++it) {
        delete it->second;
    }
    windowmap.clear();
}

visual::window* VisualManager::getWindow(const std::string &window, bool create)
{
    auto it = windowmap.find(window);

    if (it == windowmap.end()) {
        if (create)
            it = windowmap.insert(std::pair<std::string, visual::window*>(window, createWindow(window))).first;
        else
            it->second = NULL;
    }

    return it->second;
}

visual::window* VisualManager::createWindow(const std::string& window,const int width,const int height,const bool moveable){
    printf("Created window %s\n", window.c_str());
    return new visual::window(this, width, height, window, moveable);
}

void VisualManager::render()
{
    for (auto it =windowmap.begin(); it != windowmap.end(); ++it) {
        it->second->update();
    }
    Ogre::WindowEventUtilities::messagePump();
}

void VisualManager::setupMaterials(){
    //TODO Doesn't work at all :(
    {
        Ogre::MaterialPtr dflt = Ogre::MaterialManager::getSingleton().create("DefaultMaterial",
                                 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        dflt->createTechnique();
        dflt->getTechnique(0)->createPass();
        dflt->getTechnique(0)->getPass(0)->setLightingEnabled(true);
        dflt->getTechnique(0)->getPass(0)->setDiffuse(0.5, 0.5, 0.5, 1);
        Ogre::TextureUnitState *tus = dflt->getTechnique(0)->getPass(0)->createTextureUnitState();
        tus->setColourOperationEx(Ogre::LayerBlendOperationEx::LBX_MODULATE_X2, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(1, 0.1, 0.1));
    }
    {
        Ogre::MaterialPtr dflt = Ogre::MaterialManager::getSingleton().create("GroundMaterial",
                                 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        dflt->createTechnique();
        dflt->getTechnique(0)->createPass();
        dflt->getTechnique(0)->getPass(0)->setLightingEnabled(true);
        dflt->getTechnique(0)->getPass(0)->setDiffuse(0.5, 0.5, 0.5, 1);
        Ogre::TextureUnitState *tus = dflt->getTechnique(0)->getPass(0)->createTextureUnitState();
        tus->setColourOperationEx(Ogre::LayerBlendOperationEx::LBX_MODULATE_X2, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0.6, 0.6, 0.7));
    }
}

