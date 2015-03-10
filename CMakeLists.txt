set ( SOURCES 
        "src/ogre_frame_handler.cpp"
	"src/interface.cpp"
	"src/visualmanager.cpp"
	"src/window.cpp"
)

set (HEADERS 
	"include/ogre/ogre_frame_handler.h"
	"include/ogre/visualmanager.h"
	"include/ogre/window.h"
)

include_directories("include")

add_library (ogre_window_manager SHARED ${SOURCES} ${HEADERS})
#link ogre and lms
target_link_libraries(ogre_window_manager PRIVATE lms ogre_binding)
