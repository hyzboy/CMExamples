#include<hgl/math/geometry/AABB.h>
#include<hgl/math/geometry/OBB.h>
#include<hgl/math/geometry/BoundingSphere.h>
#include<hgl/math/geometry/BoundingVolumes.h>
#include<hgl/math/geometry/Frustum.h>
#include<hgl/math/geometry/Ray.h>
#include<hgl/math/geometry/Sphere.h>
#include<hgl/math/geometry/Capsule.h>
#include<hgl/math/geometry/LineSegment.h>
#include<hgl/math/geometry/Plane.h>
#include<hgl/math/geometry/Triangle.h>
#include<hgl/math/MathConstants.h>
#include<glm/glm.hpp>
#include<glm/ext/matrix_transform.hpp>
#include<glm/ext/matrix_clip_space.hpp>
#include<iostream>
#include<cassert>
#include<cmath>
#include<vector>

using namespace hgl::math;

// 测试辅助函数
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
            std::cout << "  PASSED" << std::endl; \
            passed++; \
        } else { \
            std::cout << "  FAILED" << std::endl; \
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
// AABB Tests
// ============================================================================

bool test_AABB_Construction() {
    AABB box;
    TEST_ASSERT(vec3Equal(box.GetMin(), Vector3f(0,0,0)), "Default AABB min should be (0,0,0)");
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(1,1,1)), "Default AABB max should be (1,1,1)");
    return true;
}

bool test_AABB_SetMinMax() {
    AABB box;
    Vector3f min(-1, -2, -3);
    Vector3f max(4, 5, 6);
    box.SetMinMax(min, max);
    
    TEST_ASSERT(vec3Equal(box.GetMin(), min), "AABB min not set correctly");
    TEST_ASSERT(vec3Equal(box.GetMax(), max), "AABB max not set correctly");
    TEST_ASSERT(vec3Equal(box.GetCenter(), Vector3f(1.5f, 1.5f, 1.5f)), "AABB center calculation wrong");
    TEST_ASSERT(vec3Equal(box.GetLength(), Vector3f(5, 7, 9)), "AABB length calculation wrong");
    return true;
}

bool test_AABB_SetCornerLength() {
    AABB box;
    Vector3f corner(1, 2, 3);
    Vector3f length(4, 5, 6);
    box.SetCornerLength(corner, length);
    
    TEST_ASSERT(vec3Equal(box.GetMin(), corner), "AABB corner not set correctly");
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(5, 7, 9)), "AABB max from corner+length wrong");
    return true;
}

bool test_AABB_SetFromPoints() {
    float points[] = {
        -1.0f, -2.0f, -3.0f,
        4.0f, 5.0f, 6.0f,
        2.0f, 1.0f, 0.0f,
        -2.0f, 3.0f, 4.0f
    };
    
    AABB box;
    box.SetFromPoints(points, 4, 3);
    
    TEST_ASSERT(vec3Equal(box.GetMin(), Vector3f(-2, -2, -3)), "AABB SetFromPoints min wrong");
    TEST_ASSERT(vec3Equal(box.GetMax(), Vector3f(4, 5, 6)), "AABB SetFromPoints max wrong");
    return true;
}

bool test_AABB_Merge() {
    AABB box1, box2;
    box1.SetMinMax(Vector3f(0, 0, 0), Vector3f(2, 2, 2));
    box2.SetMinMax(Vector3f(1, 1, 1), Vector3f(3, 3, 3));
    
    box1.Merge(box2);
    TEST_ASSERT(vec3Equal(box1.GetMin(), Vector3f(0, 0, 0)), "AABB merge min wrong");
    TEST_ASSERT(vec3Equal(box1.GetMax(), Vector3f(3, 3, 3)), "AABB merge max wrong");
    return true;
}

bool test_AABB_IsEmpty() {
    AABB box;
    box.SetMinMax(Vector3f(1, 1, 1), Vector3f(1, 1, 1));
    TEST_ASSERT(box.IsEmpty(), "Zero-size AABB should be empty");
    
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    TEST_ASSERT(!box.IsEmpty(), "Non-zero AABB should not be empty");
    return true;
}

