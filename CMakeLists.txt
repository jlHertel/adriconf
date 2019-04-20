cmake_minimum_required(VERSION 3.8)
project(adriconf)

set(CMAKE_CXX_STANDARD 14)

option(ENABLE_XWAYLAND "Enable XWayland support" ON)
set(TOP_CMAKE_WAS_CALLED TRUE)

set(SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/Device.cpp
        ${CMAKE_SOURCE_DIR}/Device.h
        ${CMAKE_SOURCE_DIR}/DriverOption.cpp
        ${CMAKE_SOURCE_DIR}/DriverOption.h
        ${CMAKE_SOURCE_DIR}/Section.cpp
        ${CMAKE_SOURCE_DIR}/Section.h
        ${CMAKE_SOURCE_DIR}/Parser.cpp
        ${CMAKE_SOURCE_DIR}/Parser.h
        ${CMAKE_SOURCE_DIR}/Application.cpp
        ${CMAKE_SOURCE_DIR}/Application.h
        ${CMAKE_SOURCE_DIR}/ConfigurationResolver.cpp
        ${CMAKE_SOURCE_DIR}/ConfigurationResolver.h
        ${CMAKE_SOURCE_DIR}/DRIQuery.cpp
        ${CMAKE_SOURCE_DIR}/DRIQuery.h
        ${CMAKE_SOURCE_DIR}/HelpersWayland.cpp
        ${CMAKE_SOURCE_DIR}/HelpersWayland.h
        ${CMAKE_SOURCE_DIR}/DriverConfiguration.cpp
        ${CMAKE_SOURCE_DIR}/DriverConfiguration.h
        ${CMAKE_SOURCE_DIR}/Writer.cpp
        ${CMAKE_SOURCE_DIR}/Writer.h
        ${CMAKE_SOURCE_DIR}/GUI.cpp
        ${CMAKE_SOURCE_DIR}/GUI.h
        ${CMAKE_SOURCE_DIR}/ConfigurationLoader.cpp
        ${CMAKE_SOURCE_DIR}/ConfigurationLoader.h
        ${CMAKE_SOURCE_DIR}/ApplicationOption.cpp
        ${CMAKE_SOURCE_DIR}/ApplicationOption.h
        ${CMAKE_SOURCE_DIR}/resources.c
        ${CMAKE_SOURCE_DIR}/GPUInfo.cpp
        ${CMAKE_SOURCE_DIR}/GPUInfo.h
        ${CMAKE_SOURCE_DIR}/PCIDatabaseQuery.cpp
        ${CMAKE_SOURCE_DIR}/PCIDatabaseQuery.h
        ${CMAKE_SOURCE_DIR}/ComboBoxColumn.h
        ${CMAKE_SOURCE_DIR}/DriverOptionType.h
        )

find_package(PkgConfig REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# GTKMM
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# BOOST
find_package(Boost 1.60 REQUIRED COMPONENTS locale)

# LIBXML
pkg_check_modules(LibXML++ libxml++-3.0)
if (NOT LibXML++_FOUND)
    pkg_check_modules(LibXML++2 REQUIRED libxml++-2.6)
    set(LibXML++_INCLUDE_DIRS ${LibXML++2_INCLUDE_DIRS})
    set(LibXML++_LIBRARIES ${LibXML++2_LIBRARIES})
endif ()

# X11
pkg_check_modules(X11 REQUIRED x11)

# LIBDRM
pkg_check_modules(DRM REQUIRED libdrm)

# LIBPCI
pkg_check_modules(PCILIB REQUIRED libpci)

# INTL
find_package(Intl REQUIRED)
find_package(Gettext REQUIRED)
include_directories(${INTL_INCLUDE_DIRS})
link_directories(${INTL_LIBRARY_DIRS})

#EGL
#Use these libs if running on Wayland
if (ENABLE_XWAYLAND)
    pkg_check_modules(EGL REQUIRED egl)
endif ()

#INTL INSTALL TRANSLATIONS
GETTEXT_CREATE_TRANSLATIONS(po/adriconf.pot ALL po/en.po po/hr.po po/pt_BR.po)

# Define the executable itself
add_executable(adriconf main.cpp ${SOURCE_FILES})

target_link_libraries(adriconf ${GTKMM_LIBRARIES})
target_include_directories(adriconf PUBLIC ${GTKMM_INCLUDE_DIRS})

target_link_libraries(adriconf ${Boost_LOCALE_LIBRARIES})
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

add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/resources.c
        COMMAND glib-compile-resources adriconf.gresource.xml --target=resources.c --generate-source
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${CMAKE_SOURCE_DIR}/DriConf.glade
        )
set_source_files_properties(${CMAKE_SOURCE_DIR}/resources.c PROPERTIES
        GENERATED TRUE
        )

add_subdirectory(tests)

install(TARGETS adriconf DESTINATION bin)
