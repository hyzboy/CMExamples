cm_example_project("File System" FixFilenameTest    FixFilenameTest.cpp)
cm_example_project("File System" EnumFileTest       EnumFileTest.cpp)

IF(WIN32)
cm_example_project("File System" EnumVolumeTest     EnumVolumeTest.cpp)
ENDIF(WIN32)
