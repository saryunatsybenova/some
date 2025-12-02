project(course_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ”казываем путь к GLFW
set(GLFW_ROOT "D:/glfw-3.4.bin.WIN64")

add_executable(course_project
        main.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        include/model/Student.h
 )

target_include_directories(course_project PRIVATE
        include
        imgui
        imgui/backends
        ${GLFW_ROOT}/include
)

target_compile_features(course_project PRIVATE cxx_std_20)

# ƒобавл€ем путь к библиотекам
target_link_directories(course_project PRIVATE ${GLFW_ROOT}/lib-vc2022)

find_package(OpenGL REQUIRED)

# »спользуем просто им€ библиотеки, так как путь уже добавлен через target_link_directories
target_link_libraries(course_project PRIVATE 
    glfw3
    OpenGL::GL
)

target_compile_definitions(course_project PRIVATE SLOG_ENABLED)

function(add_test_executable name)
    add_executable(${name} ${ARGN})
    target_include_directories(${name} PRIVATE include)
    target_compile_features(${name} PRIVATE cxx_std_20)
endfunction()