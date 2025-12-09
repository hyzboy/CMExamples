/**
 * 坐标系测试程序 - 验证Vulkan Z-up坐标系实现
 * 
 * 测试内容：
 * 1. 投影矩阵生成
 * 2. 视图矩阵生成
 * 3. 屏幕投影/反投影
 * 4. 射线生成
 * 5. 视锥体裁剪
 */

#include<hgl/math/MatrixTypes.h>
#include<hgl/math/VectorTypes.h>
#include<hgl/math/VectorOperations.h>
#include<hgl/math/Projection.h>
#include<hgl/math/MatrixOperations.h>
#include<hgl/math/geometry/Frustum.h>
#include<hgl/math/geometry/Ray.h>
#include<hgl/math/geometry/Coordinate.h>
#include<hgl/graph/CameraInfo.h>
#include<iostream>
#include<iomanip>

using namespace hgl;
using namespace hgl::math;
using namespace hgl::graph;

// 测试辅助宏
#define TEST_ASSERT(condition) \
    if (!(condition)) { \
        std::cout << "  ✗ 断言失败: " #condition << " (行 " << __LINE__ << ")" << std::endl; \
        ++test_failed; \
    } else { \
        ++test_passed; \
    }

#define RUN_TEST(test_func) \
    std::cout << "\n测试: " #test_func << std::endl; \
    test_func(); \
    ++test_total;

int test_passed = 0;
int test_failed = 0;
int test_total = 0;

