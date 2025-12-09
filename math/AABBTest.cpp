#include<hgl/math/geometry/AABB.h>
#include<hgl/math/geometry/OBB.h>
#include<hgl/math/geometry/Ray.h>
#include<hgl/math/geometry/Plane.h>
#include<hgl/math/geometry/Triangle.h>
#include<glm/glm.hpp>
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
// AABB 基础功能测试
// ============================================================================

bool test_AABB_Construction() {
    AABB box;
    TEST_ASSERT(vec3Equal(box.GetMin(), Vector3f(0,0,0)), "Default AABB min should be (0,0,0)");
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(1,1,1)), "Default AABB max should be (1,1,1)");
    return true;
}

bool test_AABB_SetMinMax() {
    AABB box;
    Vector3f min(-1, -2, -3);
    Vector3f max(4, 5, 6);
    box.SetMinMax(min, max);
    
    TEST_ASSERT(vec3Equal(box.GetMin(), min), "AABB min not set correctly");
    TEST_ASSERT(vec3Equal(box.GetMax(), max), "AABB max not set correctly");
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(1.5f, 1.5f, 1.5f)), "AABB center calculation wrong");
    TEST_ASSERT(vec3Equal(box.GetLength(), Vector3f(5, 7, 9)), "AABB length calculation wrong");
    return true;
}

bool test_AABB_SetCornerLength() {
    AABB box;
    Vector3f corner(1, 2, 3);
    Vector3f length(4, 5, 6);
    box.SetCornerLength(corner, length);
    
    TEST_ASSERT(vec3Equal(box.GetMin(), corner), "AABB corner not set correctly");
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(5, 7, 9)), "AABB max from corner+length wrong");
    return true;
}

bool test_AABB_SetFromPoints() {
    float points[] = {
        -1.0f, -2.0f, -3.0f,
        4.0f, 5.0f, 6.0f,
        2.0f, 1.0f, 0.0f,
        -2.0f, 3.0f, 4.0f
    };
    
    AABB box;
    box.SetFromPoints(points, 4, 3);
    
    TEST_ASSERT(vec3Equal(box.GetMin(), Vector3f(-2, -2, -3)), "AABB SetFromPoints min wrong");
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(4, 5, 6)), "AABB SetFromPoints max wrong");
    return true;
}

bool test_AABB_IsEmpty() {
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(0, 0, 0));
    TEST_ASSERT(box.IsEmpty(), "Zero-size AABB should be empty");
    
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    TEST_ASSERT(!box.IsEmpty(), "Non-zero AABB should not be empty");
    return true;
}

// ============================================================================
// AABB 点操作测试
// ============================================================================

bool test_AABB_ContainsPoint() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    TEST_ASSERT(box.ContainsPoint(Vector3f(0, 0, 0)), "AABB should contain center point");
    TEST_ASSERT(box.ContainsPoint(Vector3f(3, 3, 3)), "AABB should contain interior point");
    TEST_ASSERT(box.ContainsPoint(Vector3f(5, 5, 5)), "AABB should contain boundary point");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(6, 0, 0)), "AABB should not contain exterior point");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(0, 6, 0)), "AABB should not contain exterior point Y");
    return true;
}

bool test_AABB_ClosestPoint() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    Vector3f closest1 = box.ClosestPoint(Vector3f(0, 0, 0));
    TEST_ASSERT(vec3Equal(closest1, Vector3f(0, 0, 0)), "Closest point to center should be center");
    
    Vector3f closest2 = box.ClosestPoint(Vector3f(10, 0, 0));
    TEST_ASSERT(vec3Equal(closest2, Vector3f(5, 0, 0)), "Closest point outside should be on boundary");
    
    Vector3f closest3 = box.ClosestPoint(Vector3f(-10, 10, -10));
    TEST_ASSERT(vec3Equal(closest3, Vector3f(-5, 5, -5)), "Closest point outside corner should be corner");
    return true;
}

bool test_AABB_DistanceToPoint() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    float dist1 = box.DistanceToPoint(Vector3f(0, 0, 0));
    TEST_ASSERT(floatEqual(dist1, 0.0f), "Distance from center should be 0");
    
    float dist2 = box.DistanceToPoint(Vector3f(3, 3, 3));
    TEST_ASSERT(floatEqual(dist2, 0.0f), "Distance from interior point should be 0");
    
    float dist3 = box.DistanceToPoint(Vector3f(10, 0, 0));
    TEST_ASSERT(floatEqual(dist3, 5.0f), "Distance from exterior point should be 5");
    return true;
}

// ============================================================================
// AABB vs AABB 测试
// ============================================================================

