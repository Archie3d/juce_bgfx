cmake_minimum_required(VERSION 3.20)

project(test_bgfx VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

# JUCE
add_subdirectory(JUCE)

# bgfx
add_definitions(-DBGFX_CONFIG_MULTITHREADED=0)
add_subdirectory(bgfx.cmake)
set(BGFX_3RD_PARTY_DIR ${BGFX_DIR}/3rdparty)

# stb
set(STB_DIR ${BGFX_3RD_PARTY_DIR}/stb)
file(GLOB_RECURSE STB_SRC
    ${STB_DIR}/*.h
)
add_library(stb INTERFACE ${STB_SRC})
target_include_directories(stb INTERFACE ${STB_DIR} ${BGFX_3RD_PARTY_DIR})

# nanovg
set(NANOVG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nanovg)
file(GLOB_RECURSE NANOVG_SRC
    ${NANOVG_DIR}/*.h
    ${NANOVG_DIR}/*.cpp
)
add_library(nanovg STATIC ${NANOVG_SRC})
target_link_libraries(nanovg PUBLIC bx bimg bgfx stb)
target_include_directories(nanovg PUBLIC ${NANOVG_DIR})

#-----------------------------------------------------------

add_subdirectory(source)
