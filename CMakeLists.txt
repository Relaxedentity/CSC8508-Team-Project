cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

set(USE_PS4 CACHE BOOL FORCE)
if(USE_PS4)
    add_compile_definitions("USEPS4") 
	set(CMAKE_GENERATOR_PLATFORM ORBIS)
	set(CMAKE_VS_PLATFORM_NAME "ORBIS")
	set(CMAKE_GENERATOR_TOOLSET "Clang")
	unset(WIN32)
endif()

project(CSC8503 CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT (("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64") OR ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ORBIS")))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 23)

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Vulkan )

set(ASSET_ROOT "${CMAKE_SOURCE_DIR}/Assets/" CACHE STRING "" FORCE)
add_compile_definitions(ASSETROOTLOCATION="${ASSET_ROOT}") 

set(USE_VULKAN CACHE BOOL FORCE)
if(USE_VULKAN)
    add_compile_definitions("USEVULKAN") 
endif() 

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ReactPhysics3D") 

find_package(ReactPhysics3D REQUIRED)

find_path(IRRKLANG_INCLUDE_DIR NAMES irrKlang.h PATHS "${CMAKE_CURRENT_SOURCE_DIR}/IrrKlangSound/includes")
    set (IRRKLANG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/IrrKlangSound/libs/Winx64-visualStudio")
    set (IRRKLANG_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/IrrKlangSound/bin")
find_library(IRRKLANG_LIBRARY NAMES irrKlang PATHS ${IRRKLANG_LIB_DIR})
################################################################################
# Sub-projects
################################################################################
add_subdirectory(NCLCoreClasses)
add_subdirectory(CSC8503CoreClasses)
if(USE_PS4)
   add_subdirectory(PS4Rendering)
else()
   add_subdirectory(OpenGLRendering)
endif()
add_subdirectory(CSC8503)
if(USE_VULKAN)
    add_subdirectory(VulkanRendering)
endif()
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CSC8503)