bool test_AABB_Intersects() {
    AABB box1, box2, box3;
    box1.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    box2.SetMinMax(Vector3f(-3, -3, -3), Vector3f(3, 3, 3));
    box3.SetMinMax(Vector3f(10, 10, 10), Vector3f(15, 15, 15));
    
    TEST_ASSERT(box1.Intersects(box2), "Overlapping AABBs should intersect");
    TEST_ASSERT(!box1.Intersects(box3), "Separated AABBs should not intersect");
    TEST_ASSERT(box1.Intersects(box1), "AABB should intersect itself");
    return true;
}

bool test_AABB_Contains() {
    AABB box1, box2, box3;
    box1.SetMinMax(Vector3f(-10, -10, -10), Vector3f(10, 10, 10));
    box2.SetMinMax(Vector3f(-3, -3, -3), Vector3f(3, 3, 3));
    box3.SetMinMax(Vector3f(-15, -15, -15), Vector3f(15, 15, 15));
    
    TEST_ASSERT(box1.Contains(box2), "Larger AABB should contain smaller AABB");
    TEST_ASSERT(!box1.Contains(box3), "Smaller AABB should not contain larger AABB");
    TEST_ASSERT(box1.Contains(box1), "AABB should contain itself");
    return true;
}

bool test_AABB_GetIntersection() {
    AABB box1, box2, intersection;
    box1.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    box2.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    bool hasIntersection = box1.GetIntersection(box2, intersection);
    TEST_ASSERT(hasIntersection, "Should have intersection");
    TEST_ASSERT(vec3Equal(intersection.GetMin(), Vector3f(0, 0, 0)), "Intersection min wrong");
    TEST_ASSERT(vec3Equal(intersection.GetMax(), Vector3f(5, 5, 5)), "Intersection max wrong");
    return true;
}

bool test_AABB_Merge() {
    AABB box1, box2;
    box1.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    box2.SetMinMax(Vector3f(3, 3, 3), Vector3f(10, 10, 10));
    
    box1.Merge(box2);
    TEST_ASSERT(vec3Equal(box1.GetMin(), Vector3f(-5, -5, -5)), "Merged AABB min wrong");
    TEST_ASSERT(vec3Equal(box1.GetMax(), Vector3f(10, 10, 10)), "Merged AABB max wrong");
    return true;
}

bool test_AABB_Distance() {
    AABB box1, box2, box3;
    box1.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    box2.SetMinMax(Vector3f(10, 0, 0), Vector3f(15, 5, 5));
    box3.SetMinMax(Vector3f(0, 0, 0), Vector3f(3, 3, 3));
    
    float dist1 = box1.Distance(box2);
    TEST_ASSERT(floatEqual(dist1, 5.0f), "Distance between separated AABBs should be 5");
    
    float dist2 = box1.Distance(box3);
    TEST_ASSERT(floatEqual(dist2, 0.0f), "Distance between overlapping AABBs should be 0");
    return true;
}

// ============================================================================
// AABB 与其他几何体测试
// ============================================================================

bool test_AABB_IntersectsSphere() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    TEST_ASSERT(box.IntersectsSphere(Vector3f(0, 0, 0), 3.0f), "AABB should intersect sphere at center");
    TEST_ASSERT(!box.IntersectsSphere(Vector3f(20, 0, 0), 5.0f), "AABB should not intersect distant sphere");
    TEST_ASSERT(box.IntersectsSphere(Vector3f(7, 0, 0), 3.0f), "AABB should intersect sphere touching edge");
    return true;
}

bool test_AABB_IntersectsRay() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    Ray ray1(Vector3f(-10, 0, 0), Vector3f(1, 0, 0));
    float distance1;
    TEST_ASSERT(box.IntersectsRay(ray1, distance1), "Ray from left should intersect AABB");
    TEST_ASSERT(floatEqual(distance1, 5.0f), "Ray intersection distance should be 5");
    
    Ray ray2(Vector3f(-10, 20, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(!box.IntersectsRay(ray2), "Ray above AABB should not intersect");
    
    Ray ray3(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(box.IntersectsRay(ray3), "Ray from center should intersect");
    return true;
}

bool test_AABB_IntersectsPlane() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    Plane plane1;
    plane1.Set(Vector3f(0, 1, 0), 0.0f);
    TEST_ASSERT(box.IntersectsPlane(plane1), "AABB should intersect plane through center");
    
    Plane plane2;
    plane2.Set(Vector3f(0, 1, 0), -20.0f);
    TEST_ASSERT(!box.IntersectsPlane(plane2), "AABB should not intersect distant plane");
    return true;
}

bool test_AABB_ClassifyPlane() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    Plane plane1;
    plane1.Set(Vector3f(0, 1, 0), 0.0f);
    TEST_ASSERT(box.ClassifyPlane(plane1) == 0, "AABB should be classified as intersecting center plane");
    
    Plane plane2;
    plane2.Set(Vector3f(0, 1, 0), -20.0f);
    TEST_ASSERT(box.ClassifyPlane(plane2) == 1, "AABB should be in front of plane below");
    
    Plane plane3;
    plane3.Set(Vector3f(0, 1, 0), 20.0f);
    TEST_ASSERT(box.ClassifyPlane(plane3) == -1, "AABB should be behind plane above");
    return true;
}

