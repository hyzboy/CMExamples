#include<hgl/math/geometry/AABB.h>
#include<hgl/math/geometry/OBB.h>
#include<hgl/math/geometry/BoundingSphere.h>
#include<hgl/math/geometry/BoundingVolumes.h>
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
// BoundingVolumes Tests
// ============================================================================

bool test_BoundingVolumes_SetFromAABB() {
    std::cout << "  测试SetFromAABB方法..." << std::endl;
    
    AABB aabb;
    aabb.SetMinMax(Vector3f(-1, -1, -1), Vector3f(1, 1, 1));
    
    BoundingVolumes bv;
    bv.SetFromAABB(aabb);
    
    TEST_ASSERT(!bv.IsEmpty(), "BoundingVolumes should not be empty");
    TEST_ASSERT(vec3Equal(bv.aabb.GetCenter(), Vector3f(0, 0, 0)), "BoundingVolumes AABB center wrong");
    TEST_ASSERT(floatEqual(bv.bsphere.radius, std::sqrt(3.0f), 0.01f), "BoundingVolumes sphere radius wrong");
    return true;
}

bool test_BoundingVolumes_SetFromPoints() {
    std::cout << "  测试SetFromPoints方法..." << std::endl;
    
    float points[] = {
        -1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f
    };
    
    BoundingVolumes bv;
    bool result = bv.SetFromPoints(points, 2, 3);
    
    TEST_ASSERT(result, "SetFromPoints should return true");
    TEST_ASSERT(!bv.IsEmpty(), "BoundingVolumes should not be empty");
    return true;
}

bool test_BoundingVolumes_Conversions() {
    std::cout << "  测试包围体转换..." << std::endl;
    
    AABB aabb;
    aabb.SetMinMax(Vector3f(0, 0, 0), Vector3f(2, 2, 2));
    
    BoundingSphere sphere = ToBoundingSphere(aabb);
    TEST_ASSERT(!sphere.IsEmpty(), "Converted sphere should not be empty");
    
    OBB obb = ToOBB(aabb);
    TEST_ASSERT(!obb.IsEmpty(), "Converted OBB should not be empty");
    TEST_ASSERT(vec3Equal(obb.GetCenter(), aabb.GetCenter()), "OBB center should match AABB center");
    
    AABB aabb2 = ToAABB(obb);
    TEST_ASSERT(!aabb2.IsEmpty(), "Converted AABB should not be empty");
    
    return true;
}

bool test_BoundingVolumes_Pack() {
    std::cout << "  测试Pack/Unpack方法..." << std::endl;
    
    BoundingVolumes bv;
    AABB aabb;
    aabb.SetMinMax(Vector3f(-1, -1, -1), Vector3f(1, 1, 1));
    bv.SetFromAABB(aabb);
    
    BoundingVolumesData packed;
    bv.Pack(&packed);
    
    TEST_ASSERT(floatEqual(packed.aabbMin[0], -1.0f), "Packed AABB min X wrong");
    TEST_ASSERT(floatEqual(packed.aabbMax[0], 1.0f), "Packed AABB max X wrong");
    
    BoundingVolumes unpacked;
    packed.To(&unpacked);
    
    TEST_ASSERT(vec3Equal(unpacked.aabb.GetMin(), bv.aabb.GetMin()), "Unpacked AABB min wrong");
    return true;
}

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "    BoundingVolumes (包围体集合) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    RUN_TEST(test_BoundingVolumes_SetFromAABB);
    RUN_TEST(test_BoundingVolumes_SetFromPoints);
    RUN_TEST(test_BoundingVolumes_Conversions);
    RUN_TEST(test_BoundingVolumes_Pack);
    
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
