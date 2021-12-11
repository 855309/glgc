project(glgc)

add_executable(
    glgc
    src/main.cpp
    src/file.cpp
)

find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)
find_package(ImGui-SFML REQUIRED)

target_link_libraries(glgc sfml-audio sfml-graphics sfml-window sfml-system)
target_link_libraries(glgc PRIVATE ImGui-SFML::ImGui-SFML)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/etc/lang.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})