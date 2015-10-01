#ifndef OGRE_FRAME_HANDLER
#define OGRE_FRAME_HANDLER
#include <lms/module.h>
#include <lms/logging/logger.h>
#include <ogre/visualmanager.h>

class OgreFrameHandler: public lms::Module{
public:
    bool initialize();
    bool deinitialize();

    bool cycle();
private:
    VisualManager* visualmanager;
};
#endif /*OGRE_FRAME_HANDLER */
