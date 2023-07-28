macro(cm_android_project project_name)
    target_link_libraries(${project_name} PRIVATE CMCore CMPlatform CMUtil)

    if(UNIX)
        target_link_libraries(${project_name} PRIVATE dl)
    endif()

    IF(WIN32)
        target_link_libraries(${project_name} PRIVATE ${HGL_MATH_LIB})

        set_debugger_directory(${project_name} ${CMAKE_CURRENT_SOURCE_DIR})

        target_sources(${project_name} PRIVATE ${CM_MANIFEST})
    ENDIF()

    set_property(TARGET ${project_name} PROPERTY FOLDER "CM/Examples/android")
endmacro()

####################################################################################################
SET(LOAD_SOC_DB_SOURCE  SOCDataBase/LoadSOC.cpp)

add_library(SOCDataBase ${LOAD_SOC_DB_SOURCE})

cm_android_project(SOCDataBase)

####################################################################################################
SET(ADA_LOAD_GAME_RECORD_SOURCE AndroidDeviceAnalysis/ParseDeviceID.cpp
                                AndroidDeviceAnalysis/ParseScreenSize.cpp
                                AndroidDeviceAnalysis/ParseDateTime.cpp
                                AndroidDeviceAnalysis/ParseAndroidVersion.cpp
                                AndroidDeviceAnalysis/ParseNumber.cpp
                                AndroidDeviceAnalysis/LoadRecordFile.cpp
                                AndroidDeviceAnalysis/EnumCSVFile.cpp)

source_group("Load GameRecord" FILES ${ADA_LOAD_GAME_RECORD_SOURCE})

add_executable(AndroidDeviceAnalysis    AndroidDeviceAnalysis/main.cpp
					                    AndroidDeviceAnalysis/GameRecord.h
                                        AndroidDeviceAnalysis/GameRecord.cpp
                                        AndroidDeviceAnalysis/AndroidDeviceRecord.cpp
#                                        AndroidDeviceAnalysis/ExportReport.cpp
                                        ${ADA_LOAD_GAME_RECORD_SOURCE})

cm_android_project(AndroidDeviceAnalysis)
target_link_libraries(AndroidDeviceAnalysis PRIVATE SOCDataBase)
