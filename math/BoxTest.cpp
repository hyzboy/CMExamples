#include<hgl/math/geometry/Box.h>
#include<hgl/math/geometry/AABB.h>
#include<hgl/math/geometry/OBB.h>
#include<hgl/math/geometry/Frustum.h>
#include<hgl/math/geometry/Sphere.h>
#include<hgl/math/geometry/Ray.h>
#include<hgl/math/geometry/Plane.h>
#include<hgl/math/geometry/Triangle.h>
#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
#include<iostream>
#include<cassert>
#include<cmath>

using namespace hgl::math;

// 测试辅助函数
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAILED: " << message << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        std::cout << "Running " << #test_func << "..." << std::endl; \
        if (test_func()) { \
            std::cout << "  PASSED" << std::endl; \
            passed++; \
        } else { \
            std::cout << "  FAILED" << std::endl; \
            failed++; \
        } \
        total++; \
    } while(0)

bool floatEqual(float a, float b, float epsilon = 0.0001f) {
    return std::fabs(a - b) < epsilon;
}

bool vec3Equal(const Vector3f& a, const Vector3f& b, float epsilon = 0.0001f) {
    return floatEqual(a.x, b.x, epsilon) && 
           floatEqual(a.y, b.y, epsilon) && 
           floatEqual(a.z, b.z, epsilon);
}

// ============================================================================
// Box Tests - 基础功能
// ============================================================================

bool test_Box_Construction() {
    Box box;
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(0, 0, 0)), "Default Box center should be (0,0,0)");
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(1, 1, 1)), "Default Box extent should be (1,1,1)");
    
    Box box2(Vector3f(1, 2, 3), Vector3f(4, 5, 6));
    TEST_ASSERT(vec3Equal(box2.GetCenter(), Vector3f(1, 2, 3)), "Box center not set correctly");
    TEST_ASSERT(vec3Equal(box2.GetExtent(), Vector3f(4, 5, 6)), "Box extent not set correctly");
    return true;
}

bool test_Box_FromAABB() {
    AABB aabb;
    aabb.SetMinMax(Vector3f(-2, -3, -4), Vector3f(2, 3, 4));
    
    Box box(aabb);
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(0, 0, 0)), "Box from AABB center wrong");
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(2, 3, 4)), "Box from AABB extent wrong");
    
    AABB aabb2 = box.ToAABB();
    TEST_ASSERT(vec3Equal(aabb2.GetMin(), Vector3f(-2, -3, -4)), "Box.ToAABB() min wrong");
    TEST_ASSERT(vec3Equal(aabb2.GetMax(), Vector3f(2, 3, 4)), "Box.ToAABB() max wrong");
    return true;
}

bool test_Box_GetMinMax() {
    Box box(Vector3f(5, 5, 5), Vector3f(2, 3, 4));
    
    Vector3f min = box.GetMin();
    Vector3f max = box.GetMax();
    
    TEST_ASSERT(vec3Equal(min, Vector3f(3, 2, 1)), "Box GetMin() wrong");
    TEST_ASSERT(vec3Equal(max, Vector3f(7, 8, 9)), "Box GetMax() wrong");
    TEST_ASSERT(vec3Equal(box.GetSize(), Vector3f(4, 6, 8)), "Box GetSize() wrong");
    return true;
}

bool test_Box_SetMinMax() {
    Box box;
    box.SetMinMax(Vector3f(-5, -10, -15), Vector3f(5, 10, 15));
    
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(0, 0, 0)), "Box SetMinMax center wrong");
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(5, 10, 15)), "Box SetMinMax extent wrong");
    return true;
}

bool test_Box_GetCorners() {
    Box box(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    Vector3f corners[8];
    box.GetCorners(corners);
    
    TEST_ASSERT(vec3Equal(corners[0], Vector3f(-1, -1, -1)), "Box corner 0 wrong");
    TEST_ASSERT(vec3Equal(corners[7], Vector3f(1, 1, 1)), "Box corner 7 wrong");
    return true;
}

// ============================================================================
// Box Tests - 点操作
// ============================================================================

bool test_Box_ContainsPoint() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    TEST_ASSERT(box.ContainsPoint(Vector3f(0, 0, 0)), "Box should contain center point");
    TEST_ASSERT(box.ContainsPoint(Vector3f(3, 3, 3)), "Box should contain interior point");
    TEST_ASSERT(box.ContainsPoint(Vector3f(5, 5, 5)), "Box should contain boundary point");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(6, 0, 0)), "Box should not contain exterior point");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(0, 6, 0)), "Box should not contain exterior point Y");
    return true;
}

