
macro(cm_example_project project_name)
    target_link_libraries(${project_name} PRIVATE CMCore CMPlatform CMUtil)

    if(UNIX)
        target_link_libraries(${project_name} PRIVATE dl)
    endif()

    IF(WIN32)
        target_link_libraries(${project_name} PRIVATE ${HGL_MATH_LIB})

        set_debugger_directory(${project_name} ${CMAKE_CURRENT_SOURCE_DIR})

        target_sources(${project_name} PRIVATE ${CM_MANIFEST})
    ENDIF()

    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples")
endmacro()

add_executable(HalfFloatTest HalfFloatTest.cpp)
cm_example_project(HalfFloatTest)

add_executable(GetCpuInfo GetCpuInfo.cpp)
cm_example_project(GetCpuInfo)

add_executable(CollectionTest CollectionTest.cpp)
cm_example_project(CollectionTest)

add_executable(FixFilenameTest FixFilenameTest.cpp)
cm_example_project(FixFilenameTest)

IF(WIN32)

    add_executable(EnumVolumeTest EnumVolumeTest.cpp)
    cm_example_project(EnumVolumeTest)

ENDIF(WIN32)

add_executable(SplitStringTest SplitStringTest.cpp)
cm_example_project(SplitStringTest)

add_executable(CreateBinaryH CreateBinaryH.cpp)
cm_example_project(CreateBinaryH)

add_executable(NormalCompressTest NormalCompressTest.cpp)
CM_EXAMPLE_PROJECT(NormalCompressTest)

add_executable(MultiMapTest MultiMapTest.cpp)
cm_example_project(MultiMapTest)

add_executable(OSFontList OSFontList.cpp)
cm_example_project(OSFontList)
target_link_libraries(OSFontList PRIVATE CMUtil)

add_executable(RuntimeAssetManagerTest RuntimeAssetManagerTest.cpp RuntimeAssetManager.h RAM_TestClass.h RAM_TestClass.cpp)
cm_example_project(RuntimeAssetManagerTest)

add_executable(DistributionChart2D DistributionChart2D.cpp)
cm_example_project(DistributionChart2D)