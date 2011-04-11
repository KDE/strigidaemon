add_library(dbuscpp STATIC dbushandler.cpp dbusmessagereader.cpp
    dbusmessagewriter.cpp dbusobjectcallhandler.cpp
)

target_link_libraries(dbuscpp ${DBUS_LIBRARIES})