bool test_Box_ClosestPoint() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    Vector3f closest1 = box.ClosestPoint(Vector3f(0, 0, 0));
    TEST_ASSERT(vec3Equal(closest1, Vector3f(0, 0, 0)), "Closest point to center should be center");
    
    Vector3f closest2 = box.ClosestPoint(Vector3f(10, 0, 0));
    TEST_ASSERT(vec3Equal(closest2, Vector3f(5, 0, 0)), "Closest point outside should be on boundary");
    
    Vector3f closest3 = box.ClosestPoint(Vector3f(-10, 10, -10));
    TEST_ASSERT(vec3Equal(closest3, Vector3f(-5, 5, -5)), "Closest point outside corner should be corner");
    return true;
}

bool test_Box_DistanceToPoint() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    float dist1 = box.DistanceToPoint(Vector3f(0, 0, 0));
    TEST_ASSERT(floatEqual(dist1, 0.0f), "Distance from center should be 0");
    
    float dist2 = box.DistanceToPoint(Vector3f(3, 3, 3));
    TEST_ASSERT(floatEqual(dist2, 0.0f), "Distance from interior point should be 0");
    
    float dist3 = box.DistanceToPoint(Vector3f(10, 0, 0));
    TEST_ASSERT(floatEqual(dist3, 5.0f), "Distance from exterior point should be 5");
    return true;
}

// ============================================================================
// Box Tests - Box间操作
// ============================================================================

