PROJECT( wiegand )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.0 )

SET( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}" )

INCLUDE( VersionMacros )

SET( DEFAULT_HEADER_INSTALL_DIR include/\${target} )
SET( DEFAULT_LIBRARY_INSTALL_DIR lib/ )
SET( DEFAULT_EXECUTABLE_INSTALL_DIR bin/ )
SET( CMAKE_DEBUG_POSTFIX _debug )
SET( BUILD_SHARED_LIBS NO )

SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE( Boost COMPONENTS thread date_time system filesystem program_options signals serialization chrono) 

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )

INCLUDE( SetupTargetMacros )

#ADD_DEFINITIONS( -DBOOST_CONTEXT_UCTX )
#ADD_DEFINITIONS( -D_XOPEN_SOURCE )


set( sources main.cpp )
set( libraries 
    ${Boost_PROGRAM_OPTIONS_LIBRARY}  
    ${Boost_SYSTEM_LIBRARY}  
    )


add_executable( wiegand ${sources} )
target_link_libraries( wiegand ${libraries} )
