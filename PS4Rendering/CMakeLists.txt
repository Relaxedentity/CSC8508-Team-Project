set(PROJECT_NAME PS4Rendering)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "PS4Renderer.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "PS4Renderer.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

set_source_files_properties(${ALL_FILES} PROPERTIES LANGUAGE CXX)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE PS4Rendering)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
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
#if(MSVC)
#    target_compile_options(${PROJECT_NAME} PRIVATE
#        $<$<CONFIG:Release>:
#            /Oi;
##            /Gy
 #       >
#        /permissive-;
#        /std:c++latest;
#        /sdl;
 #       /W3;
  #      ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
   #     ${DEFAULT_CXX_EXCEPTION_HANDLING};
    #    /Y-
   # )
   # target_link_options(${PROJECT_NAME} PRIVATE
   #     $<$<CONFIG:Release>:
   #         /OPT:REF;
   #         /OPT:ICF
   #     >
   # )
#endif()

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