bool test_Box_IntersectsBox() {
    Box box1(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    Box box2(Vector3f(4, 0, 0), Vector3f(3, 3, 3));
    Box box3(Vector3f(20, 0, 0), Vector3f(2, 2, 2));
    
    TEST_ASSERT(box1.Intersects(box2), "Overlapping boxes should intersect");
    TEST_ASSERT(!box1.Intersects(box3), "Separated boxes should not intersect");
    TEST_ASSERT(box1.Intersects(box1), "Box should intersect itself");
    return true;
}

bool test_Box_ContainsBox() {
    Box box1(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    Box box2(Vector3f(0, 0, 0), Vector3f(3, 3, 3));
    Box box3(Vector3f(0, 0, 0), Vector3f(15, 15, 15));
    
    TEST_ASSERT(box1.Contains(box2), "Larger box should contain smaller box");
    TEST_ASSERT(!box1.Contains(box3), "Smaller box should not contain larger box");
    TEST_ASSERT(box1.Contains(box1), "Box should contain itself");
    return true;
}

bool test_Box_GetIntersection() {
    Box box1(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    Box box2(Vector3f(3, 3, 3), Vector3f(5, 5, 5));
    Box intersection;
    
    bool hasIntersection = box1.GetIntersection(box2, intersection);
    TEST_ASSERT(hasIntersection, "Should have intersection");
    TEST_ASSERT(vec3Equal(intersection.GetMin(), Vector3f(-2, -2, -2)), "Intersection min wrong");
    TEST_ASSERT(vec3Equal(intersection.GetMax(), Vector3f(5, 5, 5)), "Intersection max wrong");
    return true;
}

bool test_Box_Merge() {
    Box box1(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    Box box2(Vector3f(10, 10, 10), Vector3f(5, 5, 5));
    
    box1.Merge(box2);
    TEST_ASSERT(vec3Equal(box1.GetMin(), Vector3f(-5, -5, -5)), "Merged box min wrong");
    TEST_ASSERT(vec3Equal(box1.GetMax(), Vector3f(15, 15, 15)), "Merged box max wrong");
    return true;
}

bool test_Box_Distance() {
    Box box1(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    Box box2(Vector3f(10, 0, 0), Vector3f(2, 2, 2));
    Box box3(Vector3f(3, 0, 0), Vector3f(1, 1, 1));
    
    float dist1 = box1.Distance(box2);
    TEST_ASSERT(floatEqual(dist1, 3.0f), "Distance between separated boxes should be 3");
    
    float dist2 = box1.Distance(box3);
    TEST_ASSERT(floatEqual(dist2, 0.0f), "Distance between overlapping boxes should be 0");
    return true;
}

// ============================================================================
// Box Tests - 几何体碰撞检测
// ============================================================================

bool test_Box_IntersectsSphere() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    Sphere sphere1;
    sphere1.Set(Vector3f(0, 0, 0), 3.0f);
    Sphere sphere2;
    sphere2.Set(Vector3f(20, 0, 0), 5.0f);
    
    TEST_ASSERT(box.IntersectsSphere(sphere1), "Box should intersect sphere at center");
    TEST_ASSERT(!box.IntersectsSphere(sphere2), "Box should not intersect distant sphere");
    
    TEST_ASSERT(box.IntersectsSphere(Vector3f(0, 0, 0), 3.0f), "Box should intersect sphere (vector form)");
    return true;
}

bool test_Box_IntersectsRay() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    Ray ray1(Vector3f(-10, 0, 0), Vector3f(1, 0, 0));
    float distance1;
    TEST_ASSERT(box.IntersectsRay(ray1, distance1), "Ray from left should intersect box");
    TEST_ASSERT(floatEqual(distance1, 5.0f), "Ray intersection distance should be 5");
    
    Ray ray2(Vector3f(-10, 20, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(!box.IntersectsRay(ray2), "Ray above box should not intersect");
    
    Ray ray3(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(box.IntersectsRay(ray3), "Ray from center should intersect");
    return true;
}

bool test_Box_IntersectsPlane() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    Plane plane1;
    plane1.Set(Vector3f(0, 1, 0), 0.0f);
    TEST_ASSERT(box.IntersectsPlane(plane1), "Box should intersect plane through center");
    
    Plane plane2;
    plane2.Set(Vector3f(0, 1, 0), -20.0f);
    TEST_ASSERT(!box.IntersectsPlane(plane2), "Box should not intersect distant plane");
    return true;
}

bool test_Box_ClassifyPlane() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    Plane plane1;
    plane1.Set(Vector3f(0, 1, 0), 0.0f);
    TEST_ASSERT(box.ClassifyPlane(plane1) == 0, "Box should be classified as intersecting center plane");
    
    Plane plane2;
    plane2.Set(Vector3f(0, 1, 0), -20.0f);
    TEST_ASSERT(box.ClassifyPlane(plane2) == 1, "Box should be in front of plane below");
    
    Plane plane3;
    plane3.Set(Vector3f(0, 1, 0), 20.0f);
    TEST_ASSERT(box.ClassifyPlane(plane3) == -1, "Box should be behind plane above");
    return true;
}

bool test_Box_IntersectsTriangle() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    Triangle3f tri1(Vector3f(-1, -1, 0), Vector3f(1, -1, 0), Vector3f(0, 1, 0));
    TEST_ASSERT(box.IntersectsTriangle(tri1), "Box should intersect triangle through center");
    
    Triangle3f tri2(Vector3f(20, 20, 20), Vector3f(21, 20, 20), Vector3f(20, 21, 20));
    TEST_ASSERT(!box.IntersectsTriangle(tri2), "Box should not intersect distant triangle");
    return true;
}

bool test_Box_IntersectsAABB() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    AABB aabb1;
    aabb1.SetMinMax(Vector3f(-3, -3, -3), Vector3f(3, 3, 3));
    TEST_ASSERT(box.IntersectsAABB(aabb1), "Box should intersect overlapping AABB");
    
    AABB aabb2;
    aabb2.SetMinMax(Vector3f(20, 20, 20), Vector3f(25, 25, 25));
    TEST_ASSERT(!box.IntersectsAABB(aabb2), "Box should not intersect distant AABB");
    return true;
}

// ============================================================================
// Box Tests - 变换操作
// ============================================================================

bool test_Box_ExpandToInclude() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    box.ExpandToInclude(Vector3f(10, 10, 10));
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(10, 10, 10)), "Box should expand to include point");
    
    box.ExpandToInclude(Vector3f(-8, -8, -8));
    TEST_ASSERT(vec3Equal(box.GetMin(), Vector3f(-8, -8, -8)), "Box should expand to include negative point");
    return true;
}

