
macro(cm_example_project project_name)
    target_link_libraries(${project_name} PRIVATE CMCore CMPlatform)

    if(UNIX)
        target_link_libraries(${project_name} PRIVATE dl)
    endif()

    IF(WIN32)
        set_debugger_directory(${project_name} ${CMAKE_CURRENT_SOURCE_DIR})
    ENDIF()

    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples")
endmacro()


add_executable(GetCpuInfo GetCpuInfo.cpp)
cm_example_project(GetCpuInfo)

add_executable(CollectionTest CollectionTest.cpp)
cm_example_project(CollectionTest)
