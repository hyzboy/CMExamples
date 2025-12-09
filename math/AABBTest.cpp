#include<hgl/math/geometry/AABB.h>
#include<hgl/math/geometry/OBB.h>
#include<hgl/math/geometry/Ray.h>
#include<hgl/math/geometry/Triangle.h>
#include<hgl/math/geometry/Plane.h>
#include<hgl/math/MathConstants.h>
#include<hgl/math/VectorOperations.h>
#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
#include<iostream>
#include<cassert>
#include<cmath>
#include<vector>

using namespace hgl::math;

// ============================================================================
// 测试辅助宏和函数
// ============================================================================

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "  FAILED: " << message << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        std::cout << "\nRunning " << #test_func << "..." << std::endl; \
        if (test_func()) { \
            std::cout << "  ✓ PASSED" << std::endl; \
            passed++; \
        } else { \
            std::cout << "  ✗ FAILED" << std::endl; \
            failed++; \
        } \
        total++; \
    } while(0)

void printVector3f(const char* name, const Vector3f& v) {
    std::cout << "    " << name << ": (" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void printAABB(const char* name, const AABB& box) {
    std::cout << "    " << name << ":" << std::endl;
    printVector3f("  Min", box.GetMin());
    printVector3f("  Max", box.GetMax());
    printVector3f("  Center", box.GetCenter());
    printVector3f("  Length", box.GetLength());
}

// ============================================================================
// 基本构造和设置测试
// ============================================================================

bool test_AABB_DefaultConstruction() {
    std::cout << "  测试默认构造函数..." << std::endl;
    
    AABB box;
    TEST_ASSERT(IsNearlyEqual(box.GetMin(), Vector3f(0,0,0)), "默认AABB最小点应为(0,0,0)");
    TEST_ASSERT(IsNearlyEqual(box.GetMax(), Vector3f(1,1,1)), "默认AABB最大点应为(1,1,1)");
    TEST_ASSERT(IsNearlyEqual(box.GetCenter(), Vector3f(0.5f,0.5f,0.5f)), "默认AABB中心应为(0.5,0.5,0.5)");
    TEST_ASSERT(IsNearlyEqual(box.GetLength(), Vector3f(1,1,1)), "默认AABB长度应为(1,1,1)");
    
    return true;
}

bool test_AABB_SetMinMax() {
    std::cout << "  测试SetMinMax方法..." << std::endl;
    
    AABB box;
    Vector3f min(-1, -2, -3);
    Vector3f max(4, 5, 6);
    box.SetMinMax(min, max);
    
    TEST_ASSERT(IsNearlyEqual(box.GetMin(), min), "AABB最小点设置错误");
    TEST_ASSERT(IsNearlyEqual(box.GetMax(), max), "AABB最大点设置错误");
    TEST_ASSERT(IsNearlyEqual(box.GetCenter(), Vector3f(1.5f, 1.5f, 1.5f)), "AABB中心计算错误");
    TEST_ASSERT(IsNearlyEqual(box.GetLength(), Vector3f(5, 7, 9)), "AABB长度计算错误");
    
    return true;
}

bool test_AABB_SetCornerLength() {
    std::cout << "  测试SetCornerLength方法..." << std::endl;
    
    AABB box;
    Vector3f corner(1, 2, 3);
    Vector3f length(4, 5, 6);
    box.SetCornerLength(corner, length);
    
    TEST_ASSERT(IsNearlyEqual(box.GetMin(), corner), "AABB顶角设置错误");
    TEST_ASSERT(IsNearlyEqual(box.GetMax(), Vector3f(5, 7, 9)), "AABB从顶角+长度计算最大点错误");
    TEST_ASSERT(IsNearlyEqual(box.GetLength(), length), "AABB长度设置错误");
    
    return true;
}

bool test_AABB_SetFromPoints() {
    std::cout << "  测试SetFromPoints方法..." << std::endl;
    
    float points[] = {
        -1.0f, -2.0f, -3.0f,
        4.0f, 5.0f, 6.0f,
        2.0f, 1.0f, 0.0f,
        -2.0f, 3.0f, 4.0f
    };
    
    AABB box;
    box.SetFromPoints(points, 4, 3);
    
    TEST_ASSERT(IsNearlyEqual(box.GetMin(), Vector3f(-2, -2, -3)), "从点集计算的最小点错误");
    TEST_ASSERT(IsNearlyEqual(box.GetMax(), Vector3f(4, 5, 6)), "从点集计算的最大点错误");
    
    return true;
}

bool test_AABB_Clear() {
    std::cout << "  测试Clear方法..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(-10, -10, -10), Vector3f(10, 10, 10));
    box.Clear();
    
    TEST_ASSERT(IsNearlyEqual(box.GetMin(), Vector3f(0,0,0)), "Clear后最小点应为零向量");
    TEST_ASSERT(IsNearlyEqual(box.GetMax(), Vector3f(0,0,0)), "Clear后最大点应为零向量");
    TEST_ASSERT(box.IsEmpty(), "Clear后AABB应为空");
    
    return true;
}

bool test_AABB_IsEmpty() {
    std::cout << "  测试IsEmpty方法..." << std::endl;
    
    AABB empty_box;
    empty_box.Clear();
    TEST_ASSERT(empty_box.IsEmpty(), "空AABB应返回true");
    
    AABB non_empty_box;
    non_empty_box.SetMinMax(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    TEST_ASSERT(!non_empty_box.IsEmpty(), "非空AABB应返回false");
    
    return true;
}

// ============================================================================
// 点相关碰撞检测测试
// ============================================================================

bool test_AABB_ContainsPoint() {
    std::cout << "  测试ContainsPoint方法..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    TEST_ASSERT(box.ContainsPoint(Vector3f(5, 5, 5)), "应包含中心点");
    TEST_ASSERT(box.ContainsPoint(Vector3f(0, 0, 0)), "应包含最小点");
    TEST_ASSERT(box.ContainsPoint(Vector3f(10, 10, 10)), "应包含最大点");
    TEST_ASSERT(box.ContainsPoint(Vector3f(0, 5, 10)), "应包含边界点");
    
    TEST_ASSERT(!box.ContainsPoint(Vector3f(-1, 5, 5)), "不应包含外部点(-1,5,5)");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(11, 5, 5)), "不应包含外部点(11,5,5)");
    TEST_ASSERT(!box.ContainsPoint(Vector3f(5, 5, 11)), "不应包含外部点(5,5,11)");
    
    return true;
}