bool test_Box_Expand() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    box.Expand(2.0f);
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(7, 7, 7)), "Box extent should expand by 2");
    
    box.Expand(Vector3f(1, 2, 3));
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(8, 9, 10)), "Box extent should expand by vector");
    return true;
}

bool test_Box_Scale() {
    Box box(Vector3f(5, 5, 5), Vector3f(2, 3, 4));
    
    box.Scale(2.0f);
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(5, 5, 5)), "Box center should not change on scale");
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(4, 6, 8)), "Box extent should double");
    return true;
}

bool test_Box_Translate() {
    Box box(Vector3f(0, 0, 0), Vector3f(5, 5, 5));
    
    box.Translate(Vector3f(10, 20, 30));
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(10, 20, 30)), "Box center should translate");
    TEST_ASSERT(vec3Equal(box.GetExtent(), Vector3f(5, 5, 5)), "Box extent should not change on translate");
    return true;
}

bool test_Box_Transformed() {
    Box box(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    
    Matrix4f transform = glm::translate(Matrix4f(1.0f), Vector3f(5, 5, 5));
    Box transformed = box.Transformed(transform);
    
    TEST_ASSERT(vec3Equal(transformed.GetCenter(), Vector3f(5, 5, 5)), "Transformed box center wrong");
    return true;
}

// ============================================================================
// Box Tests - 属性与工具
// ============================================================================

bool test_Box_VolumeAndArea() {
    Box box(Vector3f(0, 0, 0), Vector3f(2, 3, 4));
    
    float volume = box.GetVolume();
    TEST_ASSERT(floatEqual(volume, 192.0f), "Box volume should be 2*2*3*4*8 = 192");
    
    float area = box.GetSurfaceArea();
    TEST_ASSERT(floatEqual(area, 104.0f), "Box surface area should be 2*(4*6 + 6*8 + 8*4) = 104");
    return true;
}

bool test_Box_IsEmpty() {
    Box box1(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
    TEST_ASSERT(box1.IsEmpty(), "Zero extent box should be empty");
    
    Box box2(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    TEST_ASSERT(!box2.IsEmpty(), "Non-zero extent box should not be empty");
    
    box2.Clear();
    TEST_ASSERT(box2.IsEmpty(), "Cleared box should be empty");
    return true;
}

bool test_Box_Operators() {
    Box box1(Vector3f(5, 5, 5), Vector3f(2, 2, 2));
    Box box2(Vector3f(5, 5, 5), Vector3f(2, 2, 2));
    Box box3(Vector3f(10, 10, 10), Vector3f(3, 3, 3));
    
    TEST_ASSERT(box1 == box2, "Equal boxes should compare equal");
    TEST_ASSERT(box1 != box3, "Different boxes should not be equal");
    
    Box box4 = box1 + Vector3f(5, 5, 5);
    TEST_ASSERT(vec3Equal(box4.GetCenter(), Vector3f(10, 10, 10)), "Box + vector should translate");
    
    Box box5 = box1 * 2.0f;
    TEST_ASSERT(vec3Equal(box5.GetExtent(), Vector3f(4, 4, 4)), "Box * scalar should scale extent");
    return true;
}

bool test_Box_ConversionFunctions() {
    AABB aabb;
    aabb.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    Box box1 = ToBox(aabb);
    TEST_ASSERT(vec3Equal(box1.GetCenter(), Vector3f(0, 0, 0)), "ToBox(AABB) center wrong");
    TEST_ASSERT(vec3Equal(box1.GetExtent(), Vector3f(5, 5, 5)), "ToBox(AABB) extent wrong");
    
    return true;
}

// ============================================================================
// 边界情况测试
// ============================================================================

bool test_Box_EdgeCase_ZeroExtent() {
    Box box(Vector3f(5, 5, 5), Vector3f(0, 0, 0));
    
    TEST_ASSERT(box.IsEmpty(), "Zero extent box should be empty");
    TEST_ASSERT(floatEqual(box.GetVolume(), 0.0f), "Zero extent box volume should be 0");
    TEST_ASSERT(box.ContainsPoint(Vector3f(5, 5, 5)), "Zero extent box should contain its center");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(5.001f, 5, 5)), "Zero extent box should not contain nearby points");
    return true;
}

bool test_Box_EdgeCase_NegativeExtent() {
    Box box(Vector3f(0, 0, 0), Vector3f(-1, -2, -3));
    
    // 负extent应该被正确处理（取绝对值或clamp到0）
    Vector3f min_pt = box.GetMin();
    Vector3f max_pt = box.GetMax();
    
    TEST_ASSERT(min_pt.x <= max_pt.x, "Min should be <= Max for X");
    TEST_ASSERT(min_pt.y <= max_pt.y, "Min should be <= Max for Y");
    TEST_ASSERT(min_pt.z <= max_pt.z, "Min should be <= Max for Z");
    return true;
}

bool test_Box_EdgeCase_LargeValues() {
    Box box(Vector3f(1000000, 2000000, 3000000), Vector3f(500000, 600000, 700000));
    
    TEST_ASSERT(box.ContainsPoint(Vector3f(1000000, 2000000, 3000000)), "Large box should contain its center");
    TEST_ASSERT(box.ContainsPoint(box.GetMin()), "Large box should contain min corner");
    TEST_ASSERT(box.ContainsPoint(box.GetMax()), "Large box should contain max corner");
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    int total = 0;
    int passed = 0;
    int failed = 0;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Box Class Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n=== 基础功能测试 ===" << std::endl;
    RUN_TEST(test_Box_Construction);
    RUN_TEST(test_Box_FromAABB);
    RUN_TEST(test_Box_GetMinMax);
    RUN_TEST(test_Box_SetMinMax);
    RUN_TEST(test_Box_GetCorners);
    
    std::cout << "\n=== 点操作测试 ===" << std::endl;
    RUN_TEST(test_Box_ContainsPoint);
    RUN_TEST(test_Box_ClosestPoint);
    RUN_TEST(test_Box_DistanceToPoint);
    
    std::cout << "\n=== Box间操作测试 ===" << std::endl;
    RUN_TEST(test_Box_IntersectsBox);
    RUN_TEST(test_Box_ContainsBox);
    RUN_TEST(test_Box_GetIntersection);
    RUN_TEST(test_Box_Merge);
    RUN_TEST(test_Box_Distance);
    
    std::cout << "\n=== 几何体碰撞检测测试 ===" << std::endl;
    RUN_TEST(test_Box_IntersectsSphere);
    RUN_TEST(test_Box_IntersectsRay);
    RUN_TEST(test_Box_IntersectsPlane);
    RUN_TEST(test_Box_ClassifyPlane);
    RUN_TEST(test_Box_IntersectsTriangle);
    RUN_TEST(test_Box_IntersectsAABB);
    
    std::cout << "\n=== 变换操作测试 ===" << std::endl;
    RUN_TEST(test_Box_ExpandToInclude);
    RUN_TEST(test_Box_Expand);
    RUN_TEST(test_Box_Scale);
    RUN_TEST(test_Box_Translate);
    RUN_TEST(test_Box_Transformed);
    
    std::cout << "\n=== 属性与工具测试 ===" << std::endl;
    RUN_TEST(test_Box_VolumeAndArea);
    RUN_TEST(test_Box_IsEmpty);
    RUN_TEST(test_Box_Operators);
    RUN_TEST(test_Box_ConversionFunctions);
    
    std::cout << "\n=== 边界情况测试 ===" << std::endl;
    RUN_TEST(test_Box_EdgeCase_ZeroExtent);
    RUN_TEST(test_Box_EdgeCase_NegativeExtent);
    RUN_TEST(test_Box_EdgeCase_LargeValues);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试结果" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "总计:  " << total << std::endl;
    std::cout << "通过: " << passed << " (" << (total > 0 ? (passed * 100 / total) : 0) << "%)" << std::endl;
    std::cout << "失败: " << failed << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
