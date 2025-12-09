cm_example_project("Math" OutputEpsilon         OutputEpsilon.cpp)
cm_example_project("Math" TransformBenchmark    TransformBenchmark.cpp)
target_link_libraries(TransformBenchmark PRIVATE CMMath)
cm_example_project("Math" TransformTest         TransformTest.cpp)
target_link_libraries(TransformTest PRIVATE CMMath)
cm_example_project("Math" RotateMatrix          RotateMatrix.cpp)

# Geometry Test Suite - 拆分为多个独立测试
cm_example_project("Math" AABBTest              AABBTest.cpp)
target_link_libraries(AABBTest PRIVATE CMMath)
set_property(TARGET AABBTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" OBBTest               OBBTest.cpp)
target_link_libraries(OBBTest PRIVATE CMMath)
set_property(TARGET OBBTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" BoundingSphereTest    BoundingSphereTest.cpp)
target_link_libraries(BoundingSphereTest PRIVATE CMMath)
set_property(TARGET BoundingSphereTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" BoundingVolumesTest   BoundingVolumesTest.cpp)
target_link_libraries(BoundingVolumesTest PRIVATE CMMath)
set_property(TARGET BoundingVolumesTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" FrustumTest           FrustumTest.cpp)
target_link_libraries(FrustumTest PRIVATE CMMath)
set_property(TARGET FrustumTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" SphereTest            SphereTest.cpp)
target_link_libraries(SphereTest PRIVATE CMMath)
set_property(TARGET SphereTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" RayTest               RayTest.cpp)
target_link_libraries(RayTest PRIVATE CMMath)
set_property(TARGET RayTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" LineSegmentTest       LineSegmentTest.cpp)
target_link_libraries(LineSegmentTest PRIVATE CMMath)
set_property(TARGET LineSegmentTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" PlaneTest             PlaneTest.cpp)
target_link_libraries(PlaneTest PRIVATE CMMath)
set_property(TARGET PlaneTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" TriangleTest          TriangleTest.cpp)
target_link_libraries(TriangleTest PRIVATE CMMath)
set_property(TARGET TriangleTest PROPERTY CXX_STANDARD 20)

cm_example_project("Math" CoordinateSystemTest  CoordinateSystemTest.cpp)
target_link_libraries(CoordinateSystemTest PRIVATE CMMath)
set_property(TARGET CoordinateSystemTest PROPERTY CXX_STANDARD 20)

# 保留原有的GeometryTest（包含所有测试和集成测试）
cm_example_project("Math" GeometryTest          GeometryTest.cpp)
target_link_libraries(GeometryTest PRIVATE CMMath)
set_property(TARGET GeometryTest PROPERTY CXX_STANDARD 20)
