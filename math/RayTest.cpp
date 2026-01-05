#include<hgl/math/geometry/Ray.h>
#include<hgl/math/geometry/primitives/Sphere.h>
#include<iostream>
#include<cassert>
#include<cmath>

using namespace hgl::math;

// 测试辅助宏
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
            std::cout << "  ✓ PASSED" << std::endl; \
            passed++; \
        } else { \
            std::cout << "  ✗ FAILED" << std::endl; \
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
// Ray Tests
// ============================================================================

bool test_Ray_Construction() {
    std::cout << "  测试默认构造函数..." << std::endl;
    
    Ray ray;
    TEST_ASSERT(vec3Equal(ray.origin, Vector3f(0, 0, 0)), "Default ray origin wrong");
    TEST_ASSERT(vec3Equal(ray.direction, Vector3f(0, 0, 0)), "Default ray direction wrong");
    return true;
}

bool test_Ray_ConstructionWithParams() {
    std::cout << "  测试带参数的构造函数..." << std::endl;
    
    Ray ray(Vector3f(1, 2, 3), Vector3f(0, 0, 1));
    TEST_ASSERT(vec3Equal(ray.origin, Vector3f(1, 2, 3)), "Ray origin not set");
    TEST_ASSERT(vec3Equal(ray.direction, Vector3f(0, 0, 1)), "Ray direction not set");
    return true;
}

bool test_Ray_GetLengthPoint() {
    std::cout << "  测试GetLengthPoint方法..." << std::endl;
    
    Ray ray(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f point = ray.GetLengthPoint(5.0f);
    
    TEST_ASSERT(vec3Equal(point, Vector3f(5, 0, 0)), "Ray GetLengthPoint wrong");
    return true;
}

bool test_Ray_ClosestPoint() {
    std::cout << "  测试ClosestPoint方法..." << std::endl;
    
    Ray ray(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f point(5, 3, 0);
    Vector3f closest = ray.ClosestPoint(point);
    
    TEST_ASSERT(vec3Equal(closest, Vector3f(5, 0, 0)), "Ray ClosestPoint wrong");
    return true;
}

bool test_Ray_ToPointDistance() {
    std::cout << "  测试ToPointDistance方法..." << std::endl;
    
    Ray ray(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f point(5, 3, 4);
    float dist = ray.ToPointDistance(point);
    
    TEST_ASSERT(floatEqual(dist, 5.0f), "Ray ToPointDistance wrong");
    return true;
}

bool test_Ray_CrossSphere() {
    std::cout << "  测试CrossSphere方法..." << std::endl;
    
    Ray ray(Vector3f(-5, 0, 0), Vector3f(1, 0, 0));
    Sphere sphere(Vector3f(0, 0, 0), 1.0f);
    
    TEST_ASSERT(ray.CrossSphere(sphere), "Ray should intersect sphere");
    
    Ray ray2(Vector3f(-5, 10, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(!ray2.CrossSphere(sphere), "Ray should not intersect sphere");
    
    return true;
}

bool test_Ray_CrossCircle() {
    std::cout << "  测试CrossCircle方法..." << std::endl;
    
    Ray ray(Vector3f(0, 0, -5), Vector3f(0, 0, 1));
    Vector3f center(0, 0, 0);
    Vector3f normal(0, 0, 1);
    float radius = 2.0f;
    
    TEST_ASSERT(ray.CrossCircle(center, normal, radius), "Ray should intersect circle");
    
    return true;
}

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "    Ray (射线) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    RUN_TEST(test_Ray_Construction);
    RUN_TEST(test_Ray_ConstructionWithParams);
    RUN_TEST(test_Ray_GetLengthPoint);
    RUN_TEST(test_Ray_ClosestPoint);
    RUN_TEST(test_Ray_ToPointDistance);
    RUN_TEST(test_Ray_CrossSphere);
    RUN_TEST(test_Ray_CrossCircle);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "测试总结" << std::endl;
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
    
    return failed == 0 ? 0 : 1;
}
