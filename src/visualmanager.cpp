
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
#include <lms/framework.h>

VisualManager *VisualManager::_instance = NULL;
/**
 * TODO NOCHMAL SELBST NEU SCHREIBEN!
 * @brief VisualManager::VisualManager
 * @param dataManager
 * @param rootlogger
 */
VisualManager::VisualManager(lms::Module* creator,lms::Module::FakeDataManager* dataManager,
                             lms::logging::Logger &rootlogger,const std::string& pathToConfigs,
                             const lms::ModuleConfig *config):
        dataManager(dataManager),logger(rootlogger), config(config){
    logger.debug("init") << "Initialising VisualManager";
    _instance = this;
    this->creator = creator;

    //create LogManager
    ogreLogManager = new Ogre::LogManager();
    //TODO: link to our own logger.
    ogreLogManager->createLog("ogre.log", true, false, false);
    //Load config
    std::vector<std::string> privateConfPaths;
    privateConfPaths.push_back(pathToConfigs);

    //create ogre root
    root = new Ogre::Root(pathToConfigs+"/ogre_plugins.cfg",
                          pathToConfigs+"/ogre_default.cfg", "ogre.log");

    // A list of required plugins
    Ogre::StringVector required_plugins;
    required_plugins.push_back("GL RenderSystem");
    required_plugins.push_back("Octree Scene Manager");
    
    // Check if the required plugins are installed and ready for use
    Ogre::Root::PluginInstanceList ip = root->getInstalledPlugins();
    logger.debug("check required plugins");
    for (Ogre::StringVector::iterator required = required_plugins.begin(); required != required_plugins.end(); required++) {
        bool found = false;
        // try to find the required plugin in the current installed plugins
        for (Ogre::Root::PluginInstanceList::iterator available = ip.begin(); available != ip.end(); available++) {
            logger.debug("pluginName: ")<<(*available)->getName();
            if ((*available)->getName() == *required) {
                found = true;
                break;
            }
        }
        if (!found) {
            invalidate();
            logger.error("init") << "Required plugin not found: " << *required;
        }
    }

    // TODO setup resources
    //Ogre::ResourceGroupManager::getSingleton().addResourceLocation(cfg_manager->path()  + "../res/materials", "FileSystem", "General");

    // configure
    // Grab the OpenGL RenderSystem, or exit
    Ogre::RenderSystem* rs = root->getRenderSystemByName("OpenGL Rendering Subsystem");
    if(!(rs->getName() == "OpenGL Rendering Subsystem")) {
        invalidate();
        logger.error("init") << "OpenGL Rendering Subsystem not found";
    }else{
        // configure our RenderSystem
        rs->setConfigOption("Full Screen", "No");

        root->setRenderSystem(rs);

        root->initialise(false);

        setupMaterials();
    }
    logger.debug("init") << "Initialised VisualManager success:" << isValid();
}

VisualManager::~VisualManager(){
    //Windows will be deleted by the dataManager
    windowmap.clear();
}

visual::Window* VisualManager::getWindow(lms::Module* module,const std::string &title, bool create){
    //grant module write access
    logger.debug("getWindow()") << "called getwindow" << module->getName();
    lms::WriteDataChannel<visual::Window> window_ = dataManager->writeChannel<visual::Window>(module,title);
    //TODO it should work but it's not nice
    visual::Window *window = window_.get();
    //check if window has been already created
    auto it = windowmap.find(title);
    if (it == windowmap.end()) {
        //no need to create it
        if (create){
            //TODO use fullscreen
            bool fullscreen = config->get<bool>("fullscreen", false);
            int width = config->get<int>("width",600);
            int height = config->get<int>("height",600);
            //TODO moveable doesn't seem to work
            bool moveable = config->get<bool>("moveable",true);

            //create new window
            dataManager->readChannel<visual::Window>(creator,title);

            window->init(logger,this, width, height, title, moveable, fullscreen);
            it = windowmap.insert(std::pair<std::string, visual::Window*>(title, window)).first;
        }else{
            it->second = NULL;
        }
    }
    return it->second;
}

void VisualManager::render(){
    logger.debug("render");
    for (auto it =windowmap.begin(); it != windowmap.end(); ++it) {
        logger.debug("render window");
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

void VisualManager::invalidate(){
    valid = false;
}

bool VisualManager::isValid(){
    return valid;
}