bool test_AABB_ClosestPoint() {
    std::cout << "  测试ClosestPoint方法..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 内部点应返回自身
    Vector3f inside_point(5, 5, 5);
    TEST_ASSERT(IsNearlyEqual(box.ClosestPoint(inside_point), inside_point), 
                "内部点的最近点应为自身");
    
    // 外部点应钳制到边界
    TEST_ASSERT(IsNearlyEqual(box.ClosestPoint(Vector3f(-5, 5, 5)), Vector3f(0, 5, 5)), 
                "外部点(-5,5,5)的最近点应为(0,5,5)");
    TEST_ASSERT(IsNearlyEqual(box.ClosestPoint(Vector3f(15, 5, 5)), Vector3f(10, 5, 5)), 
                "外部点(15,5,5)的最近点应为(10,5,5)");
    TEST_ASSERT(IsNearlyEqual(box.ClosestPoint(Vector3f(5, -3, 12)), Vector3f(5, 0, 10)), 
                "外部点(5,-3,12)的最近点应为(5,0,10)");
    
    return true;
}

bool test_AABB_DistanceToPoint() {
    std::cout << "  测试DistanceToPoint方法..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 内部点距离应为0
    TEST_ASSERT(IsNearlyEqual(box.DistanceToPoint(Vector3f(5, 5, 5)), 0.0f), 
                "内部点到AABB的距离应为0");
    
    // 表面点距离应为0
    TEST_ASSERT(IsNearlyEqual(box.DistanceToPoint(Vector3f(0, 5, 5)), 0.0f), 
                "表面点到AABB的距离应为0");
    
    // 外部点距离测试
    TEST_ASSERT(IsNearlyEqual(box.DistanceToPoint(Vector3f(-5, 5, 5)), 5.0f), 
                "点(-5,5,5)到AABB的距离应为5");
    TEST_ASSERT(IsNearlyEqual(box.DistanceToPoint(Vector3f(15, 5, 5)), 5.0f), 
                "点(15,5,5)到AABB的距离应为5");
    
    return true;
}

