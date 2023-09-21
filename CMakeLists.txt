cmake_minimum_required(VERSION 3.5)
project(spovm)

find_package(SQLite3 REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

#if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  set(CMAKE_HAVE_THREADS_LIBRARY 1)
  set(CMAKE_USE_WIN32_THREADS_INIT 0)
  set(CMAKE_USE_PTHREADS_INIT 1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
#endif()

set(SOURCES 
    src/main.cpp
    src/filter.cpp
    src/DB.cpp
    src/walker.cpp
    src/executor.cpp
)

set(HEADERS
    "include/filter.hpp"
    "include/DB.hpp"
    "include/walker.hpp"
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
#add_executable(test ${PROJECT_SOURCE_DIR}/example/sandbox.cpp)

target_link_libraries(${PROJECT_NAME} ${SQLite3_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

install(TARGETS
    DESTINATION bin/${PROJECT_NAME}
    )
