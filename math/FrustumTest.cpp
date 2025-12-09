#include<hgl/math/geometry/Frustum.h>
#include<hgl/math/geometry/AABB.h>
#include<hgl/math/MathConstants.h>
#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
#include<glm/ext/matrix_clip_space.hpp>
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

// ============================================================================
// Frustum Tests
// ============================================================================

void printFrustumPlanes(const Matrix4f& mvp) {
    FrustumPlanes planes;
    GetFrustumPlanes(planes, mvp);
    
    const char* names[] = {"Left", "Right", "Front", "Back", "Top", "Bottom"};
    std::cout << "  Frustum Planes:" << std::endl;
    for (int i = 0; i < 6; i++) {
        std::cout << "    " << names[i] << ": normal=(" 
                  << planes[i].x << ", " << planes[i].y << ", " << planes[i].z 
                  << "), d=" << planes[i].w << std::endl;
    }
}

void debugPointAgainstFrustum(const Frustum& frustum, const Vector3f& point, const Matrix4f& mvp) {
    FrustumPlanes planes;
    GetFrustumPlanes(planes, mvp);
    
    const char* names[] = {"Left", "Right", "Front", "Back", "Top", "Bottom"};
    std::cout << "  Testing point (" << point.x << ", " << point.y << ", " << point.z << "):" << std::endl;
    
    for (int i = 0; i < 6; i++) {
        Vector3f normal(planes[i].x, planes[i].y, planes[i].z);
        float distance = dot(normal, point) - planes[i].w;
        std::cout << "    " << names[i] << ": distance=" << distance;
        if (distance < 0) std::cout << " [OUTSIDE]";
        std::cout << std::endl;
    }
}

bool test_Frustum_SetMatrix() {
    std::cout << "  测试SetMatrix方法..." << std::endl;
    
    // OpenGL/GLM右手坐标系：相机看向-Z方向
    // 相机在(0,0,5)，看向(0,0,0)，实际上目标点在相机的-Z方向
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 5), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    std::cout << "  Camera at (0,0,5) looking at (0,0,0) [-Z direction]" << std::endl;
    std::cout << "  Near=0.1, Far=100, FOV=45deg" << std::endl;
    
    printFrustumPlanes(mvp);
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试目标点 (0,0,0) - 应该在视锥体内
    Vector3f testPoint(0, 0, 0);
    debugPointAgainstFrustum(frustum, testPoint, mvp);
    
    auto result = frustum.PointIn(testPoint);
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE, 
                "Target point should be inside frustum");
    
    // 相机后方的点应该在外部
    result = frustum.PointIn(Vector3f(0, 0, 6));
    TEST_ASSERT(result == Frustum::Scope::OUTSIDE, 
                "Point behind camera should be outside frustum");
    
    return true;
}

bool test_Frustum_PointIn() {
    std::cout << "  测试PointIn方法..." << std::endl;
    
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试明显在外面的点
    auto result = frustum.PointIn(Vector3f(1000, 1000, 1000));
    TEST_ASSERT(result == Frustum::Scope::OUTSIDE, "Far point should be outside");
    
    return true;
}

bool test_Frustum_SphereIn() {
    std::cout << "  测试SphereIn方法..." << std::endl;
    
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    std::cout << "  Camera at (0,0,10) looking at (0,0,0) [-Z direction]" << std::endl;
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试多个球体位置（相机看向-Z，目标点在前方，大于相机Z的点在后方）
    struct TestSphere { Vector3f pos; float radius; const char* desc; };
    TestSphere spheres[] = {
        {Vector3f(0, 0, 0), 1.0f, "at target"},
        {Vector3f(0, 0, 5), 1.0f, "midway"},
        {Vector3f(0, 0, 9.5f), 1.0f, "near camera"},
        {Vector3f(0, 0, 12), 1.0f, "behind camera"},
    };
    
    for (const auto& s : spheres) {
        auto res = frustum.SphereIn(s.pos, s.radius);
        std::cout << "  Sphere at (" << s.pos.x << "," << s.pos.y << "," << s.pos.z 
                  << ") r=" << s.radius << " [" << s.desc << "]: ";
        if (res == Frustum::Scope::OUTSIDE) std::cout << "OUTSIDE";
        else if (res == Frustum::Scope::INTERSECT) std::cout << "INTERSECT";
        else std::cout << "INSIDE";
        std::cout << std::endl;
    }
    
    // 目标点的球体应该在视锥体内
    auto result = frustum.SphereIn(Vector3f(0, 0, 0), 1.0f);
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE, "Sphere at target should not be outside");
    
    // 相机后方的球体应该在外部
    result = frustum.SphereIn(Vector3f(0, 0, 12), 1.0f);
    TEST_ASSERT(result == Frustum::Scope::OUTSIDE, "Sphere behind camera should be outside");
    
    return true;
}

bool test_Frustum_BoxIn() {
    std::cout << "  测试BoxIn方法..." << std::endl;
    
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    std::cout << "  Camera at (0,0,10) looking at (0,0,0) [-Z direction]" << std::endl;
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试多个盒子位置（相机看向-Z，目标点在前方，大于相机Z的点在后方）
    struct TestBox { Vector3f min; Vector3f max; const char* desc; };
    TestBox boxes[] = {
        {Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(0.5f, 0.5f, 0.5f), "at target"},
        {Vector3f(-0.5f, -0.5f, 4.5f), Vector3f(0.5f, 0.5f, 5.5f), "midway"},
        {Vector3f(-0.5f, -0.5f, 9.0f), Vector3f(0.5f, 0.5f, 9.8f), "near camera"},
        {Vector3f(-0.5f, -0.5f, 11.5f), Vector3f(0.5f, 0.5f, 12.5f), "behind camera"},
    };
    
    for (const auto& b : boxes) {
        AABB box;
        box.SetMinMax(b.min, b.max);
        auto res = frustum.BoxIn(box);
        Vector3f center = (b.min + b.max) * 0.5f;
        std::cout << "  Box center (" << center.x << "," << center.y << "," << center.z 
                  << ") [" << b.desc << "]: ";
        if (res == Frustum::Scope::OUTSIDE) std::cout << "OUTSIDE";
        else if (res == Frustum::Scope::INTERSECT) std::cout << "INTERSECT";
        else std::cout << "INSIDE";
        std::cout << std::endl;
    }
    
    // 目标点的盒子应该在视锥体内
    AABB box;
    box.SetMinMax(Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(0.5f, 0.5f, 0.5f));
    auto result = frustum.BoxIn(box);
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE, "Box at target should not be outside");
    
    // 相机后方的盒子应该在外部
    box.SetMinMax(Vector3f(-0.5f, -0.5f, 11.5f), Vector3f(0.5f, 0.5f, 12.5f));
    result = frustum.BoxIn(box);
    TEST_ASSERT(result == Frustum::Scope::OUTSIDE, "Box behind camera should be outside");
    
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    Frustum (视锥体) 测试" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    int total = 0;
    int passed = 0;
    int failed = 0;
    
    RUN_TEST(test_Frustum_SetMatrix);
    RUN_TEST(test_Frustum_PointIn);
    RUN_TEST(test_Frustum_SphereIn);
    RUN_TEST(test_Frustum_BoxIn);
    
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