bool test_AABB_DistanceToPointSquared() {
    std::cout << "  测试DistanceToPointSquared方法..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    TEST_ASSERT(IsNearlyEqual(box.DistanceToPointSquared(Vector3f(5, 5, 5)), 0.0f), 
                "内部点到AABB的距离平方应为0");
    TEST_ASSERT(IsNearlyEqual(box.DistanceToPointSquared(Vector3f(-5, 5, 5)), 25.0f), 
                "点(-5,5,5)到AABB的距离平方应为25");
    
    return true;
}

bool test_AABB_ExpandToInclude() {
    std::cout << "  测试ExpandToInclude方法..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 扩展以包含外部点
    box.ExpandToInclude(Vector3f(15, 5, 5));
    TEST_ASSERT(IsNearlyEqual(box.GetMax(), Vector3f(15, 10, 10)), 
                "扩展后应包含点(15,5,5)");
    
    box.ExpandToInclude(Vector3f(-5, -5, -5));
    TEST_ASSERT(IsNearlyEqual(box.GetMin(), Vector3f(-5, -5, -5)), 
                "扩展后应包含点(-5,-5,-5)");
    
    // 包含内部点不应改变AABB
    AABB box2;
    box2.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    box2.ExpandToInclude(Vector3f(5, 5, 5));
    TEST_ASSERT(IsNearlyEqual(box2.GetMin(), Vector3f(0, 0, 0)) && 
                IsNearlyEqual(box2.GetMax(), Vector3f(10, 10, 10)), 
                "包含内部点不应改变AABB");
    
    return true;
}

// ============================================================================
// AABB间碰撞检测测试
// ============================================================================

bool test_AABB_Intersects() {
    std::cout << "  测试AABB与AABB相交检测..." << std::endl;
    
    AABB box1;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 相交的情况
    AABB box2;
    box2.SetMinMax(Vector3f(5, 5, 5), Vector3f(15, 15, 15));
    TEST_ASSERT(box1.Intersects(box2), "部分重叠的AABB应相交");
    
    // 完全包含
    AABB box3;
    box3.SetMinMax(Vector3f(2, 2, 2), Vector3f(8, 8, 8));
    TEST_ASSERT(box1.Intersects(box3), "完全包含的AABB应相交");
    
    // 不相交
    AABB box4;
    box4.SetMinMax(Vector3f(20, 20, 20), Vector3f(30, 30, 30));
    TEST_ASSERT(!box1.Intersects(box4), "不重叠的AABB不应相交");
    
    // 边界接触（遵循Unreal Engine惯例，边界接触不算相交）
    AABB box5;
    box5.SetMinMax(Vector3f(10, 0, 0), Vector3f(20, 10, 10));
    TEST_ASSERT(!box1.Intersects(box5), "边界接触的AABB不应相交");
    
    // 完全分离（有间隙）
    AABB box6;
    box6.SetMinMax(Vector3f(11, 0, 0), Vector3f(20, 10, 10));
    TEST_ASSERT(!box1.Intersects(box6), "有间隙的AABB不应相交");
    
    return true;
}

bool test_AABB_Contains() {
    std::cout << "  测试AABB包含另一个AABB..." << std::endl;
    
    AABB box1;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 完全包含
    AABB box2;
    box2.SetMinMax(Vector3f(2, 2, 2), Vector3f(8, 8, 8));
    TEST_ASSERT(box1.Contains(box2), "应完全包含较小的AABB");
    
    // 部分重叠
    AABB box3;
    box3.SetMinMax(Vector3f(5, 5, 5), Vector3f(15, 15, 15));
    TEST_ASSERT(!box1.Contains(box3), "部分重叠不算完全包含");
    
    // 完全不包含
    AABB box4;
    box4.SetMinMax(Vector3f(20, 20, 20), Vector3f(30, 30, 30));
    TEST_ASSERT(!box1.Contains(box4), "分离的AABB不应包含");
    
    // 自身包含
    TEST_ASSERT(box1.Contains(box1), "AABB应包含自身");
    
    return true;
}

