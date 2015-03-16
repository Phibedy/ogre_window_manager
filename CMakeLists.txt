include_directories("include")
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

add_library (ogre_window_manager SHARED ${SOURCES} ${HEADERS})
#link ogre and lms
target_link_libraries(ogre_window_manager PRIVATE lms ogre_binding)


#copy ogre configs
set(ConfigFiles
    "configs/ogre_plugins.cfg"
    "configs/ogre_default.cfg"
)
foreach(ConfigFile ${ConfigFiles})
    add_custom_command(TARGET ogre_window_manager PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                            copy "${CMAKE_CURRENT_LIST_DIR}/${ConfigFile}" "$<TARGET_FILE_DIR:ogre_window_manager>/${ConfigFile}"
    )
endforeach()
