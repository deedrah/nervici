cmake_minimum_required (VERSION 2.6.0)
project (Nervici CXX C)

set (CMAKE_COLOR_MAKEFILE ON)
set (CMAKE_VERBOSE_MAKEFILE OFF)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

find_package (SDL REQUIRED)
find_package (SDL_image REQUIRED)
find_package (OpenAL REQUIRED)
find_package (PkgConfig REQUIRED)
find_package (SDL_gfx REQUIRED)
find_package (Gettext REQUIRED)

pkg_check_modules (GLIBMM REQUIRED glibmm-2.4)
pkg_check_modules (GTHREAD REQUIRED gthread-2.0)
pkg_check_modules (GIOMM REQUIRED giomm-2.4)
pkg_check_modules (SDLPango REQUIRED SDL_Pango)

#set (CMAKE_C_FLAGS "-O2 -std=c99")
set (CMAKE_CXX_FLAGS "-O2 -std=c++0x")
#set (CMAKE_C_FLAGS_DEBUG "-Wall -O0 -pedantic -g3 -gdwarf-2")
set (CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -pedantic -g3 -gdwarf-2")
set (CMAKE_CXX_FLAGS_PROFILE "-pg")

set (DATA_DIR "share/nervici")
file (TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${DATA_DIR} NERVICI_DIR)

configure_file (src/nervici.sh.in nervici.sh)

include_directories ("include")

include_directories (SYSTEM 
    ${SDL_INCLUDE_DIR} 
    ${SDLIMAGE_INCLUDE_DIR} 
    ${SDLGFX_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR} 
    ${GLIBMM_INCLUDE_DIRS} 
    ${GTHREAD_INCLUDE_DIRS} 
    ${GIOMM_INCLUDE_DIRS} 
    ${SDLPango_INCLUDE_DIRS} 
)

include (nervici)
include (images)
include (sounds)
include (music)

install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/nervici.sh
    DESTINATION bin
    RENAME nervici)

add_subdirectory (po)

