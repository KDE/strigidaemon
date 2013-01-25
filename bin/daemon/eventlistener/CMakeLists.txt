if(ENABLE_INOTIFY)
    set (inotify_SRC inotifylistener.cpp)
endif()

if(FAM_FOUND AND ENABLE_FAM)
    set (fam_SRC famlistener.cpp)
    set (eventlistener_LIBS ${eventlistener_LIBS} ${FAM_LIBRARIES})
endif()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_library(eventlistener STATIC
    event.cpp
    eventlistenerqueue.cpp
    pollinglistener.cpp
    fslistener.cpp	
    ${inotify_SRC}
    ${fam_SRC})

target_link_libraries(eventlistener ${eventlistener_LIBS})
