project(glgc)

add_executable(
    glgc
    src/main.cpp
    src/expr.cpp
)

find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)
find_package(ImGui-SFML REQUIRED)

target_link_libraries(glgc sfml-audio sfml-graphics sfml-window sfml-system)
target_link_libraries(glgc PRIVATE ImGui-SFML::ImGui-SFML)