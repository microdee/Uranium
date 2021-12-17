# ******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(UraniumCefProcess
	VERSION 0.3.0
	LANGUAGES CXX
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(URCP_IS_X64 ON)
else()
	set(URCP_IS_X64 OFF)
endif()

set(URCP_PLATFORM "Win64" CACHE STRING "The target platform using the Unreal platform name")
set(URCP_CEF_VERSION "85.0.4183.121.ST" CACHE STRING "The target CEF distributable version.")

set(URCP_CEF_ROOT ${CMAKE_SOURCE_DIR}/ThirdParty/CEF/${URCP_PLATFORM}/${URCP_CEF_VERSION})
set(URCP_SRC_ROOT ${CMAKE_SOURCE_DIR}/UraniumCefProcess)
set(URCP_SHARED_SRC "${CMAKE_SOURCE_DIR}/CefCommon/Shared")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${URCP_CEF_ROOT}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(URCP_MAIN_FILES
	"${URCP_SRC_ROOT}/main.cpp"
	"${URCP_SRC_ROOT}/UraniumCefApp.h"
	"${URCP_SRC_ROOT}/UraniumCefApp.cpp"
	"${URCP_SRC_ROOT}/CefModuleLoader.h"
	"${URCP_SRC_ROOT}/CefModuleLoader/NativeCefModuleLoader.h"
	"${URCP_SRC_ROOT}/CefModuleLoader/NativeCefModuleLoader.cpp"
)

source_group( "Main" FILES ${URCP_MAIN_FILES} )

set(URCP_SHARED_FILES
	"${URCP_SHARED_SRC}/CefV8Utils.h"
	"${URCP_SHARED_SRC}/CefCompat.h"
	"${URCP_SHARED_SRC}/CefV8Utils.cpp"
	"${URCP_SHARED_SRC}/CefIncludesStart.h"
	"${URCP_SHARED_SRC}/CefIncludesEnd.h"
)

source_group( "Shared" FILES ${URCP_SHARED_FILES} )

add_executable(UraniumCefProcess WIN32
	${URCP_MAIN_FILES}
	${URCP_SHARED_FILES}
)

set(CEF_ROOT ${URCP_CEF_ROOT})
find_package(CEF REQUIRED)

ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
SET_EXECUTABLE_TARGET_PROPERTIES(UraniumCefProcess)
SET_CEF_TARGET_OUT_DIR()

target_include_directories(UraniumCefProcess PUBLIC
	${CMAKE_SOURCE_DIR}/UraniumCefProcess
	${URCP_SHARED_SRC}
	${_CEF_ROOT}
)

add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

add_dependencies(UraniumCefProcess libcef_dll_wrapper)

target_link_libraries(UraniumCefProcess
	libcef_lib
	libcef_dll_wrapper
)

if (MSVC)
	add_custom_command(
		TARGET UraniumCefProcess
		POST_BUILD
		COMMAND "mt.exe" -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\UraniumCefProcess\\app.manifest\" -outputresource:\"$<TARGET_FILE:UraniumCefProcess>\"\;1 -nologo
		COMMENT "Adding app manifest..." 
	)
endif(MSVC)

PRINT_CEF_CONFIG()