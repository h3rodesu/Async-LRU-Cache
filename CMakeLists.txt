cmake_minimum_required(VERSION 3.20)
project(Asynchronous_LRU_Cache VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    main.cpp
)

set(HEADERS
    NewCache.h
    Cache.ipp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# ---- Зависимости ----
find_package(libpqxx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE libpqxx::pqxx)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()