// 打印向量
void PrintVec3(const char* name, const Vector3f& v) {
    std::cout << "  " << name << ": (" 
              << std::fixed << std::setprecision(3)
              << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

void PrintVec4(const char* name, const Vector4f& v) {
    std::cout << "  " << name << ": (" 
              << std::fixed << std::setprecision(3)
              << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")" << std::endl;
}

// 测试1: 透视投影矩阵基本性质
void test_PerspectiveMatrix()
{
    std::cout << "验证Vulkan Z-up透视投影矩阵..." << std::endl;
    
    Matrix4f proj = PerspectiveMatrix(90.0f, 1.0f, 0.1f, 100.0f);
    
    // 测试中心点 (0,0,znear) 应该投影到NDC (0,0,0)
    Vector4f center_near(0, 0, 0.1f, 1);
    Vector4f clip = proj * center_near;
    Vector3f ndc(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
    
    std::cout << "  近平面中心点 -> NDC: ";
    PrintVec3("", ndc);
    
    TEST_ASSERT(abs(ndc.x) < 0.001f);
    TEST_ASSERT(abs(ndc.y) < 0.001f);
    TEST_ASSERT(abs(ndc.z - 0.0f) < 0.001f);  // Vulkan: znear -> z=0
    
    // 测试远平面中心点
    Vector4f center_far(0, 100.0f, 0, 1);
    clip = proj * center_far;
    ndc = Vector3f(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
    
    std::cout << "  远平面中心点 -> NDC: ";
    PrintVec3("", ndc);
    
    TEST_ASSERT(abs(ndc.z - 1.0f) < 0.01f);  // Vulkan: zfar -> z=1
}

// 测试2: LookAt矩阵 (Z-up系统)
void test_LookAtMatrix()
{
    std::cout << "验证Vulkan Z-up视图矩阵..." << std::endl;
    
    // 摄像机在原点，看向+Y方向，Z轴向上
    Vector3f eye(0, 0, 0);
    Vector3f target(0, 10, 0);  // 朝+Y看
    Vector3f up(0, 0, 1);       // Z轴向上
    
    Matrix4f view = LookAtMatrix(eye, target, up);
    
    // 测试：+Y方向的点应该在摄像机前方
    Vector4f point_forward(0, 5, 0, 1);
    Vector4f view_space = view * point_forward;
    
    PrintVec4("前方点(0,5,0)在视图空间", view_space);
    
    // 在Z-up系统中，摄像机朝+Y，所以点(0,5,0)应该在视图空间的+Y方向
    TEST_ASSERT(view_space.y > 0);  // 在摄像机前方
    
    // 测试：+Z方向的点应该在摄像机上方
    Vector4f point_up(0, 0, 5, 1);
    view_space = view * point_up;
    
    PrintVec4("上方点(0,0,5)在视图空间", view_space);
    
    TEST_ASSERT(view_space.z > 0);  // 在摄像机上方
}

// 测试3: 正交投影矩阵
void test_OrthoMatrix()
{
    std::cout << "验证Vulkan正交投影矩阵..." << std::endl;
    
    Matrix4f ortho = OrthoMatrix(800.0f, 600.0f, 0.0f, 1.0f);
    
    // 测试左上角 (0,0)
    Vector4f top_left(0, 0, 0.5f, 1);
    Vector4f clip = ortho * top_left;
    Vector3f ndc(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
    
    PrintVec3("左上角(0,0,0.5) -> NDC", ndc);
    
    // Vulkan: (0,0)应该在NDC的(-1,-1)（左上角）
    TEST_ASSERT(abs(ndc.x - (-1.0f)) < 0.01f);
    TEST_ASSERT(abs(ndc.y - (-1.0f)) < 0.01f);
    
    // 测试右下角 (800,600)
    Vector4f bottom_right(800, 600, 0.5f, 1);
    clip = ortho * bottom_right;
    ndc = Vector3f(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
    
    PrintVec3("右下角(800,600,0.5) -> NDC", ndc);
    
    // Vulkan: (800,600)应该在NDC的(+1,+1)（右下角）
    TEST_ASSERT(abs(ndc.x - 1.0f) < 0.01f);
    TEST_ASSERT(abs(ndc.y - 1.0f) < 0.01f);
}

// 测试4: 屏幕投影和反投影
void test_ProjectUnProject()
{
    std::cout << "验证屏幕投影/反投影一致性..." << std::endl;
    
    Vector3f eye(0, -5, 2);
    Vector3f target(0, 0, 2);
    Vector3f up(0, 0, 1);
    
    Matrix4f view = LookAtMatrix(eye, target, up);
    Matrix4f proj = PerspectiveMatrix(90.0f, 800.0f/600.0f, 0.1f, 100.0f);
    Vector2u vp_size(800, 600);
    
    // 测试世界空间点
    Vector3f world_point(1, 0, 2);
    
    // 投影到屏幕
    Vector2i screen = ProjectToScreen(world_point, view, proj, vp_size);
    std::cout << "  世界点(1,0,2) -> 屏幕坐标: (" << screen.x << ", " << screen.y << ")" << std::endl;
    
    // 反投影回世界（注意：只能恢复射线方向，不是原点）
    Vector3f unprojected = UnProjectToWorld(screen, view, proj, vp_size);
    PrintVec3("屏幕坐标 -> 近平面点", unprojected);
    
    // 验证反投影的点应该在从摄像机到原点的射线上
    Vector3f ray_dir = normalize(world_point - eye);
    Vector3f unproj_dir = normalize(unprojected - eye);
    
    float dot_product = dot(ray_dir, unproj_dir);
    std::cout << "  射线方向相似度: " << dot_product << std::endl;
    
    TEST_ASSERT(dot_product > 0.99f);  // 方向应该非常接近
}

// 测试5: 射线生成
void test_RayGeneration()
{
    std::cout << "验证射线生成..." << std::endl;
    
    // 设置摄像机
    CameraInfo ci;
    ci.pos = Vector3f(0, -5, 2);
    ci.view_line = normalize(Vector3f(0, 5, -2) - ci.pos);
    ci.world_up = Vector3f(0, 0, 1);
    
    ci.view = LookAtMatrix(ci.pos, ci.pos + ci.view_line * 5.0f, ci.world_up);
    ci.projection = PerspectiveMatrix(90.0f, 800.0f/600.0f, 0.1f, 100.0f);
    ci.vp = ci.projection * ci.view;
    ci.inverse_vp = inverse(ci.vp);
    
    Vector2u vp_size(800, 600);
    
    // 测试屏幕中心点的射线
    Vector2i center_screen(400, 300);
    Ray ray;
    ray.SetFromViewportPoint(center_screen, &ci, vp_size);
    
    PrintVec3("射线起点", ray.GetOrigin());
    PrintVec3("射线方向", ray.GetDirection());
    
    // 射线方向应该大致朝向+Y方向（摄像机朝向）
    TEST_ASSERT(ray.GetDirection().y > 0.5f);
    
    // 射线起点应该接近摄像机位置
    float dist_to_camera = length(ray.GetOrigin() - ci.pos);
    std::cout << "  射线起点到摄像机距离: " << dist_to_camera << std::endl;
    TEST_ASSERT(dist_to_camera < 0.2f);  // 应该在近平面上
}

// 测试6: 视锥体裁剪
void test_FrustumCulling()
{
    std::cout << "验证视锥体裁剪..." << std::endl;
    
    Vector3f eye(0, -5, 2);
    Vector3f target(0, 0, 2);
    Vector3f up(0, 0, 1);
    
    Matrix4f view = LookAtMatrix(eye, target, up);
    Matrix4f proj = PerspectiveMatrix(90.0f, 1.0f, 0.1f, 100.0f);
    Matrix4f vp = proj * view;
    
    Frustum frustum;
    frustum.SetMatrix(vp);
    
    // 测试摄像机前方的点（应该在内部）
    Vector3f point_front(0, 0, 2);
    auto result = frustum.PointIn(point_front);
    std::cout << "  前方点(0,0,2): " << (result == Frustum::Scope::INSIDE ? "内部" : 
                                         result == Frustum::Scope::OUTSIDE ? "外部" : "相交") << std::endl;
    TEST_ASSERT(result != Frustum::Scope::OUTSIDE);
    
    // 测试摄像机后方的点（应该在外部）
    Vector3f point_back(0, -10, 2);
    result = frustum.PointIn(point_back);
    std::cout << "  后方点(0,-10,2): " << (result == Frustum::Scope::INSIDE ? "内部" : 
                                          result == Frustum::Scope::OUTSIDE ? "外部" : "相交") << std::endl;
    TEST_ASSERT(result == Frustum::Scope::OUTSIDE);
}

// 测试7: 坐标系转换
void test_CoordinateConversion()
{
    std::cout << "验证坐标系转换函数..." << std::endl;
    
    // 测试OpenGL Y-up到Vulkan Z-up的转换
    Vector3f gl_point(1, 2, -3);  // OpenGL: X右, Y上, Z后
    Vector3f vk_point = glYUp2vkZUp(gl_point);
    
    PrintVec3("OpenGL Y-up (1,2,-3)", gl_point);
    PrintVec3("Vulkan Z-up", vk_point);
    
    // 预期：GL.x->VK.x, GL.y->VK.z, GL.z->-VK.y
    TEST_ASSERT(abs(vk_point.x - 1.0f) < 0.001f);
    TEST_ASSERT(abs(vk_point.y - 3.0f) < 0.001f);  // GL的-Z变成VK的+Y
    TEST_ASSERT(abs(vk_point.z - 2.0f) < 0.001f);  // GL的Y变成VK的Z
    
    // 测试逆变换
    Vector3f gl_back = vkZUp2glYUp(vk_point);
    PrintVec3("转换回OpenGL Y-up", gl_back);
    
    TEST_ASSERT(IsNearlyEqual(gl_point, gl_back, 0.001f));
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "Vulkan Z-up 坐标系测试" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n坐标系约定：" << std::endl;
    std::cout << "  世界空间: X右, Y前(摄像机朝向), Z上" << std::endl;
    std::cout << "  Vulkan NDC: x右[-1,1], y下[-1,1], z前[0,1]" << std::endl;
    std::cout << "  屏幕空间: 左上角(0,0), x右, y下" << std::endl;
    std::cout << "========================================" << std::endl;

    RUN_TEST(test_PerspectiveMatrix);
    RUN_TEST(test_LookAtMatrix);
    RUN_TEST(test_OrthoMatrix);
    RUN_TEST(test_ProjectUnProject);
    RUN_TEST(test_RayGeneration);
    RUN_TEST(test_FrustumCulling);
    RUN_TEST(test_CoordinateConversion);

    std::cout << "\n========================================" << std::endl;
    std::cout << "测试总结" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "总计测试: " << test_total << std::endl;
    std::cout << "通过: " << test_passed << " ✓" << std::endl;
    std::cout << "失败: " << test_failed << " ✗" << std::endl;

    if (test_failed == 0) {
        std::cout << "\n🎉 所有测试通过！" << std::endl;
    } else {
        std::cout << "\n⚠️  部分测试失败，请检查实现" << std::endl;
    }

    std::cout << "========================================" << std::endl;

    return test_failed > 0 ? 1 : 0;
}
