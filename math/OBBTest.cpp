#include<hgl/math/geometry/OBB.h>
#include<hgl/math/MathConstants.h>
#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
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
// OBB Tests
// ============================================================================

bool test_OBB_Construction() {
    std::cout << "  测试默认构造函数..." << std::endl;
    OBB obb;
    TEST_ASSERT(vec3Equal(obb.GetCenter(), Vector3f(0, 0, 0)), "Default OBB center wrong");
    TEST_ASSERT(vec3Equal(obb.GetHalfExtend(), Vector3f(1, 1, 1)), "Default OBB half extent wrong");
    return true;
}

bool test_OBB_Set() {
    std::cout << "  测试Set方法..." << std::endl;
    OBB obb;
    Vector3f center(1, 2, 3);
    Vector3f halfLen(0.5f, 1.0f, 1.5f);
    obb.Set(center, halfLen);
    
    TEST_ASSERT(vec3Equal(obb.GetCenter(), center), "OBB center not set");
    TEST_ASSERT(vec3Equal(obb.GetHalfExtend(), halfLen), "OBB half extent not set");
    return true;
}

bool test_OBB_SetWithAxes() {
    std::cout << "  测试Set with axes方法..." << std::endl;
    OBB obb;
    Vector3f center(0, 0, 0);
    Vector3f axis0(1, 0, 0);
    Vector3f axis1(0, 1, 0);
    Vector3f axis2(0, 0, 1);
    Vector3f halfLen(1, 1, 1);
    
    obb.Set(center, axis0, axis1, axis2, halfLen);
    
    TEST_ASSERT(vec3Equal(obb.GetAxis(0), axis0), "OBB axis 0 wrong");
    TEST_ASSERT(vec3Equal(obb.GetAxis(1), axis1), "OBB axis 1 wrong");
    TEST_ASSERT(vec3Equal(obb.GetAxis(2), axis2), "OBB axis 2 wrong");
    return true;
}

bool test_OBB_GetCorners() {
    std::cout << "  测试GetCorners方法..." << std::endl;
    OBB obb;
    obb.Set(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    
    Vector3f corners[8];
    obb.GetCorners(corners);
    
    // 检查所有角点都在合理范围内
    for(int i = 0; i < 8; i++) {
        TEST_ASSERT(corners[i].x >= -1.0f && corners[i].x <= 1.0f, "OBB corner X out of range");
        TEST_ASSERT(corners[i].y >= -1.0f && corners[i].y <= 1.0f, "OBB corner Y out of range");
        TEST_ASSERT(corners[i].z >= -1.0f && corners[i].z <= 1.0f, "OBB corner Z out of range");
    }
    return true;
}

bool test_OBB_SetFromPoints() {
    std::cout << "  测试SetFromPoints方法..." << std::endl;
    float points[] = {
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f
    };
    
    OBB obb;
    obb.SetFromPoints(points, 8, 3, 45.0f, 15.0f, 5.0f);
    
    TEST_ASSERT(!obb.IsEmpty(), "OBB from points should not be empty");
    float volume = obb.GetHalfExtend().x * obb.GetHalfExtend().y * obb.GetHalfExtend().z * 8.0f;
    TEST_ASSERT(volume > 6.0f && volume < 10.0f, "OBB volume seems unreasonable");
    return true;
}

bool test_OBB_Transformed() {
    std::cout << "  测试Transformed方法..." << std::endl;
    OBB obb;
    obb.Set(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    
    Matrix4f transform = glm::translate(Matrix4f(1.0f), Vector3f(3, 3, 3));
    OBB transformed = obb.Transformed(transform);
    
    TEST_ASSERT(vec3Equal(transformed.GetCenter(), Vector3f(3, 3, 3)), "Transformed OBB center wrong");
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    OBB (有向包围盒) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int total = 0;
    int passed = 0;
    int failed = 0;
    
    RUN_TEST(test_OBB_Construction);
    RUN_TEST(test_OBB_Set);
    RUN_TEST(test_OBB_SetWithAxes);
    RUN_TEST(test_OBB_GetCorners);
    RUN_TEST(test_OBB_SetFromPoints);
    RUN_TEST(test_OBB_Transformed);
    
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
