cmake_minimum_required(VERSION 3.13.0)
project(cbui VERSION 0.3.0)


# linux:
if (UNIX)
message("UNIX configuration")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw)


# Windows:
else()
message("WIN32 configuration")
set(DIR_C_LIBS "lib_win")

# glfw / glew
include_directories(${DIR_C_LIBS}/glfw/include/)
link_directories(${DIR_C_LIBS}/glfw/lib/)
include_directories(${DIR_C_LIBS}/glew/include/)
link_directories(${DIR_C_LIBS}/glew/lib/)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} glfw3)
target_link_libraries(${PROJECT_NAME} glew32)
target_link_libraries(${PROJECT_NAME} OpenGL32)


endif()
