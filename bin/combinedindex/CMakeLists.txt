add_library(combinedindex STATIC combinedindexmanager.cpp)

target_link_libraries(combinedindex streamanalyzer)