bool test_AABB_Transformed() {
    AABB box;
    box.SetMinMax(Vector3f(-1, -1, -1), Vector3f(1, 1, 1));
    
    // 简单平移变换
    Matrix4f transform = glm::translate(Matrix4f(1.0f), Vector3f(5, 5, 5));
    AABB transformed = box.Transformed(transform);
    
    TEST_ASSERT(vec3Equal(transformed.GetMin(), Vector3f(4, 4, 4)), "Transformed AABB min wrong");
    TEST_ASSERT(vec3Equal(transformed.GetMax(), Vector3f(6, 6, 6)), "Transformed AABB max wrong");
    return true;
}

bool test_AABB_GetVertexPN() {
    AABB box;
    box.SetMinMax(Vector3f(0, 0, 0), Vector3f(2, 2, 2));
    
    Vector3f normal(1, 1, 1);
    Vector3f vp = box.GetVertexP(normal);
    Vector3f vn = box.GetVertexN(normal);
    
    TEST_ASSERT(vec3Equal(vp, Vector3f(2, 2, 2)), "GetVertexP wrong");
    TEST_ASSERT(vec3Equal(vn, Vector3f(0, 0, 0)), "GetVertexN wrong");
    return true;
}

// ============================================================================
// OBB Tests
// ============================================================================

bool test_OBB_Construction() {
    OBB obb;
    TEST_ASSERT(vec3Equal(obb.GetCenter(), Vector3f(0, 0, 0)), "Default OBB center wrong");
    TEST_ASSERT(vec3Equal(obb.GetHalfExtend(), Vector3f(1, 1, 1)), "Default OBB half extent wrong");
    return true;
}

bool test_OBB_Set() {
    OBB obb;
    Vector3f center(1, 2, 3);
    Vector3f halfLen(0.5f, 1.0f, 1.5f);
    obb.Set(center, halfLen);
    
    TEST_ASSERT(vec3Equal(obb.GetCenter(), center), "OBB center not set");
    TEST_ASSERT(vec3Equal(obb.GetHalfExtend(), halfLen), "OBB half extent not set");
    return true;
}

bool test_OBB_SetWithAxes() {
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
    // 使用较大的步长以加快测试速度
    obb.SetFromPoints(points, 8, 3, 45.0f, 15.0f, 5.0f);
    
    TEST_ASSERT(!obb.IsEmpty(), "OBB from points should not be empty");
    // OBB应该包围一个立方体,体积应该接近8
    float volume = obb.GetHalfExtend().x * obb.GetHalfExtend().y * obb.GetHalfExtend().z * 8.0f;
    TEST_ASSERT(volume > 6.0f && volume < 10.0f, "OBB volume seems unreasonable");
    return true;
}

bool test_OBB_Transformed() {
    OBB obb;
    obb.Set(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    
    Matrix4f transform = glm::translate(Matrix4f(1.0f), Vector3f(3, 3, 3));
    OBB transformed = obb.Transformed(transform);
    
    TEST_ASSERT(vec3Equal(transformed.GetCenter(), Vector3f(3, 3, 3)), "Transformed OBB center wrong");
    return true;
}

// ============================================================================
// BoundingSphere Tests
// ============================================================================

bool test_BoundingSphere_Construction() {
    BoundingSphere sphere;
    TEST_ASSERT(sphere.IsEmpty(), "Default BoundingSphere should be empty");
    return true;
}

bool test_BoundingSphere_SetFromPoints() {
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
    BoundingSphere sphere;
    sphere.center = Vector3f(1, 2, 3);
    sphere.radius = 5.0f;
    sphere.Clear();
    
    TEST_ASSERT(sphere.IsEmpty(), "Cleared sphere should be empty");
    TEST_ASSERT(vec3Equal(sphere.center, Vector3f(0, 0, 0)), "Cleared sphere center should be zero");
    return true;
}

// ============================================================================
// BoundingVolumes Tests
// ============================================================================

bool test_BoundingVolumes_SetFromAABB() {
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

bool test_Frustum_SetMatrix() {
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 5), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    std::cout << "  Camera at (0,0,5) looking at (0,0,0)" << std::endl;
    std::cout << "  Near=0.1, Far=100, FOV=45deg" << std::endl;
    
    printFrustumPlanes(mvp);
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试多个点
    Vector3f test_points[] = {
        Vector3f(0, 0, 0),   // 目标点
        Vector3f(0, 0, 2),   // 相机前方
        Vector3f(0, 0, 4),   // 更接近相机
        Vector3f(0, 0, -5),  // 相机后方
    };
    
    for (const auto& pt : test_points) {
        auto result = frustum.PointIn(pt);
        std::cout << "  Point(" << pt.x << "," << pt.y << "," << pt.z << "): ";
        if (result == Frustum::Scope::OUTSIDE) std::cout << "OUTSIDE";
        else if (result == Frustum::Scope::INTERSECT) std::cout << "INTERSECT";
        else std::cout << "INSIDE";
        std::cout << std::endl;
    }
    
    // 测试相机前方的点（在近裁剪面和远裁剪面之间）
    auto result = frustum.PointIn(Vector3f(0, 0, 2));
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE, 
                "Point in front of camera should be inside frustum");
    return true;
}

