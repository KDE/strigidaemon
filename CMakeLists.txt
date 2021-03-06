project (strigidaemon)

##### cmake settings #####

cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MacroFindOptionalDep)
enable_testing()

# Used to create config files
include(CMakePackageConfigHelpers)

# Set the installation paths
include(GNUInstallDirs)
set(LIB_DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
set(INCLUDE_DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

##### global variables #####

# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library
# install directory. Alex
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${LIB_DESTINATION}" _isSystemPlatformLibDir)
list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${LIB_DESTINATION}" _isSystemCLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${LIB_DESTINATION}" _isSystemCxxLibDir)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
if("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")
set(CMAKE_INSTALL_RPATH                ${LIB_DESTINATION} )
endif()

option(ENABLE_DBUS "enable dbus support in the daemon" ON)
option(ENABLE_INOTIFY "enable monitoring the filesystem with inotify" OFF)
option(ENABLE_FAM "enable monitoring the filesystem with FAM" ON)
option(ENABLE_POLLING "enable monitoring the filesystem via polling" ON)
option(ENABLE_LOG4CXX "enable use of the logging framework Log4CXX" ON)

if(CMAKE_SYSTEM MATCHES "Linux")
    set(ENABLE_INOTIFY ON)
endif(CMAKE_SYSTEM MATCHES "Linux")

if(NOT MSVC AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")
    add_definitions(-fPIC)
endif(NOT MSVC AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")

##### environment inspection #####

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  find_package(LibStreams REQUIRED)
  find_package(LibStreamAnalyzer REQUIRED)
else()
  set(LIBSTREAMS_INCLUDE_DIRS
    ${libstreams_SOURCE_DIR}/include
    ${libstreams_BINARY_DIR}/include
    )
  set(LIBSTREAMANALYZER_INCLUDE_DIRS
    ${libstreamanalyzer_SOURCE_DIR}/include
    ${libstreamanalyzer_BINARY_DIR}/include
    )
  set(LIBSTREAMS_LIBRARIES streams)
  set(LIBSTREAMANALYZER_LIBRARIES streamanalyzer)
endif()

find_package(Threads REQUIRED)
find_package(LibXml2 REQUIRED)
find_optional_dep(DBus ENABLE_DBUS DBUS_FOUND "dbus inter-process communication")
find_optional_dep(FAM ENABLE_FAM FAM_FOUND "efficient file change monitoring system")
find_optional_dep(Log4cxx ENABLE_LOG4CXX LOG4CXX_FOUND "advanced logging")
#find_optional_dep(CLucene ENABLE_CLUCENE CLucene_FOUND "CLucene backend")
#find_optional_dep(Exiv2 ENABLE_EXIV2 EXIV2_FOUND "indexing of EXIF/IPTC metadata")
find_package(CppUnit)
include(ConfigureChecks.cmake)
set (LIBSEARCHCLIENT_VERSION ${STRIGI_VERSION_STRING})

configure_package_config_file(LibSearchClient.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/LibSearchClientConfig.cmake
                              INSTALL_DESTINATION ${LIB_DESTINATION}/cmake/LibSearchClient
                              PATH_VARS INCLUDE_DESTINATION LIB_DESTINATION
)



configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY )
IF (ENABLE_LOG4CXX AND LOG4CXX_FOUND)
    add_definitions(-DHAVE_LOG4CXX)
ENDIF (ENABLE_LOG4CXX AND LOG4CXX_FOUND)


##### building and testing #####

configure_file(${strigidaemon_SOURCE_DIR}/strigi_thread.h.cmake
               ${strigidaemon_BINARY_DIR}/include/strigi/strigi_thread.h)

include_directories(${LIBSTREAMS_INCLUDE_DIRS}
                    ${LIBSTREAMANALYZER_INCLUDE_DIRS}
                    ${strigidaemon_SOURCE_DIR}/include
                    ${strigidaemon_SOURCE_DIR}/bin/daemon/xsd
                    ${strigidaemon_BINARY_DIR}
                    ${strigidaemon_BINARY_DIR}/bin/daemon/dbus
                    ${DBUS_INCLUDE_DIRS}
                    ${ICONV_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR}
                    bin/combinedindex
                    bin/daemon
)
if(LOG4CXX_FOUND)
	include_directories(${LOG4CXX_INCLUDE_DIR})
endif(LOG4CXX_FOUND)
link_directories(${LIBSTREAMS_LIBRARY_DIRS})
link_directories(${LIBSTREAMANALYZER_LIBRARY_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

add_subdirectory(lib)
add_subdirectory(bin)
if(CppUnit_FOUND)
    add_subdirectory(tests)
endif(CppUnit_FOUND)

##### installing #####

# all installed files are listed here
# headers
file(GLOB STRIGI_HEADERS include/strigi/*.h)
install(FILES
    ${STRIGI_HEADERS}
    DESTINATION include/strigi
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LibSearchClientConfig.cmake
        DESTINATION ${LIB_DESTINATION}/cmake/LibSearchClient)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")
add_executable(strigidaemon dummy.cpp)
target_link_libraries(strigidaemon libstrigidaemon)
install(TARGETS strigidaemon
        RUNTIME DESTINATION bin)
