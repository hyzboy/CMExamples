#include<hgl/math/geometry/BoundingSphere.h>
#include<hgl/math/MathConstants.h>
#include<iostream>
#include<cassert>
#include<cmath>

using namespace hgl::math;

// 测试辅助宏和函数
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

bool floatEqual(float a, float b, float epsilon = 0.0001f) {
    return std::fabs(a - b) < epsilon;
}

bool vec3Equal(const Vector3f& a, const Vector3f& b, float epsilon = 0.0001f) {
    return floatEqual(a.x, b.x, epsilon) && 
           floatEqual(a.y, b.y, epsilon) && 
           floatEqual(a.z, b.z, epsilon);
}

// ============================================================================
// BoundingSphere Tests
// ============================================================================

bool test_BoundingSphere_Construction() {
    std::cout << "  测试默认构造函数..." << std::endl;
    BoundingSphere sphere;
    TEST_ASSERT(sphere.IsEmpty(), "Default BoundingSphere should be empty");
    return true;
}

bool test_BoundingSphere_SetFromPoints() {
    std::cout << "  测试SetFromPoints方法..." << std::endl;
    float points[] = {
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.0f
    };
    
    BoundingSphere sphere;
    sphere.SetFromPoints(points, 6, 3);
    
    TEST_ASSERT(!sphere.IsEmpty(), "BoundingSphere from points should not be empty");
    TEST_ASSERT(vec3Equal(sphere.center, Vector3f(0, 0, 0)), "BoundingSphere center should be origin");
    TEST_ASSERT(floatEqual(sphere.radius, 1.0f, 0.01f), "BoundingSphere radius should be 1");
    return true;
}

bool test_BoundingSphere_Clear() {
    std::cout << "  测试Clear方法..." << std::endl;
    BoundingSphere sphere;
    sphere.center = Vector3f(1, 2, 3);
    sphere.radius = 5.0f;
    sphere.Clear();
    
    TEST_ASSERT(sphere.IsEmpty(), "Cleared sphere should be empty");
    TEST_ASSERT(vec3Equal(sphere.center, Vector3f(0, 0, 0)), "Cleared sphere center should be zero");
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    BoundingSphere (包围球) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int total = 0;
    int passed = 0;
    int failed = 0;
    
    RUN_TEST(test_BoundingSphere_Construction);
    RUN_TEST(test_BoundingSphere_SetFromPoints);
    RUN_TEST(test_BoundingSphere_Clear);
    
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
    
    return (failed == 0) ? 0 : 1;
}
