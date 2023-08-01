macro(cm_example_project sub_folder project_name)
    target_link_libraries(${project_name} PRIVATE CMCore CMPlatform CMUtil)

    if(UNIX)
        target_link_libraries(${project_name} PRIVATE dl)
    endif()

    IF(WIN32)
        target_link_libraries(${project_name} PRIVATE ${HGL_MATH_LIB})

        set_debugger_directory(${project_name} ${CMAKE_CURRENT_SOURCE_DIR})

        target_sources(${project_name} PRIVATE ${CM_MANIFEST})
    ENDIF()

    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples/${sub_folder}")
endmacro()

####################################################################################################

add_executable(HalfFloatTest datatype/HalfFloatTest.cpp)
cm_example_project("DataType" HalfFloatTest)

add_executable(LifetimeTest datatype/LifetimeTest.cpp)
#cm_example_project("DataType" LifetimeTest)
set_property(TARGET LifetimeTest PROPERTY FOLDER "CM/Examples/DataType")

add_executable(CollectionTest datatype/CollectionTest.cpp)
cm_example_project("DataType" CollectionTest)

add_executable(SplitStringTest datatype/SplitStringTest.cpp)
cm_example_project("DataType" SplitStringTest)

add_executable(StrChrTest datatype/strchr_test.cpp)
cm_example_project("DataType" StrChrTest)

add_executable(Uint2StrTest datatype/utos_test.cpp)
cm_example_project("DataType" Uint2StrTest)

add_executable(MapTest datatype/MapTest.cpp)
cm_example_project("DataType" MapTest)

add_executable(MultiMapTest datatype/MultiMapTest.cpp)
cm_example_project("DataType" MultiMapTest)

add_executable(RuntimeAssetManagerTest datatype/ram/RuntimeAssetManagerTest.cpp
                                       datatype/ram/RuntimeAssetManager.h
                                       datatype/ram/RAM_TestClass.h
                                       datatype/ram/RAM_TestClass.cpp)

cm_example_project("DataType/RAM" RuntimeAssetManagerTest)

add_executable(Size2Test datatype/Size2Test.cpp)
cm_example_project("DataType" Size2Test)

add_executable(DataArrayTest datatype/DataArrayTest.cpp)
cm_example_project("DataType" DataArrayTest)

add_executable(Uint2HexStrTest datatype/Uint2HexStrTest.cpp)
cm_example_project("DataType" Uint2HexStrTest)

add_executable(StackTest datatype/StackTest.cpp)
cm_example_project("DataType" StackTest)

add_executable(QueueTest datatype/QueueTest.cpp)
cm_example_project("DataType" QueueTest)

####################################################################################################

add_executable(FixFilenameTest filesystem/FixFilenameTest.cpp)
cm_example_project("File System" FixFilenameTest)

add_executable(EnumFileTest filesystem/EnumFileTest.cpp)
cm_example_project("File System" EnumFileTest)

IF(WIN32)

    add_executable(EnumVolumeTest filesystem/EnumVolumeTest.cpp)
    cm_example_project("File System" EnumVolumeTest)

ENDIF(WIN32)

####################################################################################################

add_executable(GetCpuInfo GetCpuInfo.cpp)
cm_example_project("Hareware" GetCpuInfo)

add_executable(CreateBinaryH CreateBinaryH.cpp)
cm_example_project("" CreateBinaryH)

add_executable(NormalCompressTest NormalCompressTest.cpp)
CM_EXAMPLE_PROJECT("" NormalCompressTest)

add_executable(OSFontList OSFontList.cpp)
cm_example_project("os" OSFontList)
target_link_libraries(OSFontList PRIVATE CMUtil)

add_executable(DistributionChart2D DistributionChart2D.cpp BitmapFont.cpp BitmapFont.h)
cm_example_project("chart" DistributionChart2D)

add_executable(TimeCount time/time_count.cpp)
cm_example_project("time" TimeCount)

####################################################################################################

add_executable(PAttribTest utils/PAttribTest.cpp)
cm_example_project("utils" PAttribTest)

add_executable(Base64Test utils/base64test.cpp)
cm_example_project("utils" Base64Test)

add_executable(HashTest utils/HashTest.cpp)
cm_example_project("utils" HashTest)

####################################################################################################
OPTION(CM_EXAMPLES_ABOUT_ANDROID    OFF)

IF(CM_EXAMPLES_ABOUT_ANDROID)
add_subdirectory(android)
ENDIF()

