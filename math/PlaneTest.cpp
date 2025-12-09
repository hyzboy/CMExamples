#include<hgl/math/geometry/Plane.h>
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
// Plane Tests
// ============================================================================

bool test_Plane_Set() {
    std::cout << "  测试Set方法..." << std::endl;
    
    Plane plane;
    plane.Set(Vector3f(0, 1, 0), 5.0f);
    
    TEST_ASSERT(vec3Equal(plane.normal, Vector3f(0, 1, 0)), "Plane normal wrong");
    TEST_ASSERT(floatEqual(plane.d, 5.0f), "Plane d wrong");
    return true;
}

bool test_Plane_Distance() {
    std::cout << "  测试Distance方法..." << std::endl;
    
    Plane plane;
    plane.Set(Vector3f(0, 1, 0), 0.0f);
    
    Vector3f point(0, 5, 0);
    float dist = plane.Distance(point);
    
    TEST_ASSERT(floatEqual(dist, 5.0f, 0.1f), "Plane distance calculation wrong");
    return true;
}

bool test_Plane_SetFromCenterNormal() {
    std::cout << "  测试从中心点和法线设置..." << std::endl;
    
    Plane plane;
    Vector3f center(0, 5, 0);
    Vector3f normal(0, 1, 0);
    plane.Set(center, normal);
    
    TEST_ASSERT(vec3Equal(plane.normal, normal), "Plane normal from center wrong");
    return true;
}

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "    Plane (平面) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    RUN_TEST(test_Plane_Set);
    RUN_TEST(test_Plane_Distance);
    RUN_TEST(test_Plane_SetFromCenterNormal);
    
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