bool test_Frustum_PointIn() {
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
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    std::cout << "  Camera at (0,0,10) looking at (0,0,0)" << std::endl;
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试多个球体位置
    struct TestSphere { Vector3f pos; float radius; const char* desc; };
    TestSphere spheres[] = {
        {Vector3f(0, 0, 0), 1.0f, "at target"},
        {Vector3f(0, 0, 5), 1.0f, "midway"},
        {Vector3f(0, 0, 9), 1.0f, "near camera"},
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
    
    // 测试相机前方的球体（在近裁剪面和远裁剪面之间）
    auto result = frustum.SphereIn(Vector3f(0, 0, 5), 1.0f);
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE, "Sphere in front of camera should not be outside");
    
    return true;
}

bool test_Frustum_BoxIn() {
    Matrix4f proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    Matrix4f view = glm::lookAt(Vector3f(0, 0, 10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f mvp = proj * view;
    
    std::cout << "  Camera at (0,0,10) looking at (0,0,0)" << std::endl;
    
    Frustum frustum;
    frustum.SetMatrix(mvp);
    
    // 测试多个盒子位置
    struct TestBox { Vector3f min; Vector3f max; const char* desc; };
    TestBox boxes[] = {
        {Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(0.5f, 0.5f, 0.5f), "at target"},
        {Vector3f(-0.5f, -0.5f, 4.5f), Vector3f(0.5f, 0.5f, 5.5f), "midway"},
        {Vector3f(-0.5f, -0.5f, 8.5f), Vector3f(0.5f, 0.5f, 9.5f), "near camera"},
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
    
    // 测试相机前方的盒子（在近裁剪面和远裁剪面之间）
    AABB box;
    box.SetMinMax(Vector3f(-0.5f, -0.5f, 4.5f), Vector3f(0.5f, 0.5f, 5.5f));
    
    auto result = frustum.BoxIn(box);
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE, "Box in front of camera should not be outside");
    
    return true;
}

// ============================================================================
// Sphere Tests
// ============================================================================

bool test_Sphere_Construction() {
    Sphere sphere;
    TEST_ASSERT(vec3Equal(sphere.GetCenter(), Vector3f(0, 0, 0)), "Default sphere center wrong");
    TEST_ASSERT(floatEqual(sphere.GetRadius(), 1.0f), "Default sphere radius wrong");
    return true;
}

bool test_Sphere_Set() {
    Sphere sphere;
    sphere.Set(Vector3f(1, 2, 3), 5.0f);
    
    TEST_ASSERT(vec3Equal(sphere.GetCenter(), Vector3f(1, 2, 3)), "Sphere center not set");
    TEST_ASSERT(floatEqual(sphere.GetRadius(), 5.0f), "Sphere radius not set");
    return true;
}

bool test_Sphere_GetVolume() {
    Sphere sphere(Vector3f(0, 0, 0), 1.0f);
    float volume = sphere.GetVolume();
    float expected = (4.0f/3.0f) * HGL_PI;
    
    TEST_ASSERT(floatEqual(volume, expected, 0.01f), "Sphere volume calculation wrong");
    return true;
}

bool test_Sphere_GetPoint() {
    Sphere sphere(Vector3f(0, 0, 0), 2.0f);
    Vector3f dir(1, 0, 0);
    Vector3f point = sphere.GetPoint(dir);
    
    TEST_ASSERT(vec3Equal(point, Vector3f(2, 0, 0)), "Sphere GetPoint wrong");
    return true;
}

bool test_EllipseSphere_Construction() {
    EllipseSphere sphere;
    TEST_ASSERT(vec3Equal(sphere.GetCenter(), Vector3f(0, 0, 0)), "Default ellipse sphere center wrong");
    TEST_ASSERT(vec3Equal(sphere.GetRadius(), Vector3f(1, 1, 1)), "Default ellipse sphere radius wrong");
    return true;
}

bool test_EllipseSphere_GetVolume() {
    EllipseSphere sphere(Vector3f(0, 0, 0), Vector3f(1, 2, 3));
    float volume = sphere.GetVolume();
    float expected = (4.0f/3.0f) * HGL_PI * 1.0f * 2.0f * 3.0f;
    
    TEST_ASSERT(floatEqual(volume, expected, 0.01f), "Ellipse sphere volume wrong");
    return true;
}

// ============================================================================
// Ray Tests
// ============================================================================

bool test_Ray_Construction() {
    Ray ray;
    TEST_ASSERT(vec3Equal(ray.origin, Vector3f(0, 0, 0)), "Default ray origin wrong");
    TEST_ASSERT(vec3Equal(ray.direction, Vector3f(0, 0, 0)), "Default ray direction wrong");
    return true;
}

bool test_Ray_ConstructionWithParams() {
    Ray ray(Vector3f(1, 2, 3), Vector3f(0, 0, 1));
    TEST_ASSERT(vec3Equal(ray.origin, Vector3f(1, 2, 3)), "Ray origin not set");
    TEST_ASSERT(vec3Equal(ray.direction, Vector3f(0, 0, 1)), "Ray direction not set");
    return true;
}

bool test_Ray_GetLengthPoint() {
    Ray ray(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f point = ray.GetLengthPoint(5.0f);
    
    TEST_ASSERT(vec3Equal(point, Vector3f(5, 0, 0)), "Ray GetLengthPoint wrong");
    return true;
}

bool test_Ray_ClosestPoint() {
    Ray ray(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f point(5, 3, 0);
    Vector3f closest = ray.ClosestPoint(point);
    
    TEST_ASSERT(vec3Equal(closest, Vector3f(5, 0, 0)), "Ray ClosestPoint wrong");
    return true;
}

bool test_Ray_ToPointDistance() {
    Ray ray(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f point(5, 3, 4);
    float dist = ray.ToPointDistance(point);
    
    TEST_ASSERT(floatEqual(dist, 5.0f), "Ray ToPointDistance wrong");
    return true;
}

bool test_Ray_CrossSphere() {
    Ray ray(Vector3f(-5, 0, 0), Vector3f(1, 0, 0));
    Sphere sphere(Vector3f(0, 0, 0), 1.0f);
    
    TEST_ASSERT(ray.CrossSphere(sphere), "Ray should intersect sphere");
    
    Ray ray2(Vector3f(-5, 10, 0), Vector3f(1, 0, 0));
    TEST_ASSERT(!ray2.CrossSphere(sphere), "Ray should not intersect sphere");
    
    return true;
}

bool test_Ray_CrossCircle() {
    Ray ray(Vector3f(0, 0, -5), Vector3f(0, 0, 1));
    Vector3f center(0, 0, 0);
    Vector3f normal(0, 0, 1);
    float radius = 2.0f;
    
    TEST_ASSERT(ray.CrossCircle(center, normal, radius), "Ray should intersect circle");
    
    return true;
}

// ============================================================================
// LineSegment Tests
// ============================================================================

bool test_LineSegment_Construction() {
    LineSegment seg(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    TEST_ASSERT(vec3Equal(seg.GetStart(), Vector3f(0, 0, 0)), "LineSegment start wrong");
    TEST_ASSERT(vec3Equal(seg.GetEnd(), Vector3f(1, 1, 1)), "LineSegment end wrong");
    return true;
}

bool test_LineSegment_Distance() {
    LineSegment seg(Vector3f(0, 0, 0), Vector3f(3, 0, 0));
    float dist = seg.Distance();
    
    TEST_ASSERT(floatEqual(dist, 3.0f), "LineSegment distance wrong");
    return true;
}

bool test_LineSegment_Direction() {
    LineSegment seg(Vector3f(0, 0, 0), Vector3f(1, 0, 0));
    Vector3f dir = seg.GetStartDirection();
    
    TEST_ASSERT(vec3Equal(dir, Vector3f(1, 0, 0)), "LineSegment start direction wrong");
    return true;
}

// ============================================================================
// Plane Tests
// ============================================================================

bool test_Plane_Set() {
    Plane plane;
    plane.Set(Vector3f(0, 1, 0), 5.0f);
    
    TEST_ASSERT(vec3Equal(plane.normal, Vector3f(0, 1, 0)), "Plane normal wrong");
    TEST_ASSERT(floatEqual(plane.d, 5.0f), "Plane d wrong");
    return true;
}

bool test_Plane_Distance() {
    Plane plane;
    plane.Set(Vector3f(0, 1, 0), 0.0f);
    
    Vector3f point(0, 5, 0);
    float dist = plane.Distance(point);
    
    TEST_ASSERT(floatEqual(dist, 5.0f, 0.1f), "Plane distance calculation wrong");
    return true;
}

bool test_Plane_SetFromCenterNormal() {
    Plane plane;
    Vector3f center(0, 5, 0);
    Vector3f normal(0, 1, 0);
    plane.Set(center, normal);
    
    TEST_ASSERT(vec3Equal(plane.normal, normal), "Plane normal from center wrong");
    return true;
}

// ============================================================================
// Triangle Tests
// ============================================================================

bool test_Triangle2_Construction() {
    Triangle2f tri(Vector2f(0, 0), Vector2f(1, 0), Vector2f(0, 1));
    
    TEST_ASSERT(vec3Equal(Vector3f(tri[0], 0), Vector3f(0, 0, 0)), "Triangle2 vertex 0 wrong");
    TEST_ASSERT(vec3Equal(Vector3f(tri[1], 0), Vector3f(1, 0, 0)), "Triangle2 vertex 1 wrong");
    TEST_ASSERT(vec3Equal(Vector3f(tri[2], 0), Vector3f(0, 1, 0)), "Triangle2 vertex 2 wrong");
    return true;
}

bool test_Triangle2_PointIn() {
    Triangle2f tri(Vector2f(0, 0), Vector2f(2, 0), Vector2f(0, 2));
    
    TEST_ASSERT(tri.PointIn(Vector2f(0.5f, 0.5f)), "Point should be in triangle");
    TEST_ASSERT(!tri.PointIn(Vector2f(2, 2)), "Point should be outside triangle");
    
    return true;
}

bool test_Triangle3_Construction() {
    Triangle3f tri(Vector3f(0, 0, 0), Vector3f(1, 0, 0), Vector3f(0, 1, 0));
    
    TEST_ASSERT(vec3Equal(tri[0], Vector3f(0, 0, 0)), "Triangle3 vertex 0 wrong");
    TEST_ASSERT(vec3Equal(tri[1], Vector3f(1, 0, 0)), "Triangle3 vertex 1 wrong");
    TEST_ASSERT(vec3Equal(tri[2], Vector3f(0, 1, 0)), "Triangle3 vertex 2 wrong");
    return true;
}

// ============================================================================
// Integration Tests
// ============================================================================

bool test_Integration_AABB_OBB_Conversion() {
    AABB aabb;
    aabb.SetMinMax(Vector3f(-2, -3, -4), Vector3f(2, 3, 4));
    
    OBB obb = ToOBB(aabb);
    TEST_ASSERT(vec3Equal(obb.GetCenter(), aabb.GetCenter()), "AABB->OBB center mismatch");
    
    AABB aabb2 = ToAABB(obb);
    TEST_ASSERT(vec3Equal(aabb2.GetCenter(), aabb.GetCenter(), 0.01f), "OBB->AABB center mismatch");
    
    return true;
}

bool test_Integration_TransformChain() {
    AABB aabb;
    aabb.SetMinMax(Vector3f(-1, -1, -1), Vector3f(1, 1, 1));
    
    Matrix4f t1 = glm::translate(Matrix4f(1.0f), Vector3f(5, 5, 5));
    Matrix4f t2 = glm::scale(Matrix4f(1.0f), Vector3f(2, 2, 2));
    Matrix4f transform = t1 * t2;
    
    AABB transformed = aabb.Transformed(transform);
    TEST_ASSERT(!transformed.IsEmpty(), "Transformed AABB should not be empty");
    
    OBB obb = ToOBB(aabb);
    OBB obbTransformed = obb.Transformed(transform);
    TEST_ASSERT(!obbTransformed.IsEmpty(), "Transformed OBB should not be empty");
    
    return true;
}

bool test_Integration_BoundingHierarchy() {
    // 创建多个物体的包围盒
    AABB boxes[3];
    boxes[0].SetMinMax(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    boxes[1].SetMinMax(Vector3f(2, 0, 0), Vector3f(3, 1, 1));
    boxes[2].SetMinMax(Vector3f(0, 2, 0), Vector3f(1, 3, 1));
    
    // 合并所有包围盒
    AABB combined = boxes[0];
    combined.Merge(boxes[1]);
    combined.Merge(boxes[2]);
    
    TEST_ASSERT(vec3Equal(combined.GetMin(), Vector3f(0, 0, 0)), "Combined AABB min wrong");
    TEST_ASSERT(vec3Equal(combined.GetMax(), Vector3f(3, 3, 1)), "Combined AABB max wrong");
    
    return true;
}

// ============================================================================
// Edge Case Tests
// ============================================================================

bool test_EdgeCase_EmptyGeometry() {
    AABB aabb;
    aabb.Clear();
    TEST_ASSERT(aabb.IsEmpty(), "Cleared AABB should be empty");
    
    OBB obb;
    obb.Clear();
    TEST_ASSERT(obb.IsEmpty(), "Cleared OBB should be empty");
    
    BoundingSphere sphere;
    sphere.Clear();
    TEST_ASSERT(sphere.IsEmpty(), "Cleared sphere should be empty");
    
    return true;
}

bool test_EdgeCase_SinglePoint() {
    float point[] = {1.0f, 2.0f, 3.0f};
    
    AABB aabb;
    aabb.SetFromPoints(point, 1, 3);
    TEST_ASSERT(aabb.IsEmpty(), "Single point AABB should be empty");
    
    return true;
}

bool test_EdgeCase_NullPointers() {
    AABB aabb;
    aabb.SetFromPoints(nullptr, 0, 3);
    TEST_ASSERT(aabb.IsEmpty(), "AABB from null points should be empty");
    
    BoundingSphere sphere;
    sphere.SetFromPoints(nullptr, 0, 3);
    TEST_ASSERT(sphere.IsEmpty(), "Sphere from null points should be empty");
    
    return true;
}

bool test_EdgeCase_NegativeScale() {
    AABB aabb;
    aabb.SetMinMax(Vector3f(-1, -1, -1), Vector3f(1, 1, 1));
    
    Matrix4f transform = glm::scale(Matrix4f(1.0f), Vector3f(-1, 1, 1));
    AABB transformed = aabb.Transformed(transform);
    
    TEST_ASSERT(!transformed.IsEmpty(), "Negatively scaled AABB should not be empty");
    
    return true;
}

// ============================================================================
// Performance Tests
// ============================================================================

bool test_Performance_AABBManyMerges() {
    AABB base;
    base.SetMinMax(Vector3f(0, 0, 0), Vector3f(1, 1, 1));
    
    for(int i = 0; i < 1000; i++) {
        AABB other;
        other.SetMinMax(Vector3f(i, i, i), Vector3f(i+1, i+1, i+1));
        base.Merge(other);
    }
    
    TEST_ASSERT(!base.IsEmpty(), "Merged AABB should not be empty");
    TEST_ASSERT(vec3Equal(base.GetMin(), Vector3f(0, 0, 0)), "Merged AABB min wrong");
    TEST_ASSERT(vec3Equal(base.GetMax(), Vector3f(1000, 1000, 1000)), "Merged AABB max wrong");
    
    return true;
}

bool test_Performance_ManyPointsAABB() {
    std::vector<float> points;
    for(int i = 0; i < 10000; i++) {
        points.push_back(float(i % 100));
        points.push_back(float((i * 2) % 100));
        points.push_back(float((i * 3) % 100));
    }
    
    AABB aabb;
    aabb.SetFromPoints(points.data(), 10000, 3);
    
    TEST_ASSERT(!aabb.IsEmpty(), "AABB from many points should not be empty");
    
    return true;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    int total = 0, passed = 0, failed = 0;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Geometry Library Test Suite" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "=== AABB Tests ===" << std::endl;
    RUN_TEST(test_AABB_Construction);
    RUN_TEST(test_AABB_SetMinMax);
    RUN_TEST(test_AABB_SetCornerLength);
    RUN_TEST(test_AABB_SetFromPoints);
    RUN_TEST(test_AABB_Merge);
    RUN_TEST(test_AABB_IsEmpty);
    RUN_TEST(test_AABB_Transformed);
    RUN_TEST(test_AABB_GetVertexPN);
    
    std::cout << "\n=== OBB Tests ===" << std::endl;
    RUN_TEST(test_OBB_Construction);
    RUN_TEST(test_OBB_Set);
    RUN_TEST(test_OBB_SetWithAxes);
    RUN_TEST(test_OBB_GetCorners);
    RUN_TEST(test_OBB_SetFromPoints);
    RUN_TEST(test_OBB_Transformed);
    
    std::cout << "\n=== BoundingSphere Tests ===" << std::endl;
    RUN_TEST(test_BoundingSphere_Construction);
    RUN_TEST(test_BoundingSphere_SetFromPoints);
    RUN_TEST(test_BoundingSphere_Clear);
    
    std::cout << "\n=== BoundingVolumes Tests ===" << std::endl;
    RUN_TEST(test_BoundingVolumes_SetFromAABB);
    RUN_TEST(test_BoundingVolumes_SetFromPoints);
    RUN_TEST(test_BoundingVolumes_Conversions);
    RUN_TEST(test_BoundingVolumes_Pack);
    
    std::cout << "\n=== Frustum Tests ===" << std::endl;
    RUN_TEST(test_Frustum_SetMatrix);
    RUN_TEST(test_Frustum_PointIn);
    RUN_TEST(test_Frustum_SphereIn);
    RUN_TEST(test_Frustum_BoxIn);
    
    std::cout << "\n=== Sphere Tests ===" << std::endl;
    RUN_TEST(test_Sphere_Construction);
    RUN_TEST(test_Sphere_Set);
    RUN_TEST(test_Sphere_GetVolume);
    RUN_TEST(test_Sphere_GetPoint);
    RUN_TEST(test_EllipseSphere_Construction);
    RUN_TEST(test_EllipseSphere_GetVolume);
    
    std::cout << "\n=== Ray Tests ===" << std::endl;
    RUN_TEST(test_Ray_Construction);
    RUN_TEST(test_Ray_ConstructionWithParams);
    RUN_TEST(test_Ray_GetLengthPoint);
    RUN_TEST(test_Ray_ClosestPoint);
    RUN_TEST(test_Ray_ToPointDistance);
    RUN_TEST(test_Ray_CrossSphere);
    RUN_TEST(test_Ray_CrossCircle);
    
    std::cout << "\n=== LineSegment Tests ===" << std::endl;
    RUN_TEST(test_LineSegment_Construction);
    RUN_TEST(test_LineSegment_Distance);
    RUN_TEST(test_LineSegment_Direction);
    
    std::cout << "\n=== Plane Tests ===" << std::endl;
    RUN_TEST(test_Plane_Set);
    RUN_TEST(test_Plane_Distance);
    RUN_TEST(test_Plane_SetFromCenterNormal);
    
    std::cout << "\n=== Triangle Tests ===" << std::endl;
    RUN_TEST(test_Triangle2_Construction);
    RUN_TEST(test_Triangle2_PointIn);
    RUN_TEST(test_Triangle3_Construction);
    
    std::cout << "\n=== Integration Tests ===" << std::endl;
    RUN_TEST(test_Integration_AABB_OBB_Conversion);
    RUN_TEST(test_Integration_TransformChain);
    RUN_TEST(test_Integration_BoundingHierarchy);
    
    std::cout << "\n=== Edge Case Tests ===" << std::endl;
    RUN_TEST(test_EdgeCase_EmptyGeometry);
    RUN_TEST(test_EdgeCase_SinglePoint);
    RUN_TEST(test_EdgeCase_NullPointers);
    RUN_TEST(test_EdgeCase_NegativeScale);
    
    std::cout << "\n=== Performance Tests ===" << std::endl;
    RUN_TEST(test_Performance_AABBManyMerges);
    RUN_TEST(test_Performance_ManyPointsAABB);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total:  " << total << std::endl;
    std::cout << "Passed: " << passed << " (" << (total > 0 ? (passed * 100 / total) : 0) << "%)" << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
