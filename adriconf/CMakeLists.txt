# Check if we are running from inside main config
if (NOT TOP_CMAKE_WAS_CALLED)
    message(FATAL_ERROR "
    You did not 'cmake' the top CMakeLists.txt file. Use the one in the top dir.
    It is advice to delete all wrongly generated cmake stuff => CMakeFiles & CMakeCache.txt")
endif ()

# Define the executable itself
add_executable(adriconf main.cpp ${SHARED_SOURCE_FILES} ${CMAKE_SOURCE_DIR}/adriconf/resources.c)

target_link_libraries(adriconf ${GTKMM_LIBRARIES})
target_include_directories(adriconf PUBLIC ${GTKMM_INCLUDE_DIRS})

target_link_libraries(adriconf ${Boost_LOCALE_LIBRARIES})
target_link_libraries(adriconf ${Boost_FILESYSTEM_LIBRARIES})
target_link_libraries(adriconf ${Boost_SYSTEM_LIBRARIES})
target_include_directories(adriconf PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(adriconf ${LibXML++_LIBRARIES})
target_include_directories(adriconf PUBLIC ${LibXML++_INCLUDE_DIRS})

target_link_libraries(adriconf ${X11_LIBRARIES})
target_include_directories(adriconf PUBLIC ${X11_INCLUDE_DIRS})

target_link_libraries(adriconf ${DRM_LIBRARIES})
target_include_directories(adriconf PUBLIC ${DRM_INCLUDE_DIRS})

target_link_libraries(adriconf ${PCILIB_LIBRARIES})
target_include_directories(adriconf PUBLIC ${PCILIB_INCLUDE_DIRS})

target_link_libraries(adriconf ${OPENGL_gl_LIBRARY})
if (OpenGL_GLX_FOUND)
    target_link_libraries(adriconf ${OPENGL_glx_LIBRARY})
endif ()

if (ENABLE_XWAYLAND)
    target_link_libraries(adriconf ${EGL_LIBRARIES})
    target_include_directories(adriconf PUBLIC ${EGL_INCLUDE_DIRS})
    target_compile_definitions(adriconf PRIVATE ENABLE_XWAYLAND)
endif ()

target_compile_options(adriconf PRIVATE -Wall -Wextra -pedantic)

add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/adriconf/resources.c
        COMMAND glib-compile-resources ${CMAKE_SOURCE_DIR}/adriconf/adriconf.gresource.xml --target=resources.c --generate-source
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/adriconf
        DEPENDS ${CMAKE_SOURCE_DIR}/adriconf/DriConf.glade
        COMMENT "Generate resources"
        )

set_source_files_properties(${CMAKE_SOURCE_DIR}/adriconf/resources.c PROPERTIES
        GENERATED TRUE
        )

install(TARGETS adriconf DESTINATION bin)