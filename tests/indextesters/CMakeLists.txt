set(indextester_SRCS indextest.cpp
                     indexreadertester.cpp
                     indexwritertester.cpp
                     indexsearchtester.cpp
                     indexmanagertester.cpp
                    )

if(CLucene_FOUND)
  set(indextester_SRCS ${indextester_SRCS} clucenetests.cpp)
  set(indextester_DEPS ${indextester_DEPS} clucene)
endif()

add_executable(indextester ${indextester_SRCS})
if (indextester_DEPS)
  add_dependencies(indextester ${indextester_DEPS})
endif()

target_link_libraries(indextester
                      strigi_test_runner
                      unittestfunctions
                      ${LOG4CXX_LIBRARIES}
                     )

add_test(indextester indextester)