bool test_AABB_GetIntersection() {
    std::cout << "  测试AABB交集计算..." << std::endl;
    
    AABB box1;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    AABB box2;
    box2.SetMinMax(Vector3f(5, 5, 5), Vector3f(15, 15, 15));
    
    AABB intersection;
    bool result = box1.GetIntersection(box2, intersection);
    
    TEST_ASSERT(result, "应能计算出交集");
    TEST_ASSERT(IsNearlyEqual(intersection.GetMin(), Vector3f(5, 5, 5)), 
                "交集最小点应为(5,5,5)");
    TEST_ASSERT(IsNearlyEqual(intersection.GetMax(), Vector3f(10, 10, 10)), 
                "交集最大点应为(10,10,10)");
    
    // 不相交的情况
    AABB box3;
    box3.SetMinMax(Vector3f(20, 20, 20), Vector3f(30, 30, 30));
    AABB no_intersection;
    result = box1.GetIntersection(box3, no_intersection);
    TEST_ASSERT(!result, "不相交的AABB应返回false");
    
    return true;
}

bool test_AABB_Distance() {
    std::cout << "  测试AABB间距离计算..." << std::endl;
    
    AABB box1;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 相交的AABB距离应为0
    AABB box2;
    box2.SetMinMax(Vector3f(5, 5, 5), Vector3f(15, 15, 15));
    TEST_ASSERT(IsNearlyEqual(box1.Distance(box2), 0.0f), 
                "相交的AABB距离应为0");
    
    // 分离的AABB
    AABB box3;
    box3.SetMinMax(Vector3f(20, 0, 0), Vector3f(30, 10, 10));
    TEST_ASSERT(IsNearlyEqual(box1.Distance(box3), 10.0f), 
                "X轴分离10单位的AABB距离应为10");
    
    return true;
}

bool test_AABB_Merge() {
    std::cout << "  测试AABB合并..." << std::endl;
    
    AABB box1;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    AABB box2;
    box2.SetMinMax(Vector3f(5, 5, 5), Vector3f(20, 20, 20));
    
    box1.Merge(box2);
    
    TEST_ASSERT(IsNearlyEqual(box1.GetMin(), Vector3f(0, 0, 0)), 
                "合并后最小点应为(0,0,0)");
    TEST_ASSERT(IsNearlyEqual(box1.GetMax(), Vector3f(20, 20, 20)), 
                "合并后最大点应为(20,20,20)");
    
    return true;
}

bool test_AABB_OperatorPlusEqual() {
    std::cout << "  测试+=运算符..." << std::endl;
    
    AABB box1;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    AABB box2;
    box2.SetMinMax(Vector3f(-5, -5, -5), Vector3f(5, 5, 5));
    
    box1 += box2;
    
    TEST_ASSERT(IsNearlyEqual(box1.GetMin(), Vector3f(-5, -5, -5)), 
                "+=后最小点应为(-5,-5,-5)");
    TEST_ASSERT(IsNearlyEqual(box1.GetMax(), Vector3f(10, 10, 10)), 
                "+=后最大点应为(10,10,10)");
    
    return true;
}

// ============================================================================
// 其他几何体碰撞检测测试
// ============================================================================

bool test_AABB_IntersectsSphere() {
    std::cout << "  测试AABB与球体相交..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 球心在AABB内
    TEST_ASSERT(box.IntersectsSphere(Vector3f(5, 5, 5), 1.0f), 
                "球心在AABB内应相交");
    
    // 球体与AABB相交
    TEST_ASSERT(box.IntersectsSphere(Vector3f(-2, 5, 5), 3.0f), 
                "球体部分重叠应相交");
    
    // 球体与AABB不相交
    TEST_ASSERT(!box.IntersectsSphere(Vector3f(20, 20, 20), 5.0f), 
                "远离的球体不应相交");
    
    // 球体刚好接触AABB边界
    TEST_ASSERT(box.IntersectsSphere(Vector3f(-5, 5, 5), 5.0f), 
                "刚好接触边界的球体应相交");
    
    return true;
}

