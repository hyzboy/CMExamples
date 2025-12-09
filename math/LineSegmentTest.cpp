#include<hgl/math/geometry/LineSegment.h>
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
// LineSegment Tests
// ============================================================================

bool test_LineSegment_Construction() {
    std::cout << "  测试构造函数..." << std::endl;
    
    LineSegment seg(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    TEST_ASSERT(vec3Equal(seg.GetStart(), Vector3f(0, 0, 0)), "LineSegment start wrong");
    TEST_ASSERT(vec3Equal(seg.GetEnd(), Vector3f(1, 1, 1)), "LineSegment end wrong");
    return true;
}

bool test_LineSegment_Distance() {
    std::cout << "  测试Distance方法..." << std::endl;
    
    LineSegment seg(Vector3f(0, 0, 0), Vector3f(3, 0, 0));
    float dist = seg.Distance();
    
    TEST_ASSERT(floatEqual(dist, 3.0f), "LineSegment distance wrong");
    return true;
}

bool test_LineSegment_Direction() {
    std::cout << "  测试GetStartDirection方法..." << std::endl;
    
    LineSegment seg(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f dir = seg.GetStartDirection();
    
    TEST_ASSERT(vec3Equal(dir, Vector3f(1, 0, 0)), "LineSegment start direction wrong");
    return true;
}

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "    LineSegment (线段) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    RUN_TEST(test_LineSegment_Construction);
    RUN_TEST(test_LineSegment_Distance);
    RUN_TEST(test_LineSegment_Direction);
    
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
