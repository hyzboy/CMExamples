#include<hgl/math/geometry/Triangle.h>
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
// Triangle Tests
// ============================================================================

bool test_Triangle2_Construction() {
    std::cout << "  测试Triangle2构造函数..." << std::endl;
    
    Triangle2f tri(Vector2f(0, 0), Vector2f(1, 0), Vector2f(0, 1));
    
    TEST_ASSERT(vec3Equal(Vector3f(tri[0], 0), Vector3f(0, 0, 0)), "Triangle2 vertex 0 wrong");
    TEST_ASSERT(vec3Equal(Vector3f(tri[1], 0), Vector3f(1, 0, 0)), "Triangle2 vertex 1 wrong");
    TEST_ASSERT(vec3Equal(Vector3f(tri[2], 0), Vector3f(0, 1, 0)), "Triangle2 vertex 2 wrong");
    return true;
}

bool test_Triangle2_PointIn() {
    std::cout << "  测试Triangle2的PointIn方法..." << std::endl;
    
    Triangle2f tri(Vector2f(0, 0), Vector2f(2, 0), Vector2f(0, 2));
    
    TEST_ASSERT(tri.PointIn(Vector2f(0.5f, 0.5f)), "Point should be in triangle");
    TEST_ASSERT(!tri.PointIn(Vector2f(2, 2)), "Point should be outside triangle");
    
    return true;
}

bool test_Triangle3_Construction() {
    std::cout << "  测试Triangle3构造函数..." << std::endl;
    
    Triangle3f tri(Vector3f(0, 0, 0), Vector3f(1, 0, 0), Vector3f(0, 1, 0));
    
    TEST_ASSERT(vec3Equal(tri[0], Vector3f(0, 0, 0)), "Triangle3 vertex 0 wrong");
    TEST_ASSERT(vec3Equal(tri[1], Vector3f(1, 0, 0)), "Triangle3 vertex 1 wrong");
    TEST_ASSERT(vec3Equal(tri[2], Vector3f(0, 1, 0)), "Triangle3 vertex 2 wrong");
    return true;
}

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "    Triangle (三角形) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    RUN_TEST(test_Triangle2_Construction);
    RUN_TEST(test_Triangle2_PointIn);
    RUN_TEST(test_Triangle3_Construction);
    
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
