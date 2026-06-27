cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME example)
project("${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable("${PROJECT_NAME}" 
    main.cpp
    view/MainMenu.cpp
    view/GameGUI.cpp
    view/GameResults.cpp
)

set(SFML_ROOT "${CMAKE_SOURCE_DIR}/SFML")

target_include_directories("${PROJECT_NAME}" PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_ROOT}/include
)

target_link_directories("${PROJECT_NAME}" PRIVATE
    ${SFML_ROOT}/lib
)


target_link_libraries( "${PROJECT_NAME}"
    sfml-graphics
    sfml-window
    sfml-system
)