cm_example_project("Math" OutputEpsilon         OutputEpsilon.cpp)
cm_example_project("Math" TransformBenchmark    TransformBenchmark.cpp)
target_link_libraries(TransformBenchmark PRIVATE CMMath)
cm_example_project("Math" TransformTest         TransformTest.cpp)
target_link_libraries(TransformTest PRIVATE CMMath)
cm_example_project("Math" RotateMatrix          RotateMatrix.cpp)

# Geometry Test Suite
cm_example_project("Math" GeometryTest          GeometryTest.cpp)
target_link_libraries(GeometryTest PRIVATE CMMath)
set_property(TARGET GeometryTest PROPERTY CXX_STANDARD 20)


cm_example_project("Math" BoxTest          BoxTest.cpp)
target_link_libraries(BoxTest PRIVATE CMMath)
set_property(TARGET BoxTest PROPERTY CXX_STANDARD 20)