bool test_AABB_IntersectsRay() {
    std::cout << "  测试AABB与射线相交..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 射线穿过AABB
    Ray ray1(Vector3f(-5, 5, 5), Vector3f(1, 0, 0));
    float distance;
    TEST_ASSERT(box.IntersectsRay(ray1, distance), 
                "穿过AABB的射线应相交");
    TEST_ASSERT(IsNearlyEqual(distance, 5.0f, 0.01f), 
                "射线到AABB的距离应为5");
    
    // 射线起点在AABB内
    Ray ray2(Vector3f(5, 5, 5), Vector3f(1, 0, 0));
    TEST_ASSERT(box.IntersectsRay(ray2), 
                "起点在AABB内的射线应相交");
    
    // 射线不与AABB相交
    Ray ray3(Vector3f(-5, 20, 5), Vector3f(1, 0, 0));
    TEST_ASSERT(!box.IntersectsRay(ray3), 
                "不穿过AABB的射线不应相交");
    
    // 射线背向AABB
    Ray ray4(Vector3f(-5, 5, 5), Vector3f(-1, 0, 0));
    TEST_ASSERT(!box.IntersectsRay(ray4), 
                "背向AABB的射线不应相交");
    
    // 测试返回t_min和t_max的版本
    Ray ray5(Vector3f(-5, 5, 5), Vector3f(1, 0, 0));
    float t_min, t_max;
    TEST_ASSERT(box.IntersectsRay(ray5, t_min, t_max), 
                "应返回射线参数范围");
    TEST_ASSERT(t_min >= 0 && t_max > t_min, 
                "射线参数应有效");
    
    return true;
}

