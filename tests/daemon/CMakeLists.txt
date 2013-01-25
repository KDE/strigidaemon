# add a test for the daemon configuration loading classes

if (QT4_FOUND AND DBUS_FOUND AND DBUS_DBUSLAUNCH_EXECUTABLE)
    add_subdirectory(dbus)
endif()

add_executable(daemonconfiguratortest daemonconfiguratortest.cpp)
target_link_libraries(daemonconfiguratortest strigi_test_runner
                      libstrigidaemon
                      streamanalyzer
                      ${LOG4CXX_LIBRARIES}
                     )

add_test (daemonconfiguratortest daemonconfiguratortest)
