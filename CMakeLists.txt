cmake_minimum_required(VERSION 3.8)
project(adriconf)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Device.cpp Device.h
        DriverOption.cpp DriverOption.h
        Section.cpp Section.h
        Parser.cpp Parser.h
        Application.cpp Application.h
        ConfigurationResolver.cpp ConfigurationResolver.h
        DRIQuery.cpp DRIQuery.h
        DriverConfiguration.cpp DriverConfiguration.h
        Writer.cpp Writer.h GUI.cpp GUI.h ConfigurationLoader.cpp ConfigurationLoader.h ApplicationOption.cpp ApplicationOption.h
        resources.c)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

# GTKMM
pkg_check_modules(GTKMM gtkmm-3.0)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

# BOOST
find_package(Boost 1.60 COMPONENTS locale)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# LIBXML
pkg_check_modules(LibXML++ REQUIRED libxml++-3.0)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})

# X11
pkg_check_modules(X11 REQUIRED x11)
include_directories(${X11_INCLUDE_DIRS})
link_directories(${X11_LIBRARY_DIRS})

# INTL
find_package (Intl REQUIRED)
find_package(Gettext REQUIRED)
include_directories(${INTL_INCLUDE_DIRS})
link_directories(${INTL_LIBRARY_DIRS})

#INTL INSTALL TRANSLATIONS
FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

IF(NOT GETTEXT_MSGFMT_EXECUTABLE)
    MESSAGE("------
    NOTE: msgfmt not found. Translations will *not* be installed
------")
ELSE(NOT GETTEXT_MSGFMT_EXECUTABLE)

    SET(catalogname rkward)

    FILE(GLOB PO_FILES *.po)
    SET(GMO_FILES)

    FOREACH(_poFile ${PO_FILES})
        GET_FILENAME_COMPONENT(_poFileName ${_poFile} NAME)
        STRING(REGEX REPLACE "^${catalogname}_?" "" _langCode ${_poFileName} )
        STRING(REGEX REPLACE "\\.po$" "" _langCode ${_langCode} )

        IF( _langCode )
            GET_FILENAME_COMPONENT(_lang ${_poFile} NAME_WE)
            SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

            ADD_CUSTOM_COMMAND(OUTPUT ${_gmoFile}
                    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
                    DEPENDS ${_poFile})
            INSTALL(FILES ${_gmoFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_langCode}/LC_MESSAGES/ RENAME ${catalogname}.mo)
            LIST(APPEND GMO_FILES ${_gmoFile})
        ENDIF( _langCode )

    ENDFOREACH(_poFile ${PO_FILES})

    ADD_CUSTOM_TARGET(translations ALL DEPENDS ${GMO_FILES})

ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)


# Define the executable itself
add_executable(adriconf ${SOURCE_FILES})

target_link_libraries(adriconf ${GTKMM_LIBRARIES})
target_link_libraries(adriconf ${Boost_LOCALE_LIBRARIES})
target_link_libraries(adriconf ${LibXML++_LIBRARIES})
target_link_libraries(adriconf ${X11_LIBRARIES})
target_link_libraries(adriconf ${OPENGL_gl_LIBRARY})
target_link_libraries(adriconf ${Gtest_LIBRARIES})

add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/resources.c
    COMMAND glib-compile-resources adriconf.gresource.xml --target=resources.c --generate-source
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_source_files_properties(${CMAKE_SOURCE_DIR}/resources.c PROPERTIES
    GENERATED TRUE
)

# Move the glade file
add_custom_command(
        TARGET adriconf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/DriConf.glade
        ${CMAKE_CURRENT_BINARY_DIR}/)
