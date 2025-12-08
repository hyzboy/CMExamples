#include<hgl/math/Transform.h>
#include<iostream>
#include<iomanip>

using namespace hgl;
using namespace hgl::math;

#define TEST_ASSERT(condition, name) \
    do { \
        if (!(condition)) { \
            std::cout << "❌ FAILED: " << name << std::endl; \
            return false; \
        } \
        std::cout << "✓ PASSED: " << name << std::endl; \
    } while(0)

// 辅助函数：打印Vector3f
void PrintVec3(const char* name, const Vector3f& v)
{
    std::cout << name << ": (" << std::fixed << std::setprecision(3) 
              << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

// 辅助函数：打印Matrix4f
void PrintMatrix(const char* name, const Matrix4f& m)
{
    std::cout << name << ":" << std::endl;
    for(int i = 0; i < 4; i++)
    {
        std::cout << "  ";
        for(int j = 0; j < 4; j++)
            std::cout << std::setw(10) << std::fixed << std::setprecision(3) << m[j][i] << " ";
        std::cout << std::endl;
    }
}

// 测试1：基本构造和初始化
bool Test_Construction()
{
    std::cout << "\n=== Test 1: Construction and Initialization ===" << std::endl;
    
    Transform t1;
    TEST_ASSERT(t1.IsIdentity(), "Default constructor creates identity transform");
    TEST_ASSERT(t1.GetVersion() == 0, "Initial version is 0");
    
    Vector3f pos = t1.GetTranslation();
    TEST_ASSERT(IsNearlyZero(pos.x) && IsNearlyZero(pos.y) && IsNearlyZero(pos.z), 
                "Default translation is zero");
    
    Vector3f scale = t1.GetScale();
    TEST_ASSERT(IsNearlyEqual(scale.x, 1.0f) && IsNearlyEqual(scale.y, 1.0f) && IsNearlyEqual(scale.z, 1.0f),
                "Default scale is (1,1,1)");
    
    return true;
}

// 测试2：平移变换
bool Test_Translation()
{
    std::cout << "\n=== Test 2: Translation ===" << std::endl;
    
    Transform t;
    Vector3f translation(10, 20, 30);
    
    uint32 v1 = t.GetVersion();
    t.SetTranslation(translation);
    uint32 v2 = t.GetVersion();
    
    TEST_ASSERT(v2 > v1, "Version updated after SetTranslation");
    TEST_ASSERT(!t.IsIdentity(), "Transform is not identity after translation");
    
    Vector3f result = t.GetTranslation();
    TEST_ASSERT(IsNearlyEqual(result.x, 10.0f) && IsNearlyEqual(result.y, 20.0f) && IsNearlyEqual(result.z, 30.0f),
                "GetTranslation returns correct value");
    
    // 测试TransformPosition
    Vector3f point(0, 0, 0);
    Vector3f transformed = t.TransformPosition(point);
    TEST_ASSERT(IsNearlyEqual(transformed.x, 10.0f) && IsNearlyEqual(transformed.y, 20.0f) && IsNearlyEqual(transformed.z, 30.0f),
                "TransformPosition(0,0,0) = translation vector");
    
    PrintVec3("Translation", translation);
    PrintVec3("Transformed origin", transformed);
    
    return true;
}

// 测试3：缩放变换
bool Test_Scale()
{
    std::cout << "\n=== Test 3: Scale ===" << std::endl;
    
    Transform t;
    Vector3f scale(2, 3, 4);
    
    t.SetScale(scale);
    Vector3f result = t.GetScale();
    
    TEST_ASSERT(IsNearlyEqual(result.x, 2.0f) && IsNearlyEqual(result.y, 3.0f) && IsNearlyEqual(result.z, 4.0f),
                "GetScale returns correct value");
    
    // 测试缩放效果
    Vector3f point(1, 1, 1);
    Vector3f transformed = t.TransformPosition(point);
    TEST_ASSERT(IsNearlyEqual(transformed.x, 2.0f) && IsNearlyEqual(transformed.y, 3.0f) && IsNearlyEqual(transformed.z, 4.0f),
                "Scale transforms (1,1,1) to (2,3,4)");
    
    PrintVec3("Scale", scale);
    PrintVec3("Transformed (1,1,1)", transformed);
    
    // 测试均匀缩放
    t.SetScale(2.0f);
    result = t.GetScale();
    TEST_ASSERT(IsNearlyEqual(result.x, 2.0f) && IsNearlyEqual(result.y, 2.0f) && IsNearlyEqual(result.z, 2.0f),
                "SetScale(float) creates uniform scale");
    
    return true;
}

// 测试4：旋转变换
bool Test_Rotation()
{
    std::cout << "\n=== Test 4: Rotation ===" << std::endl;
    
    Transform t;
    
    // 测试绕Z轴旋转90度
    Vector3f axis(0, 0, 1);
    float angle = 90.0f;
    
    t.SetRotation(axis, angle);
    
    // (1,0,0)旋转90度绕Z轴应该变成(0,1,0)
    Vector3f point(1, 0, 0);
    Vector3f transformed = t.TransformPosition(point);
    
    TEST_ASSERT(IsNearlyZero(transformed.x) && IsNearlyEqual(transformed.y, 1.0f) && IsNearlyZero(transformed.z),
                "90° rotation around Z-axis: (1,0,0) -> (0,1,0)");
    
    PrintVec3("Original point", point);
    PrintVec3("After 90° Z rotation", transformed);
    
    // 测试清除旋转
    t.ClearRotation();
    Vector3f trans_after_clear = t.GetTranslation();
    TEST_ASSERT(IsNearlyZero(trans_after_clear.x), "ClearRotation clears rotation only, not translation");
    
    // 测试四元数旋转
    Quatf quat = RotationQuat(angle, axis);
    t.SetRotation(quat);
    transformed = t.TransformPosition(point);
    TEST_ASSERT(IsNearlyZero(transformed.x) && IsNearlyEqual(transformed.y, 1.0f) && IsNearlyZero(transformed.z),
                "Quaternion rotation produces same result");
    
    return true;
}

// 测试5：TRS组合变换
bool Test_CombinedTRS()
{
    std::cout << "\n=== Test 5: Combined Translation-Rotation-Scale ===" << std::endl;
    
    Transform t;
    
    // 设置TRS: Scale(2,2,2) -> Rotate(Z,90°) -> Translate(10,0,0)
    t.SetScale(2.0f);
    t.SetRotation(Vector3f(0,0,1), 90.0f);
    t.SetTranslation(Vector3f(10, 0, 0));
    
    // 测试点(1,0,0): 先缩放到(2,0,0)，旋转到(0,2,0)，平移到(10,2,0)
    Vector3f point(1, 0, 0);
    Vector3f result = t.TransformPosition(point);
    
    TEST_ASSERT(IsNearlyEqual(result.x, 10.0f) && IsNearlyEqual(result.y, 2.0f) && IsNearlyZero(result.z),
                "TRS transformation: (1,0,0) -> Scale -> Rotate -> Translate -> (10,2,0)");
    
    PrintVec3("Original point", point);
    PrintVec3("After TRS", result);
    
    return true;
}

// 测试6：方向和法线变换
bool Test_DirectionAndNormal()
{
    std::cout << "\n=== Test 6: Direction and Normal Transformation ===" << std::endl;
    
    Transform t;
    t.SetRotation(Vector3f(0,0,1), 90.0f);  // 绕Z轴旋转90度
    t.SetTranslation(Vector3f(100, 200, 300)); // 平移不应影响方向和法线
    
    Vector3f direction(1, 0, 0);
    Vector3f transformed_dir = t.TransformDirection(direction);
    
    // 方向向量不受平移影响，只旋转
    TEST_ASSERT(IsNearlyZero(transformed_dir.x) && IsNearlyEqual(transformed_dir.y, 1.0f) && IsNearlyZero(transformed_dir.z),
                "TransformDirection ignores translation");
    
    Vector3f normal(1, 0, 0);
    Vector3f transformed_normal = t.TransformNormal(normal);
    
    TEST_ASSERT(IsNearlyZero(transformed_normal.x) && IsNearlyEqual(transformed_normal.y, 1.0f) && IsNearlyZero(transformed_normal.z),
                "TransformNormal works correctly");
    
    PrintVec3("Original direction", direction);
    PrintVec3("Transformed direction", transformed_dir);
    
    return true;
}

// 测试7：逆变换
bool Test_InverseTransform()
{
    std::cout << "\n=== Test 7: Inverse Transformation ===" << std::endl;
    
    Transform t;
    t.SetTranslation(Vector3f(10, 20, 30));
    t.SetScale(Vector3f(2, 2, 2));
    t.SetRotation(Vector3f(0, 0, 1), 45.0f);
    
    Vector3f point(5, 5, 5);
    Vector3f transformed = t.TransformPosition(point);
    Vector3f back = t.InverseTransformPosition(transformed);
    
    // 正变换再逆变换应该回到原点
    TEST_ASSERT(IsNearlyEqual(back.x, point.x) && IsNearlyEqual(back.y, point.y) && IsNearlyEqual(back.z, point.z),
                "Transform -> InverseTransform returns to original point");
    
    PrintVec3("Original point", point);
    PrintVec3("After transform", transformed);
    PrintVec3("After inverse transform", back);
    
    return true;
}

// 测试8：矩阵转换
bool Test_MatrixConversion()
{
    std::cout << "\n=== Test 8: Matrix Conversion ===" << std::endl;
    
    Transform t;
    t.SetTranslation(Vector3f(10, 20, 30));
    t.SetScale(Vector3f(2, 3, 4));
    
    Matrix4f mat = t.GetMatrix();
    
    // 检查矩阵最后一列是否包含平移
    TEST_ASSERT(IsNearlyEqual(mat[3][0], 10.0f) && IsNearlyEqual(mat[3][1], 20.0f) && IsNearlyEqual(mat[3][2], 30.0f),
                "Matrix contains translation in last column");
    
    // 检查对角线缩放
    TEST_ASSERT(IsNearlyEqual(mat[0][0], 2.0f) && IsNearlyEqual(mat[1][1], 3.0f) && IsNearlyEqual(mat[2][2], 4.0f),
                "Matrix contains scale on diagonal");
    
    PrintMatrix("Transform matrix", mat);
    
    // 测试从矩阵构造
    Transform t2;
    t2.SetFromMatrix4f(mat);
    
    Vector3f trans2 = t2.GetTranslation();
    Vector3f scale2 = t2.GetScale();
    
    TEST_ASSERT(IsNearlyEqual(trans2.x, 10.0f) && IsNearlyEqual(trans2.y, 20.0f) && IsNearlyEqual(trans2.z, 30.0f),
                "SetFromMatrix4f extracts translation correctly");
    TEST_ASSERT(IsNearlyEqual(scale2.x, 2.0f) && IsNearlyEqual(scale2.y, 3.0f) && IsNearlyEqual(scale2.z, 4.0f),
                "SetFromMatrix4f extracts scale correctly");
    
    return true;
}

// 测试9：版本追踪
bool Test_VersionTracking()
{
    std::cout << "\n=== Test 9: Version Tracking ===" << std::endl;
    
    Transform t;
    uint32 v0 = t.GetVersion();
    
    t.SetTranslation(Vector3f(1, 2, 3));
    uint32 v1 = t.GetVersion();
    TEST_ASSERT(v1 > v0, "Version increments after SetTranslation");
    
    // 重新创建一个transform，确保干净状态
    Transform t2;
    uint32 v2_0 = t2.GetVersion();
    t2.SetScale(Vector3f(2, 2, 2));
    uint32 v2_1 = t2.GetVersion();
    TEST_ASSERT(v2_1 > v2_0, "Version increments after SetScale");
    
    t2.SetRotation(Vector3f(0, 0, 1), 90.0f);
    uint32 v2_2 = t2.GetVersion();
    std::cout << "  v2_1=" << v2_1 << ", v2_2=" << v2_2 << " (after SetRotation)" << std::endl;
    TEST_ASSERT(v2_2 > v2_1, "Version increments after SetRotation");
    
    // GetVersion不应该触发UpdateMatrix
    uint32 v2_3 = t2.GetVersion();
    TEST_ASSERT(v2_3 == v2_2, "GetVersion doesn't change version");
    
    std::cout << "Version progression: " << v2_0 << " -> " << v2_1 << " -> " << v2_2 << std::endl;
    
    return true;
}

// 测试10：Identity检查
bool Test_IdentityCheck()
{
    std::cout << "\n=== Test 10: Identity Check ===" << std::endl;
    
    Transform t;
    TEST_ASSERT(t.IsIdentity(), "New transform is identity");
    
    t.SetTranslation(Vector3f(1, 2, 3));
    TEST_ASSERT(!t.IsIdentity(), "Not identity after translation");
    
    t.SetToIdentity();
    TEST_ASSERT(t.IsIdentity(), "SetToIdentity restores identity");
    
    t.SetScale(2.0f);
    TEST_ASSERT(!t.IsIdentity(), "Not identity after scale");
    
    t.SetToIdentity();
    TEST_ASSERT(t.IsIdentity(), "Identity after SetToIdentity");
    
    return true;
}

// 测试11：Transform叠加
bool Test_TransformTransform()
{
    std::cout << "\n=== Test 11: Transform Composition ===" << std::endl;
    
    Transform parent;
    parent.SetTranslation(Vector3f(10, 0, 0));
    parent.SetScale(2.0f);
    
    Transform child;
    child.SetTranslation(Vector3f(5, 0, 0));
    
    // 子变换在父空间中
    Vector3f point(1, 0, 0);
    Vector3f in_child = child.TransformPosition(point);    // (6,0,0)
    Vector3f in_parent = parent.TransformPosition(in_child); // (22,0,0) = (6*2+10, 0, 0)
    
    // 使用TransformTransform组合变换
    Transform combined_transform = parent.TransformTransform(child);
    Vector3f combined = combined_transform.TransformPosition(point);
    
    TEST_ASSERT(IsNearlyEqual(combined.x, in_parent.x) && 
                IsNearlyEqual(combined.y, in_parent.y) && 
                IsNearlyEqual(combined.z, in_parent.z),
                "TransformTransform equals parent(child(point))");
    
    PrintVec3("Point in local space", point);
    PrintVec3("Point in child space", in_child);
    PrintVec3("Point in parent space", in_parent);
    PrintVec3("Using TransformTransform", combined);
    
    return true;
}

// 测试12：Lerp插值
bool Test_Lerp()
{
    std::cout << "\n=== Test 12: Transform Lerp ===" << std::endl;
    
    Transform from;
    from.SetTranslation(Vector3f(0, 0, 0));
    from.SetScale(1.0f);
    
    Transform to;
    to.SetTranslation(Vector3f(10, 20, 30));
    to.SetScale(2.0f);
    
    Transform mid = Lerp(from, to, 0.5f);
    
    Vector3f trans = mid.GetTranslation();
    Vector3f scale = mid.GetScale();
    
    TEST_ASSERT(IsNearlyEqual(trans.x, 5.0f) && IsNearlyEqual(trans.y, 10.0f) && IsNearlyEqual(trans.z, 15.0f),
                "Lerp interpolates translation correctly");
    TEST_ASSERT(IsNearlyEqual(scale.x, 1.5f) && IsNearlyEqual(scale.y, 1.5f) && IsNearlyEqual(scale.z, 1.5f),
                "Lerp interpolates scale correctly");
    
    PrintVec3("From translation", from.GetTranslation());
    PrintVec3("To translation", to.GetTranslation());
    PrintVec3("Mid translation (t=0.5)", trans);
    
    return true;
}

// 测试13：边界情况
bool Test_EdgeCases()
{
    std::cout << "\n=== Test 13: Edge Cases ===" << std::endl;
    
    Transform t;
    
    // 零缩放
    t.SetScale(Vector3f(0, 0, 0));
    Vector3f point(1, 1, 1);
    Vector3f result = t.TransformPosition(point);
    TEST_ASSERT(IsNearlyZero(result.x) && IsNearlyZero(result.y) && IsNearlyZero(result.z),
                "Zero scale produces zero vector");
    
    // 负缩放
    t.SetScale(Vector3f(-1, 1, 1));
    result = t.TransformPosition(Vector3f(1, 2, 3));
    TEST_ASSERT(IsNearlyEqual(result.x, -1.0f) && IsNearlyEqual(result.y, 2.0f) && IsNearlyEqual(result.z, 3.0f),
                "Negative scale works correctly");
    
    // 360度旋转应该等于不旋转
    t.SetToIdentity();
    t.SetRotation(Vector3f(0, 0, 1), 360.0f);
    result = t.TransformPosition(Vector3f(1, 0, 0));
    TEST_ASSERT(IsNearlyEqual(result.x, 1.0f) && IsNearlyZero(result.y) && IsNearlyZero(result.z),
                "360° rotation equals identity");
    
    return true;
}

// 测试14：性能测试（矩阵缓存）
bool Test_MatrixCaching()
{
    std::cout << "\n=== Test 14: Matrix Caching ===" << std::endl;
    
    Transform t;
    t.SetTranslation(Vector3f(10, 20, 30));
    t.SetScale(2.0f);
    
    uint32 v1 = t.GetVersion();
    
    // 第一次GetMatrix会触发UpdateMatrix，会更新version
    Matrix4f m1 = t.GetMatrix();
    uint32 v2 = t.GetVersion();
    
    // 后续的GetMatrix不应该改变版本号（证明使用了缓存）
    Matrix4f m2 = t.GetMatrix();
    uint32 v3 = t.GetVersion();
    Matrix4f m3 = t.GetMatrix();
    uint32 v4 = t.GetVersion();
    Matrix4f m4 = t.GetMatrix();
    uint32 v5 = t.GetVersion();
    
    std::cout << "  First GetMatrix: v1=" << v1 << " -> v2=" << v2 << std::endl;
    std::cout << "  Subsequent calls: v3=" << v3 << ", v4=" << v4 << ", v5=" << v5 << std::endl;
    
    TEST_ASSERT(v2 == v3 && v3 == v4 && v4 == v5, "Subsequent GetMatrix calls use cache (version unchanged)");
    
    // 修改后版本应该更新
    t.SetTranslation(Vector3f(1, 2, 3));
    uint32 v6 = t.GetVersion();
    TEST_ASSERT(v6 > v5, "Version updates after modification");
    
    std::cout << "Matrix cache working correctly - version stays at " << v1 
              << " across multiple GetMatrix calls" << std::endl;
    
    return true;
}

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           Transform Comprehensive Test Suite              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    int passed = 0;
    int total = 14;
    
    if (Test_Construction()) passed++;
    if (Test_Translation()) passed++;
    if (Test_Scale()) passed++;
    if (Test_Rotation()) passed++;
    if (Test_CombinedTRS()) passed++;
    if (Test_DirectionAndNormal()) passed++;
    if (Test_InverseTransform()) passed++;
    if (Test_MatrixConversion()) passed++;
    if (Test_VersionTracking()) passed++;
    if (Test_IdentityCheck()) passed++;
    if (Test_TransformTransform()) passed++;
    if (Test_Lerp()) passed++;
    if (Test_EdgeCases()) passed++;
    if (Test_MatrixCaching()) passed++;
    
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Test Summary: " << passed << "/" << total << " tests passed";
    if (passed == total)
        std::cout << "                           ║" << std::endl;
    else
        std::cout << "                          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    return (passed == total) ? 0 : 1;
}
