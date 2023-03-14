set(PROJECT_NAME PS4Rendering)

unset(WIN32)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "PS4Frame.h"
    "PS4MemoryAware.h"
    "PS4Mesh.h"
    "PS4Renderer.h"
    "PS4Shader.h"
    "PS4Texture.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "PS4Frame.cpp"
    "PS4MemoryAware.cpp"
    "PS4Mesh.cpp"
    "PS4Renderer.cpp"
    "PS4Shader.cpp"
    "PS4Texture.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set (Shader_Files
    "FragShader.pssl"
    "VertexShader.pssl"
)
source_group("Shader_Files" FILES ${PSSL_Files})

set_property(SOURCE ${PSSL_Files} PROPERTY VS_TOOL_OVERRIDE WavePsslc)
set_property(SOURCE ${PSSL_Files} PROPERTY VS_SETTINGS "GenerateHeader=true")

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${PSSL_Files}
)

set_source_files_properties(${ALL_FILES} PROPERTIES LANGUAGE CXX)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE PS4Rendering)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_KEYWORD "Orbis")

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ";"
    "UNICODE;"
    "_UNICODE"
    "NOMINMAX"	
)

################################################################################
# Compile and link options
################################################################################

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "NCLCoreClasses"
    #"opengl32.lib"
)
include_directories("../NCLCoreClasses/")
include_directories("../PS4Rendering/")

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

set(LIB_DEPENDENCIES "ReactPhysics3D::ReactPhysics3D")

target_link_libraries(${PROJECT_NAME} PUBLIC "${LIB_DEPENDENCIES}")
