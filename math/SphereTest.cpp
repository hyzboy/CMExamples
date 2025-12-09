#include<hgl/math/geometry/Sphere.h>
#include<hgl/math/MathConstants.h>
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
// Sphere Tests
// ============================================================================

bool test_Sphere_Construction() {
    std::cout << "  测试默认构造函数..." << std::endl;
    
    Sphere sphere;
    TEST_ASSERT(vec3Equal(sphere.GetCenter(), Vector3f(0, 0, 0)), "Default sphere center wrong");
    TEST_ASSERT(floatEqual(sphere.GetRadius(), 1.0f), "Default sphere radius wrong");
    return true;
}

bool test_Sphere_Set() {
    std::cout << "  测试Set方法..." << std::endl;
    
    Sphere sphere;
    sphere.Set(Vector3f(1, 2, 3), 5.0f);
    
    TEST_ASSERT(vec3Equal(sphere.GetCenter(), Vector3f(1, 2, 3)), "Sphere center not set");
    TEST_ASSERT(floatEqual(sphere.GetRadius(), 5.0f), "Sphere radius not set");
    return true;
}

bool test_Sphere_GetVolume() {
    std::cout << "  测试GetVolume方法..." << std::endl;
    
    Sphere sphere(Vector3f(0, 0, 0), 1.0f);
    float volume = sphere.GetVolume();
    float expected = (4.0f/3.0f) * hgl::math::pi;
    
    TEST_ASSERT(floatEqual(volume, expected, 0.01f), "Sphere volume calculation wrong");
    return true;
}

bool test_Sphere_GetPoint() {
    std::cout << "  测试GetPoint方法..." << std::endl;
    
    Sphere sphere(Vector3f(0, 0, 0), 2.0f);
    Vector3f dir(1, 0, 0);
    Vector3f point = sphere.GetPoint(dir);
    
    TEST_ASSERT(vec3Equal(point, Vector3f(2, 0, 0)), "Sphere GetPoint wrong");
    return true;
}

// ============================================================================
// EllipseSphere Tests
// ============================================================================

bool test_EllipseSphere_Construction() {
    std::cout << "  测试默认构造函数..." << std::endl;
    
    EllipseSphere sphere;
    TEST_ASSERT(vec3Equal(sphere.GetCenter(), Vector3f(0, 0, 0)), "Default ellipse sphere center wrong");
    TEST_ASSERT(vec3Equal(sphere.GetRadius(), Vector3f(1, 1, 1)), "Default ellipse sphere radius wrong");
    return true;
}

bool test_EllipseSphere_GetVolume() {
    std::cout << "  测试GetVolume方法..." << std::endl;
    
    EllipseSphere sphere(Vector3f(0, 0, 0), Vector3f(1, 2, 3));
    float volume = sphere.GetVolume();
    float expected = (4.0f/3.0f) * hgl::math::pi * 1.0f * 2.0f * 3.0f;
    
    TEST_ASSERT(floatEqual(volume, expected, 0.01f), "Ellipse sphere volume wrong");
    return true;
}

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "    Sphere (球体) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    RUN_TEST(test_Sphere_Construction);
    RUN_TEST(test_Sphere_Set);
    RUN_TEST(test_Sphere_GetVolume);
    RUN_TEST(test_Sphere_GetPoint);
    RUN_TEST(test_EllipseSphere_Construction);
    RUN_TEST(test_EllipseSphere_GetVolume);
    
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
