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
cm_example_project("Logger" LoggerTest LoggerTest.cpp)

####################################################################################################
cm_example_project("Debug" DebugObject  debug/DebugObject.cpp)
cm_example_project("Debug" AutoSCL      debug/AutoSCL.cpp)

####################################################################################################
cm_example_project("DataType/RAM" RuntimeAssetManagerTest  datatype/ram/RuntimeAssetManagerTest.cpp
                                                           datatype/ram/RuntimeAssetManager.h
                                                           datatype/ram/RAM_TestClass.h
                                                           datatype/ram/RAM_TestClass.cpp)
####################################################################################################

cm_example_project("DataType" TreeTest           datatype/TreeTest.cpp)

####################################################################################################
cm_example_project("DataType/TypeInfo" TypeSizeof           datatype/TypeInfo/TypeSizeof.cpp)
cm_example_project("DataType/TypeInfo" TypeCastTest         datatype/TypeInfo/TypeCastTest.cpp)
cm_example_project("DataType/TypeInfo" TypeCheck            datatype/TypeInfo/TypeCheck.cpp)
cm_example_project("DataType/TypeInfo" ObjectRelationTest   datatype/TypeInfo/ObjectRelationTest.cpp)
cm_example_project("DataType/TypeInfo" InheritTest          datatype/TypeInfo/InheritTest.cpp)

####################################################################################################
cm_example_project("DataType" UbyteColor            datatype/color_test_example.cpp)
cm_example_project("DataType" HalfFloatTest         datatype/HalfFloatTest.cpp)
cm_example_project("DataType" SplitStringTest       datatype/SplitStringTest.cpp)
cm_example_project("DataType" StrChrTest            datatype/strchr_test.cpp)
cm_example_project("DataType" Uint2StrTest          datatype/utos_test.cpp)
cm_example_project("DataType" Size2Test             datatype/Size2Test.cpp)
cm_example_project("DataType" MemcmpTest            datatype/MemcmpTest.cpp)
cm_example_project("DataType" Uint2HexStrTest       datatype/Uint2HexStrTest.cpp)

cm_example_project("DataType" ConstStringSetTest    datatype/ConstStringSetTest.cpp)
cm_example_project("DataType" IDNameTest            datatype/IDNameTest.cpp)
cm_example_project("DataType" IDObjectManagerTest   datatype/IDObjectManagerTest.cpp)


####################################################################################################
cm_example_project("Math" OutputEpsilon         math/OutputEpsilon.cpp)
cm_example_project("Math" TransformBenchmark    math/TransformBenchmark.cpp)
cm_example_project("Math" RotateMatrix          math/RotateMatrix.cpp)

####################################################################################################
cm_example_project("DataType/Collection" LifetimeTest    datatype/collection/LifetimeTest.cpp)
cm_example_project("DataType/Collection" CollectionTest  datatype/collection/CollectionTest.cpp)
cm_example_project("DataType/Collection" DataArrayTest   datatype/collection/DataArrayTest.cpp)
cm_example_project("DataType/Collection" StackTest       datatype/collection/StackTest.cpp)
cm_example_project("DataType/Collection" QueueTest       datatype/collection/QueueTest.cpp)
cm_example_project("DataType/Collection" PoolTest        datatype/collection/PoolTest.cpp)
cm_example_project("DataType/Collection" ListTest        datatype/collection/ListTest.cpp)
cm_example_project("DataType/Collection" IndexedListTest datatype/collection/IndexedListTest.cpp)
cm_example_project("DataType/Collection" MapTest         datatype/collection/MapTest.cpp)
cm_example_project("DataType/Collection" MultiMapTest    datatype/collection/MultiMapTest.cpp)
cm_example_project("DataType/Collection" StackPoolTest   datatype/collection/StackPoolTest.cpp)
cm_example_project("DataType/Collection" DataChainTest   datatype/collection/DataChainTest.cpp)
cm_example_project("DataType/Collection" DataChainTest2  datatype/collection/DataChainTest2.cpp)
cm_example_project("DataType/Collection" ResManagerTest  datatype/collection/ResourceManagerTest.cpp)
cm_example_project("DataType/Collection" HashSetTest     datatype/collection/HashSetTest.cpp)

cm_example_project("DataType/Collection" MonotonicIDListTest datatype/collection/MonotonicIDListTest.cpp)
####################################################################################################

cm_example_project("Pick" Pick2DTest    pick/Pick2DTest.cpp)
target_link_libraries(Pick2DTest PRIVATE CM2D)

####################################################################################################
cm_example_project("DataType/ActiveManager" 1_ActiveIDManagerTest           datatype/ActiveIDManagerTest.cpp)
cm_example_project("DataType/ActiveManager" 2_ActiveMemoryBlockManagerTest  datatype/ActiveMemoryBlockManagerTest.cpp)
cm_example_project("DataType/ActiveManager" 3_ActiveDataManagerTest         datatype/ActiveDataManagerTest.cpp)

####################################################################################################
cm_example_project("File System" FixFilenameTest    filesystem/FixFilenameTest.cpp)
cm_example_project("File System" EnumFileTest       filesystem/EnumFileTest.cpp)

IF(WIN32)
cm_example_project("File System" EnumVolumeTest     filesystem/EnumVolumeTest.cpp)
ENDIF(WIN32)

####################################################################################################
cm_example_project("Hareware"   GetCpuInfo          GetCpuInfo.cpp)
cm_example_project("Source"     CreateBinaryH       CreateBinaryH.cpp)
cm_example_project("Image"      NormalCompressTest  NormalCompressTest.cpp)
cm_example_project("os"         OSFontList          OSFontList.cpp)
cm_example_project("time"       TimeCount           time/time_count.cpp)

####################################################################################################
cm_example_project("chart" DistributionChart2D  DistributionChart2D.cpp BitmapFont.cpp BitmapFont.h)
target_link_libraries(DistributionChart2D PRIVATE CM2D)
cm_example_project("chart" PlayerTraceChart2D   PlayerTraceChart2D.cpp BitmapFont.cpp BitmapFont.h)
target_link_libraries(PlayerTraceChart2D PRIVATE CM2D)

#cm_example_project("chart" DAGTest   DAGTest.cpp BitmapFont.cpp BitmapFont.h)
#target_link_libraries(DAGTest PRIVATE CM2D)

####################################################################################################
cm_example_project("utils" PAttribTest      utils/PAttribTest.cpp)
cm_example_project("utils" Base64Test       utils/base64test.cpp)
cm_example_project("utils" HashTest         utils/HashTest.cpp)
cm_example_project("utils" SeriesPoolTest   utils/SeriesPoolTest.cpp)

####################################################################################################
OPTION(CM_EXAMPLES_ABOUT_ANDROID    OFF)

IF(CM_EXAMPLES_ABOUT_ANDROID)
add_subdirectory(android)
ENDIF()

