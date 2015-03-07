#include <ogre/ogre_frame_handler.h>

extern "C" {
void* getInstance () {
    return new OgreFrameHandler();
}
}