// ============================================================================
// AABB 工具函数测试
// ============================================================================

bool test_AABB_ExpandToInclude() {
    AABB box;
    box.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    box.ExpandToInclude(Vector3f(10, 10, 10));
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(10, 10, 10)), "AABB should expand to include point");
    
    box.ExpandToInclude(Vector3f(-8, -8, -8));
    TEST_ASSERT(vec3Equal(box.GetMin(), Vector3f(-8, -8, -8)), "AABB should expand to include negative point");
    return true;
}

bool test_AABB_VolumeAndArea() {
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(4, 6, 8));
    
    float volume = box.GetVolume();
    TEST_ASSERT(floatEqual(volume, 192.0f), "AABB volume should be 4*6*8 = 192");
    
    float area = box.GetSurfaceArea();
    TEST_ASSERT(floatEqual(area, 208.0f), "AABB surface area should be 2*(4*6 + 6*8 + 8*4) = 208");
    return true;
}

bool test_AABB_Transformed() {
    AABB box;
    box.SetMinMap(Vector3f(-1, -1, -1), Vector3f(1, 1, 1));
    
    Matrix4f transform = glm::translate(Matrix4f(1.0f), Vector3f(5, 5, 5));
    AABB transformed = box.Transformed(transform);
    
    TEST_ASSERT(vec3Equal(transformed.GetCenter(), Vector3f(5, 5, 5)), "Transformed AABB center wrong");
    return true;
}

// ============================================================================
// 边界情况测试
// ============================================================================

bool test_AABB_EdgeCase_ZeroSize() {
    AABB box;
    box.SetMinMax(Vector3f(5, 5, 5), Vector3f(5, 5, 5));
    
    TEST_ASSERT(box.IsEmpty(), "Zero-size AABB should be empty");
    TEST_ASSERT(floatEqual(box.GetVolume(), 0.0f), "Zero-size AABB volume should be 0");
    TEST_ASSERT(box.ContainsPoint(Vector3f(5, 5, 5)), "Zero-size AABB should contain its point");
    return true;
}

bool test_AABB_EdgeCase_LargeValues() {
    AABB box;
    box.SetMinMax(Vector3f(-1000000, -1000000, -1000000), Vector3f(1000000, 1000000, 1000000));
    
    TEST_ASSERT(box.ContainsPoint(Vector3f(0, 0, 0)), "Large AABB should contain origin");
    TEST_ASSERT(box.ContainsPoint(Vector3f(999999, 999999, 999999)), "Large AABB should contain large point");
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
    std::cout << "  AABB Class Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n=== 基础功能测试 ===" << std::endl;
    RUN_TEST(test_AABB_Construction);
    RUN_TEST(test_AABB_SetMinMax);
    RUN_TEST(test_AABB_SetCornerLength);
    RUN_TEST(test_AABB_SetFromPoints);
    RUN_TEST(test_AABB_IsEmpty);
    
    std::cout << "\n=== 点操作测试 ===" << std::endl;
    RUN_TEST(test_AABB_ContainsPoint);
    RUN_TEST(test_AABB_ClosestPoint);
    RUN_TEST(test_AABB_DistanceToPoint);
    
    std::cout << "\n=== AABB间操作测试 ===" << std::endl;
    RUN_TEST(test_AABB_Intersects);
    RUN_TEST(test_AABB_Contains);
    RUN_TEST(test_AABB_GetIntersection);
    RUN_TEST(test_AABB_Merge);
    RUN_TEST(test_AABB_Distance);
    
    std::cout << "\n=== 几何体碰撞检测测试 ===" << std::endl;
    RUN_TEST(test_AABB_IntersectsSphere);
    RUN_TEST(test_AABB_IntersectsRay);
    RUN_TEST(test_AABB_IntersectsPlane);
    RUN_TEST(test_AABB_ClassifyPlane);
    
    std::cout << "\n=== 工具函数测试 ===" << std::endl;
    RUN_TEST(test_AABB_ExpandToInclude);
    RUN_TEST(test_AABB_VolumeAndArea);
    RUN_TEST(test_AABB_Transformed);
    
    std::cout << "\n=== 边界情况测试 ===" << std::endl;
    RUN_TEST(test_AABB_EdgeCase_ZeroSize);
    RUN_TEST(test_AABB_EdgeCase_LargeValues);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试结果" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "总计:  " << total << std::endl;
    std::cout << "通过: " << passed << " (" << (total > 0 ? (passed * 100 / total) : 0) << "%)" << std::endl;
    std::cout << "失败: " << failed << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
