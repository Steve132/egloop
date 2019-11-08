cmake_minimum_required(VERSION 3.10)
project(egltest)
add_executable(egltest
	egltest.cpp
)
target_link_libraries(egltest EGL OpenGL)
add_executable(egltest2
	egltest2.cpp)
target_link_libraries(egltest2 EGL OpenGL)
