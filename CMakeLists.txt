﻿macro(cm_example_project sub_folder project_name)
    target_link_libraries(${project_name} PRIVATE CMCore CMPlatform CMUtil tsl::robin_map)

    if(UNIX)
        target_link_libraries(${project_name} PRIVATE dl)
    endif()

    IF(WIN32)
        target_link_libraries(${project_name} PRIVATE ${HGL_MATH_LIB})

        set_debugger_directory(${project_name} ${CMAKE_CURRENT_SOURCE_DIR})

        target_sources(${project_name} PRIVATE ${CM_MANIFEST})
        
        if(MSVC)
            target_sources(${project_name} INTERFACE
                           ${CM_NATVIS})
        endif()

    ENDIF()

    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples/${sub_folder}")
endmacro()

macro(set_example_project_folder sub_folder project_name)
    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples/${sub_folder}")
endmacro()

####################################################################################################
add_executable(DebugObject debug/DebugObject.cpp)
CM_EXAMPLE_PROJECT("Debug" DebugObject)

####################################################################################################
add_executable(TypeSizeof datatype/TypeSizeof.cpp)
CM_EXAMPLE_PROJECT("DataType" TypeSizeof)

add_executable(TypeCastTest datatype/TypeCastTest.cpp)
cm_example_project("DataType" TypeCastTest)

add_executable(HalfFloatTest datatype/HalfFloatTest.cpp)
cm_example_project("DataType" HalfFloatTest)

add_executable(SplitStringTest datatype/SplitStringTest.cpp)
cm_example_project("DataType" SplitStringTest ${CM_NATVIS})

add_executable(StrChrTest datatype/strchr_test.cpp)
cm_example_project("DataType" StrChrTest)

add_executable(Uint2StrTest datatype/utos_test.cpp)
cm_example_project("DataType" Uint2StrTest)

add_executable(RuntimeAssetManagerTest datatype/ram/RuntimeAssetManagerTest.cpp
                                       datatype/ram/RuntimeAssetManager.h
                                       datatype/ram/RAM_TestClass.h
                                       datatype/ram/RAM_TestClass.cpp)

cm_example_project("DataType/RAM" RuntimeAssetManagerTest)

add_executable(Size2Test datatype/Size2Test.cpp)
cm_example_project("DataType" Size2Test)

add_executable(Uint2HexStrTest datatype/Uint2HexStrTest.cpp)
cm_example_project("DataType" Uint2HexStrTest)

add_executable(ConstStringSetTest datatype/ConstStringSetTest.cpp)
cm_example_project("DataType" ConstStringSetTest)

add_executable(IDNameTest datatype/IDNameTest.cpp)
cm_example_project("DataType" IDNameTest)

####################################################################################################

add_executable(OutputEpsilon math/OutputEpsilon.cpp)
CM_EXAMPLE_PROJECT("Math" OutputEpsilon)

add_executable(TransformBenchmark math/TransformBenchmark.cpp)
CM_EXAMPLE_PROJECT("Math" TransformBenchmark)

####################################################################################################

add_executable(LifetimeTest datatype/LifetimeTest.cpp)
set_example_project_folder("DataType/DataArray" LifetimeTest)

add_executable(CollectionTest datatype/CollectionTest.cpp)
cm_example_project("DataType/DataArray" CollectionTest)

add_executable(DataArrayTest datatype/DataArrayTest.cpp)
set_example_project_folder("DataType/DataArray" DataArrayTest)

add_executable(StackTest datatype/StackTest.cpp)
set_example_project_folder("DataType/DataArray" StackTest)

add_executable(QueueTest datatype/QueueTest.cpp)
set_example_project_folder("DataType/DataArray" QueueTest)

add_executable(PoolTest datatype/PoolTest.cpp)
set_example_project_folder("DataType/DataArray" PoolTest)

add_executable(MapTest datatype/MapTest.cpp)
set_example_project_folder("DataType/DataArray" MapTest)

add_executable(StackPoolTest datatype/StackPoolTest.cpp)
set_example_project_folder("DataType/DataArray" StackPoolTest)

add_executable(DataChainTest datatype/DataChainTest.cpp)
cm_example_project("DataType/DataArray" DataChainTest)

add_executable(DataChainTest2 datatype/DataChainTest2.cpp)
cm_example_project("DataType/DataArray" DataChainTest2)
####################################################################################################

add_executable(Pick2DTest pick/Pick2DTest.cpp)
cm_example_project("Pick" Pick2DTest)
target_link_libraries(Pick2DTest PRIVATE CM2D)

####################################################################################################

add_executable(1_ActiveIDManagerTest datatype/ActiveIDManagerTest.cpp)
cm_example_project("DataType/ActiveManager" 1_ActiveIDManagerTest)

add_executable(2_ActiveMemoryBlockManagerTest datatype/ActiveMemoryBlockManagerTest.cpp)
cm_example_project("DataType/ActiveManager" 2_ActiveMemoryBlockManagerTest)

add_executable(3_ActiveDataManagerTest datatype/ActiveDataManagerTest.cpp)
cm_example_project("DataType/ActiveManager" 3_ActiveDataManagerTest)

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

add_executable(TimeCount time/time_count.cpp)
cm_example_project("time" TimeCount)

####################################################################################################

add_executable(DistributionChart2D DistributionChart2D.cpp BitmapFont.cpp BitmapFont.h)
cm_example_project("chart" DistributionChart2D)
target_link_libraries(DistributionChart2D PRIVATE CM2D)

add_executable(PlayerTraceChart2D PlayerTraceChart2D.cpp BitmapFont.cpp BitmapFont.h)
cm_example_project("chart" PlayerTraceChart2D)
target_link_libraries(PlayerTraceChart2D PRIVATE CM2D)

####################################################################################################

add_executable(PAttribTest utils/PAttribTest.cpp)
cm_example_project("utils" PAttribTest)

add_executable(Base64Test utils/base64test.cpp)
cm_example_project("utils" Base64Test)

add_executable(HashTest utils/HashTest.cpp)
cm_example_project("utils" HashTest)

add_executable(SeriesPoolTest utils/SeriesPoolTest.cpp)
cm_example_project("utils" SeriesPoolTest)

####################################################################################################
OPTION(CM_EXAMPLES_ABOUT_ANDROID    OFF)

IF(CM_EXAMPLES_ABOUT_ANDROID)
add_subdirectory(android)
ENDIF()

