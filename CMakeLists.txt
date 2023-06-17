# cmake最低要求
cmake_minimum_required( VERSION 3.18 )

if(POLICY CMP0091)
	cmake_policy(SET CMP0091 NEW)
endif(POLICY CMP0091)

# 声明一个项目
project(USBRelay)

# 基于c++11
set(CMAKE_CXX_STANDARD 17)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_BUILD_TYPE Debug)

set(SOURCES
src/main.cpp
include/usb_relay_device.h
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)



# 添加可执行文件，注意添加WIN32，否则有一个控制台黑框
add_executable(USBRelay
${SOURCES}
)

set_property(TARGET USBRelay PROPERTY
			MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# 链接库
target_link_libraries(USBRelay usb_relay_device)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/lib/usb_relay_device.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/config.ini"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
endif()