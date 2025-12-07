macro(cm_example_project sub_folder project_name)

    add_executable(${project_name} ${ARGN})

    target_link_libraries(${project_name} PRIVATE CMCore CMPlatform CMUtil tsl::robin_map)

    if(UNIX)
        target_link_libraries(${project_name} PRIVATE dl)
    endif()

    IF(WIN32)
        target_link_libraries(${project_name} PRIVATE ${HGL_MATH_LIB})

        set_debugger_directory(${project_name} ${CMAKE_CURRENT_SOURCE_DIR})

        target_sources(${project_name} PRIVATE ${CM_MANIFEST})
        
        if(MSVC)
            target_sources(${project_name} PRIVATE ${CM_NATVIS})
            SOURCE_GROUP("natvis" FILES ${CM_NATVIS})
        endif()

    ENDIF()

    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples/${sub_folder}")
endmacro()

####################################################################################################
# Root level examples
####################################################################################################
cm_example_project("Logger"    LoggerTest                  LoggerTest.cpp)
cm_example_project("Hareware"  GetCpuInfo                  GetCpuInfo.cpp)
cm_example_project("Hareware"  BigLittleTaskScheduler      BigLittleTaskScheduler.cpp)
cm_example_project("Hareware"  ArchTest                    ArchTest.cpp)
cm_example_project("Source"    CreateBinaryH               CreateBinaryH.cpp)
cm_example_project("Image"     NormalCompressTest          NormalCompressTest.cpp)
cm_example_project("os"        OSFontList                  OSFontList.cpp)

cm_example_project("chart" DistributionChart2D  DistributionChart2D.cpp BitmapFont.cpp BitmapFont.h)
target_link_libraries(DistributionChart2D PRIVATE CM2D)
cm_example_project("chart" PlayerTraceChart2D   PlayerTraceChart2D.cpp BitmapFont.cpp BitmapFont.h)
target_link_libraries(PlayerTraceChart2D PRIVATE CM2D)

#cm_example_project("chart" DAGTest   DAGTest.cpp BitmapFont.cpp BitmapFont.h)
#target_link_libraries(DAGTest PRIVATE CM2D)

####################################################################################################
# Subdirectories
####################################################################################################
add_subdirectory(debug)
add_subdirectory(datatype)
add_subdirectory(math)
add_subdirectory(pick)
add_subdirectory(filesystem)
add_subdirectory(time)
add_subdirectory(utils)

####################################################################################################
OPTION(CM_EXAMPLES_ABOUT_ANDROID    OFF)

IF(CM_EXAMPLES_ABOUT_ANDROID)
add_subdirectory(android)
ENDIF()