bool test_AABB_IntersectsPlane() {
    std::cout << "  测试AABB与平面相交..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    std::cout << "    Box center: (" << box.GetCenter().x << ", " << box.GetCenter().y << ", " << box.GetCenter().z << ")" << std::endl;
    
    // 平面穿过AABB
    Plane plane1;
    plane1.Set(Vector3f(5, 5, 5), Vector3f(1, 0, 0));  // 平面过点(5,5,5)，法线(1,0,0)
    std::cout << "    Plane1: normal=(" << plane1.normal.x << ", " << plane1.normal.y << ", " << plane1.normal.z << "), d=" << plane1.d << std::endl;
    std::cout << "    Distance from box center to plane1: " << plane1.Distance(box.GetCenter()) << std::endl;
    std::cout << "    ClassifyPlane result: " << box.ClassifyPlane(plane1) << std::endl;
    
    TEST_ASSERT(box.IntersectsPlane(plane1), 
                "穿过AABB的平面应相交");
    
    // 平面在AABB一侧
    Plane plane2;
    plane2.Set(Vector3f(20, 0, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(!box.IntersectsPlane(plane2), 
                "在AABB一侧的平面不应相交");
    
    return true;
}

bool test_AABB_ClassifyPlane() {
    std::cout << "  测试AABB平面分类..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    std::cout << "    Box: center=(" << box.GetCenter().x << ", " << box.GetCenter().y << ", " << box.GetCenter().z << "), extent=" << (box.GetLength().x / 2) << std::endl;
    
    // 平面穿过AABB
    Plane plane1;
    plane1.Set(Vector3f(5, 5, 5), Vector3f(1, 0, 0));  // 平面过点(5,5,5)，法线(1,0,0)
    int result1 = box.ClassifyPlane(plane1);
    std::cout << "    Plane1 (穿过): normal=(" << plane1.normal.x << ", " << plane1.normal.y << ", " << plane1.normal.z << "), d=" << plane1.d << ", result=" << result1 << std::endl;
    TEST_ASSERT(result1 == 0, "穿过AABB的平面应返回0");
    
    // AABB在平面前面（法线方向）
    Plane plane2;
    plane2.Set(Vector3f(-10, 0, 0), Vector3f(1, 0, 0));  // 平面过点(-10,0,0)，法线指向+X
    int result2 = box.ClassifyPlane(plane2);
    std::cout << "    Plane2 (AABB在前): normal=(" << plane2.normal.x << ", " << plane2.normal.y << ", " << plane2.normal.z << "), d=" << plane2.d << ", result=" << result2 << std::endl;
    TEST_ASSERT(result2 > 0, "AABB在平面前面应返回正数");
    
    // AABB在平面后面（法线反方向）
    Plane plane3;
    plane3.Set(Vector3f(20, 0, 0), Vector3f(1, 0, 0));  // 平面过点(20,0,0)，法线指向+X
    int result3 = box.ClassifyPlane(plane3);
    std::cout << "    Plane3 (AABB在后): normal=(" << plane3.normal.x << ", " << plane3.normal.y << ", " << plane3.normal.z << "), d=" << plane3.d << ", result=" << result3 << std::endl;
    TEST_ASSERT(result3 < 0, "AABB在平面后面应返回负数");
    
    return true;
}

bool test_AABB_IntersectsTriangle() {
    std::cout << "  测试AABB与三角形相交..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 三角形顶点在AABB内
    Triangle3f tri1(Vector3f(2, 2, 2), Vector3f(4, 2, 2), Vector3f(3, 4, 2));
    TEST_ASSERT(box.IntersectsTriangle(tri1), 
                "顶点在AABB内的三角形应相交");
    
    // 三角形完全在AABB外
    Triangle3f tri2(Vector3f(20, 20, 20), Vector3f(22, 20, 20), Vector3f(21, 22, 20));
    TEST_ASSERT(!box.IntersectsTriangle(tri2), 
                "完全在外的三角形不应相交");
    
    return true;
}

// ============================================================================
// 工具函数测试
// ============================================================================

bool test_AABB_GetSurfaceArea() {
    std::cout << "  测试表面积计算..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    float expected_area = 2.0f * (10*10 + 10*10 + 10*10); // 600
    TEST_ASSERT(IsNearlyEqual(box.GetSurfaceArea(), expected_area), 
                "表面积计算错误");
    
    // 非立方体
    AABB box2;
    box2.SetMinMax(Vector3f(0, 0, 0), Vector3f(2, 3, 4));
    float expected_area2 = 2.0f * (2*3 + 3*4 + 4*2); // 52
    TEST_ASSERT(IsNearlyEqual(box2.GetSurfaceArea(), expected_area2), 
                "非立方体表面积计算错误");
    
    return true;
}

bool test_AABB_GetVolume() {
    std::cout << "  测试体积计算..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    TEST_ASSERT(IsNearlyEqual(box.GetVolume(), 1000.0f), 
                "体积计算错误");
    
    AABB box2;
    box2.SetMinMax(Vector3f(0, 0, 0), Vector3f(2, 3, 4));
    TEST_ASSERT(IsNearlyEqual(box2.GetVolume(), 24.0f), 
                "非立方体体积计算错误");
    
    return true;
}

bool test_AABB_GetVertexP_N() {
    std::cout << "  测试GetVertexP和GetVertexN..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(10, 10, 10));
    
    // 测试GetVertexP (正方向最远点)
    Vector3f normal1(1, 0, 0);
    Vector3f vp = box.GetVertexP(normal1);
    TEST_ASSERT(IsNearlyEqual(vp, Vector3f(10, 0, 0)), 
                "GetVertexP对于(1,0,0)应返回(10,0,0)");
    
    Vector3f normal2(1, 1, 1);
    vp = box.GetVertexP(normal2);
    TEST_ASSERT(IsNearlyEqual(vp, Vector3f(10, 10, 10)), 
                "GetVertexP对于(1,1,1)应返回(10,10,10)");
    
    // 测试GetVertexN (负方向最远点)
    Vector3f vn = box.GetVertexN(normal1);
    TEST_ASSERT(IsNearlyEqual(vn, Vector3f(0, 0, 0)), 
                "GetVertexN对于(1,0,0)应返回(0,0,0)");
    
    Vector3f normal3(-1, -1, -1);
    vn = box.GetVertexN(normal3);
    TEST_ASSERT(IsNearlyEqual(vn, Vector3f(10, 10, 10)), 
                "GetVertexN对于(-1,-1,-1)应返回(10,10,10)");
    
    return true;
}

bool test_AABB_Transformed() {
    std::cout << "  测试变换后的AABB..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(2, 2, 2));
    
    // 平移变换
    Matrix4f translation = glm::translate(Matrix4f(1.0f), Vector3f(10, 0, 0));
    AABB transformed = box.Transformed(translation);
    
    TEST_ASSERT(IsNearlyEqual(transformed.GetMin(), Vector3f(10, 0, 0), 0.01f), 
                "平移后最小点错误");
    TEST_ASSERT(IsNearlyEqual(transformed.GetMax(), Vector3f(12, 2, 2), 0.01f), 
                "平移后最大点错误");
    
    // 缩放变换
    Matrix4f scale = glm::scale(Matrix4f(1.0f), Vector3f(2, 2, 2));
    AABB scaled = box.Transformed(scale);
    
    TEST_ASSERT(IsNearlyEqual(scaled.GetMin(), Vector3f(0, 0, 0), 0.01f), 
                "缩放后最小点错误");
    TEST_ASSERT(IsNearlyEqual(scaled.GetMax(), Vector3f(4, 4, 4), 0.01f), 
                "缩放后最大点错误");
    
    // 旋转变换（绕Z轴旋转45度）
    Matrix4f rotation = glm::rotate(Matrix4f(1.0f), glm::f32(HGL_PI / 4.0f), Vector3f(0, 0, 1));
    AABB rotated = box.Transformed(rotation);
    
    // 旋转后AABB会扩大以包含所有顶点
    TEST_ASSERT(rotated.GetVolume() >= box.GetVolume(), 
                "旋转后体积应不小于原体积");
    
    return true;
}

bool test_AABB_EmptyTransformed() {
    std::cout << "  测试空AABB变换..." << std::endl;
    
    AABB empty_box;
    empty_box.Clear();
    
    Matrix4f transform = glm::translate(Matrix4f(1.0f), Vector3f(10, 10, 10));
    AABB transformed = empty_box.Transformed(transform);
    
    TEST_ASSERT(transformed.IsEmpty(), 
                "空AABB变换后应仍为空");
    
    return true;
}

// ============================================================================
// 边界情况测试
// ============================================================================

bool test_AABB_EdgeCases() {
    std::cout << "  测试边界情况..." << std::endl;
    
    // 零大小AABB
    AABB zero_box;
    zero_box.SetMinMax(Vector3f(5, 5, 5), Vector3f(5, 5, 5));
    TEST_ASSERT(zero_box.IsEmpty(), 
                "零大小AABB应为空");
    TEST_ASSERT(IsNearlyEqual(zero_box.GetVolume(), 0.0f), 
                "零大小AABB体积应为0");
    
    // 一维AABB（退化为线段）
    AABB line_box;
    line_box.SetMinMax(Vector3f(0, 5, 5), Vector3f(10, 5, 5));
    TEST_ASSERT(!line_box.IsEmpty(), 
                "线段状AABB不应为空");
    TEST_ASSERT(IsNearlyEqual(line_box.GetVolume(), 0.0f), 
                "线段状AABB体积应为0");
    
    // 二维AABB（退化为平面）
    AABB plane_box;
    plane_box.SetMinMax(Vector3f(0, 0, 5), Vector3f(10, 10, 5));
    TEST_ASSERT(!plane_box.IsEmpty(), 
                "平面状AABB不应为空");
    TEST_ASSERT(IsNearlyEqual(plane_box.GetVolume(), 0.0f), 
                "平面状AABB体积应为0");
    
    return true;
}

bool test_AABB_NegativeCoordinates() {
    std::cout << "  测试负坐标..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(-10, -10, -10), Vector3f(-5, -5, -5));
    
    TEST_ASSERT(IsNearlyEqual(box.GetCenter(), Vector3f(-7.5f, -7.5f, -7.5f)), 
                "负坐标中心计算错误");
    TEST_ASSERT(IsNearlyEqual(box.GetLength(), Vector3f(5, 5, 5)), 
                "负坐标长度计算错误");
    TEST_ASSERT(IsNearlyEqual(box.GetVolume(), 125.0f), 
                "负坐标体积计算错误");
    
    return true;
}

bool test_AABB_LargeCoordinates() {
    std::cout << "  测试大坐标值..." << std::endl;
    
    AABB box;
    box.SetMinMax(Vector3f(1000, 1000, 1000), Vector3f(2000, 2000, 2000));
    
    TEST_ASSERT(box.ContainsPoint(Vector3f(1500, 1500, 1500)), 
                "大坐标点包含测试失败");
    TEST_ASSERT(IsNearlyEqual(box.GetVolume(), 1e9f, 1.0f), 
                "大坐标体积计算错误");
    
    return true;
}

// ============================================================================
// 性能测试
// ============================================================================

bool test_AABB_Performance() {
    std::cout << "  性能测试（简化版）..." << std::endl;
    
    const int NUM_BOXES = 1000;
    std::vector<AABB> boxes;
    boxes.reserve(NUM_BOXES);
    
    // 创建大量AABB
    for (int i = 0; i < NUM_BOXES; ++i) {
        AABB box;
        float x = float(i % 10) * 10.0f;
        float y = float((i / 10) % 10) * 10.0f;
        float z = float(i / 100) * 10.0f;
        box.SetMinMax(Vector3f(x, y, z), Vector3f(x+5, y+5, z+5));
        boxes.push_back(box);
    }
    
    // 执行大量相交测试
    int intersection_count = 0;
    for (int i = 0; i < NUM_BOXES; ++i) {
        for (int j = i + 1; j < NUM_BOXES; ++j) {
            if (boxes[i].Intersects(boxes[j])) {
                intersection_count++;
            }
        }
    }
    
    std::cout << "    完成 " << NUM_BOXES << " 个AABB的相交测试" << std::endl;
    std::cout << "    发现 " << intersection_count << " 对相交的AABB" << std::endl;
    
    return true;
}

// ============================================================================
// 综合场景测试
// ============================================================================

bool test_AABB_ComplexScene() {
    std::cout << "  综合场景测试..." << std::endl;
    
    // 创建一个场景层次结构
    AABB scene_root;
    scene_root.SetMinMax(Vector3f(-100, -100, -100), Vector3f(100, 100, 100));
    
    AABB building1;
    building1.SetMinMax(Vector3f(0, 0, 0), Vector3f(20, 20, 30));
    
    AABB building2;
    building2.SetMinMax(Vector3f(30, 0, 0), Vector3f(50, 15, 25));
    
    // 合并建筑到场景
    AABB buildings_merged = building1;
    buildings_merged.Merge(building2);
    
    TEST_ASSERT(scene_root.Contains(buildings_merged), 
                "场景应包含所有建筑");
    
    // 射线投射测试
    Ray ray(Vector3f(-10, 10, 15), Vector3f(1, 0, 0));
    TEST_ASSERT(building1.IntersectsRay(ray), 
                "射线应击中建筑1");
    
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    AABB (轴对齐包围盒) 全面测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int total = 0;
    int passed = 0;
    int failed = 0;
    
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【基本构造和设置测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_DefaultConstruction);
    RUN_TEST(test_AABB_SetMinMax);
    RUN_TEST(test_AABB_SetCornerLength);
    RUN_TEST(test_AABB_SetFromPoints);
    RUN_TEST(test_AABB_Clear);
    RUN_TEST(test_AABB_IsEmpty);
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【点相关碰撞检测测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_ContainsPoint);
    RUN_TEST(test_AABB_ClosestPoint);
    RUN_TEST(test_AABB_DistanceToPoint);
    RUN_TEST(test_AABB_DistanceToPointSquared);
    RUN_TEST(test_AABB_ExpandToInclude);
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【AABB间碰撞检测测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_Intersects);
    RUN_TEST(test_AABB_Contains);
    RUN_TEST(test_AABB_GetIntersection);
    RUN_TEST(test_AABB_Distance);
    RUN_TEST(test_AABB_Merge);
    RUN_TEST(test_AABB_OperatorPlusEqual);
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【其他几何体碰撞检测测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_IntersectsSphere);
    RUN_TEST(test_AABB_IntersectsRay);
    RUN_TEST(test_AABB_IntersectsPlane);
    RUN_TEST(test_AABB_ClassifyPlane);
    RUN_TEST(test_AABB_IntersectsTriangle);
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【工具函数测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_GetSurfaceArea);
    RUN_TEST(test_AABB_GetVolume);
    RUN_TEST(test_AABB_GetVertexP_N);
    RUN_TEST(test_AABB_Transformed);
    RUN_TEST(test_AABB_EmptyTransformed);
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【边界情况和特殊测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_EdgeCases);
    RUN_TEST(test_AABB_NegativeCoordinates);
    RUN_TEST(test_AABB_LargeCoordinates);
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "【性能和综合测试】" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    RUN_TEST(test_AABB_Performance);
    RUN_TEST(test_AABB_ComplexScene);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "           测试总结" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "总计测试: " << total << std::endl;
    std::cout << "通过: " << passed << " ✓" << std::endl;
    std::cout << "失败: " << failed << " ✗" << std::endl;
    
    if (failed == 0) {
        std::cout << "\n🎉 所有测试通过！" << std::endl;
    } else {
        std::cout << "\n❌ 有 " << failed << " 个测试失败" << std::endl;
    }
    
    std::cout << "========================================\n" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
