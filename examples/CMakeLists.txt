add_executable(egltest
	egltest.cpp
)
target_link_libraries(egltest EGL GL)

add_executable(egltest_raw
	egltest_raw.cpp
)
target_link_libraries(egltest_raw EGL GL)
