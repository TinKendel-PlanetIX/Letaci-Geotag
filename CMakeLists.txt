cmake_minimum_required(VERSION 3.15)
project(Geotag LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(EXIV2_ROOT ${CMAKE_SOURCE_DIR}/third_party/exiv2)

include_directories(${EXIV2_ROOT}/include)

link_directories(${EXIV2_ROOT}/lib)

add_executable(Geotag src/main.cpp)

# Link against Exiv2
target_link_libraries(Geotag PRIVATE exiv2)

add_custom_command(TARGET Geotag POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${EXIV2_ROOT}/bin/exiv2.dll"
        "$<TARGET_FILE_DIR:Geotag>"
)