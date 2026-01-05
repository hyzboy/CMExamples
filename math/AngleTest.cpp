/**
 * AngleTest.cpp - Angle 类型安全角度测试
 * 
 * 测试内容：
 * 1. 构造函数和转换
 * 2. 角度和弧度互转
 * 3. 算术运算
 * 4. 比较运算
 * 5. 字面量支持
 * 6. 边界条件
 */

#include<hgl/math/Angle.h>
#include<iostream>
#include<iomanip>
#include<cmath>

using namespace hgl::math;
using namespace hgl::math::angle_literals;

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

// 浮点数比较辅助函数
bool nearly_equal(double a, double b, double epsilon = 1e-10) {
    return std::abs(a - b) < epsilon;
}

// 测试1: 基本构造和转换
void test_BasicConstruction()
{
    std::cout << "验证角度的基本构造和类型转换..." << std::endl;
    
    // 默认构造
    Angle a1;
    TEST_ASSERT(a1.AsRadians() == 0.0);
    TEST_ASSERT(a1.AsDegrees() == 0.0);
    
    // 从弧度构造 - 使用统一初始化避免 Most Vexing Parse
    Angle a2{Radians(std::numbers::pi_v<float>)};
    TEST_ASSERT(nearly_equal(a2.AsRadians(), std::numbers::pi_v<float>));
    TEST_ASSERT(nearly_equal(a2.AsDegrees(), 180.0));
    
    // 从角度构造
    Angle a3{Degrees(90.0)};
    TEST_ASSERT(nearly_equal(a3.AsRadians(), std::numbers::pi_v<float> / 2.0));
    TEST_ASSERT(nearly_equal(a3.AsDegrees(), 90.0));
    
    std::cout << "  π rad = " << a2.AsDegrees() << "°" << std::endl;
    std::cout << "  90° = " << a3.AsRadians() << " rad" << std::endl;
}

// 测试2: 静态工厂方法
void test_StaticFactoryMethods()
{
    std::cout << "验证静态工厂方法..." << std::endl;
    
    Angle from_rad = Angle::FromRadians(std::numbers::pi_v<float> / 4.0);
    TEST_ASSERT(nearly_equal(from_rad.AsDegrees(), 45.0));
    
    Angle from_deg = Angle::FromDegrees(180.0);
    TEST_ASSERT(nearly_equal(from_deg.AsRadians(), std::numbers::pi_v<float>));
    
    std::cout << "  FromRadians(π/4) = " << from_rad.AsDegrees() << "°" << std::endl;
    std::cout << "  FromDegrees(180) = " << from_deg.AsRadians() << " rad" << std::endl;
}

// 测试3: 算术运算 - 加法
void test_Addition()
{
    std::cout << "验证角度加法..." << std::endl;
    
    Angle a1 = Angle::FromDegrees(30.0);
    Angle a2 = Angle::FromDegrees(60.0);
    Angle sum = a1 + a2;
    
    TEST_ASSERT(nearly_equal(sum.AsDegrees(), 90.0));
    
    // 复合赋值
    Angle a3 = Angle::FromDegrees(45.0);
    a3 += Angle::FromDegrees(45.0);
    TEST_ASSERT(nearly_equal(a3.AsDegrees(), 90.0));
    
    std::cout << "  30° + 60° = " << sum.AsDegrees() << "°" << std::endl;
    std::cout << "  45° += 45° = " << a3.AsDegrees() << "°" << std::endl;
}

// 测试4: 算术运算 - 减法
void test_Subtraction()
{
    std::cout << "验证角度减法..." << std::endl;
    
    Angle a1 = Angle::FromDegrees(90.0);
    Angle a2 = Angle::FromDegrees(30.0);
    Angle diff = a1 - a2;
    
    TEST_ASSERT(nearly_equal(diff.AsDegrees(), 60.0));
    
    // 复合赋值
    Angle a3 = Angle::FromDegrees(180.0);
    a3 -= Angle::FromDegrees(90.0);
    TEST_ASSERT(nearly_equal(a3.AsDegrees(), 90.0));
    
    std::cout << "  90° - 30° = " << diff.AsDegrees() << "°" << std::endl;
    std::cout << "  180° -= 90° = " << a3.AsDegrees() << "°" << std::endl;
}

// 测试5: 算术运算 - 标量乘法
void test_ScalarMultiplication()
{
    std::cout << "验证角度标量乘法..." << std::endl;
    
    Angle a1 = Angle::FromDegrees(30.0);
    Angle doubled = a1 * 2.0;
    Angle tripled = 3.0 * a1;
    
    TEST_ASSERT(nearly_equal(doubled.AsDegrees(), 60.0));
    TEST_ASSERT(nearly_equal(tripled.AsDegrees(), 90.0));
    
    // 复合赋值
    Angle a2 = Angle::FromDegrees(45.0);
    a2 *= 2.0;
    TEST_ASSERT(nearly_equal(a2.AsDegrees(), 90.0));
    
    std::cout << "  30° × 2 = " << doubled.AsDegrees() << "°" << std::endl;
    std::cout << "  3 × 30° = " << tripled.AsDegrees() << "°" << std::endl;
    std::cout << "  45° *= 2 = " << a2.AsDegrees() << "°" << std::endl;
}

// 测试6: 算术运算 - 标量除法
void test_ScalarDivision()
{
    std::cout << "验证角度标量除法..." << std::endl;
    
    Angle a1 = Angle::FromDegrees(180.0);
    Angle halved = a1 / 2.0;
    
    TEST_ASSERT(nearly_equal(halved.AsDegrees(), 90.0));
    
    // 复合赋值
    Angle a2 = Angle::FromDegrees(360.0);
    a2 /= 4.0;
    TEST_ASSERT(nearly_equal(a2.AsDegrees(), 90.0));
    
    std::cout << "  180° / 2 = " << halved.AsDegrees() << "°" << std::endl;
    std::cout << "  360° /= 4 = " << a2.AsDegrees() << "°" << std::endl;
}

// 测试7: 比较运算
void test_ComparisonOperators()
{
    std::cout << "验证角度比较运算..." << std::endl;
    
    Angle a1 = Angle::FromDegrees(30.0);
    Angle a2 = Angle::FromDegrees(30.0);
    Angle a3 = Angle::FromDegrees(60.0);
    
    // 相等性
    TEST_ASSERT(a1 == a2);
    TEST_ASSERT(a1 != a3);
    
    // 大小比较
    TEST_ASSERT(a1 < a3);
    TEST_ASSERT(a3 > a1);
    TEST_ASSERT(a1 <= a2);
    TEST_ASSERT(a1 <= a3);
    TEST_ASSERT(a3 >= a1);
    TEST_ASSERT(a2 >= a1);
    
    std::cout << "  30° == 30°: " << (a1 == a2 ? "真" : "假") << std::endl;
    std::cout << "  30° < 60°: " << (a1 < a3 ? "真" : "假") << std::endl;
    std::cout << "  60° > 30°: " << (a3 > a1 ? "真" : "假") << std::endl;
}

// 测试8: 字面量
void test_Literals()
{
    std::cout << "验证用户定义字面量..." << std::endl;
    
    // 弧度字面量
    auto angle_rad1 = 3.14159_rad;
    TEST_ASSERT(nearly_equal(angle_rad1.AsRadians(), 3.14159, 1e-5));
    TEST_ASSERT(nearly_equal(angle_rad1.AsDegrees(), 180.0, 0.01));
    
    auto angle_rad2 = 1_rad;
    TEST_ASSERT(nearly_equal(angle_rad2.AsRadians(), 1.0));
    
    // 角度字面量
    auto angle_deg1 = 90.0_deg;
    TEST_ASSERT(nearly_equal(angle_deg1.AsDegrees(), 90.0));
    TEST_ASSERT(nearly_equal(angle_deg1.AsRadians(), std::numbers::pi_v<float> / 2.0));
    
    auto angle_deg2 = 180_deg;
    TEST_ASSERT(nearly_equal(angle_deg2.AsDegrees(), 180.0));
    
    std::cout << "  3.14159_rad = " << angle_rad1.AsDegrees() << "°" << std::endl;
    std::cout << "  90.0_deg = " << angle_deg1.AsRadians() << " rad" << std::endl;
    std::cout << "  180_deg = " << angle_deg2.AsRadians() << " rad" << std::endl;
}

// 测试9: 常用角度值
void test_CommonAngles()
{
    std::cout << "验证常用角度值的精度..." << std::endl;
    
    struct TestCase {
        double degrees;
        double radians;
        const char* name;
    };
    
    TestCase cases[] = {
        {0.0,   0.0,              "0°"},
        {30.0,  std::numbers::pi_v<float> / 6.0,     "30°"},
        {45.0,  std::numbers::pi_v<float> / 4.0,     "45°"},
        {60.0,  std::numbers::pi_v<float> / 3.0,     "60°"},
        {90.0,  std::numbers::pi_v<float> / 2.0,     "90°"},
        {120.0, 2.0 * std::numbers::pi_v<float> / 3.0, "120°"},
        {135.0, 3.0 * std::numbers::pi_v<float> / 4.0, "135°"},
        {180.0, std::numbers::pi_v<float>,           "180°"},
        {270.0, 3.0 * std::numbers::pi_v<float> / 2.0, "270°"},
        {360.0, 2.0 * std::numbers::pi_v<float>,     "360°"}
    };
    
    for (const auto& tc : cases) {
        Angle from_deg = Angle::FromDegrees(tc.degrees);
        Angle from_rad = Angle::FromRadians(tc.radians);
        
        TEST_ASSERT(nearly_equal(from_deg.AsRadians(), tc.radians));
        TEST_ASSERT(nearly_equal(from_rad.AsDegrees(), tc.degrees));
        
        std::cout << "  " << tc.name << " = " 
                  << std::fixed << std::setprecision(6)
                  << from_deg.AsRadians() << " rad" << std::endl;
    }
}

// 测试10: 负角度
void test_NegativeAngles()
{
    std::cout << "验证负角度..." << std::endl;
    
    Angle neg_deg = Angle::FromDegrees(-90.0);
    TEST_ASSERT(nearly_equal(neg_deg.AsDegrees(), -90.0));
    TEST_ASSERT(nearly_equal(neg_deg.AsRadians(), -std::numbers::pi_v<float> / 2.0));
    
    Angle neg_rad = Angle::FromRadians(-std::numbers::pi_v<float>);
    TEST_ASSERT(nearly_equal(neg_rad.AsRadians(), -std::numbers::pi_v<float>));
    TEST_ASSERT(nearly_equal(neg_rad.AsDegrees(), -180.0));
    
    // 负角度运算
    Angle a1 = Angle::FromDegrees(30.0);
    Angle a2 = Angle::FromDegrees(-30.0);
    Angle sum = a1 + a2;
    TEST_ASSERT(nearly_equal(sum.AsDegrees(), 0.0));
    
    std::cout << "  -90° = " << neg_deg.AsRadians() << " rad" << std::endl;
    std::cout << "  -π rad = " << neg_rad.AsDegrees() << "°" << std::endl;
    std::cout << "  30° + (-30°) = " << sum.AsDegrees() << "°" << std::endl;
}

// 测试11: 大角度
void test_LargeAngles()
{
    std::cout << "验证大角度（超过360°）..." << std::endl;
    
    Angle large1 = Angle::FromDegrees(720.0);
    TEST_ASSERT(nearly_equal(large1.AsDegrees(), 720.0));
    TEST_ASSERT(nearly_equal(large1.AsRadians(), 4.0 * std::numbers::pi_v<float>));
    
    Angle large2 = Angle::FromDegrees(450.0);
    TEST_ASSERT(nearly_equal(large2.AsDegrees(), 450.0));
    
    // 大角度运算
    Angle a1 = Angle::FromDegrees(270.0);
    Angle a2 = Angle::FromDegrees(180.0);
    Angle sum = a1 + a2;
    TEST_ASSERT(nearly_equal(sum.AsDegrees(), 450.0));
    
    std::cout << "  720° = " << large1.AsRadians() << " rad (2圈)" << std::endl;
    std::cout << "  450° = " << large2.AsRadians() << " rad" << std::endl;
    std::cout << "  270° + 180° = " << sum.AsDegrees() << "°" << std::endl;
}

// 测试12: 混合运算
void test_MixedOperations()
{
    std::cout << "验证混合运算..." << std::endl;
    
    // 使用字面量进行复杂计算
    auto angle1 = 45.0_deg;
    auto angle2 = Angle::FromRadians(std::numbers::pi_v<float> / 4.0);
    auto sum = angle1 + angle2;
    
    TEST_ASSERT(nearly_equal(sum.AsDegrees(), 90.0));
    
    // 复杂表达式
    auto result = (90.0_deg + 45.0_deg) * 2.0 - 180.0_deg;
    TEST_ASSERT(nearly_equal(result.AsDegrees(), 90.0));
    
    // 链式运算
    auto chained = Angle::FromDegrees(0.0);
    chained += 30.0_deg;
    chained += 30.0_deg;
    chained += 30.0_deg;
    TEST_ASSERT(nearly_equal(chained.AsDegrees(), 90.0));
    
    std::cout << "  45° + π/4 rad = " << sum.AsDegrees() << "°" << std::endl;
    std::cout << "  (90° + 45°) × 2 - 180° = " << result.AsDegrees() << "°" << std::endl;
    std::cout << "  0° + 30° + 30° + 30° = " << chained.AsDegrees() << "°" << std::endl;
}

// 测试13: constexpr 支持
void test_ConstexprSupport()
{
    std::cout << "验证 constexpr 编译期计算..." << std::endl;
    
    // 编译期常量
    constexpr Angle right_angle = Angle::FromDegrees(90.0);
    constexpr double rad_value = right_angle.AsRadians();
    constexpr double deg_value = right_angle.AsDegrees();
    
    TEST_ASSERT(nearly_equal(deg_value, 90.0));
    TEST_ASSERT(nearly_equal(rad_value, std::numbers::pi_v<float> / 2.0));
    
    // 编译期运算
    constexpr Angle sum = right_angle + right_angle;
    constexpr double sum_deg = sum.AsDegrees();
    TEST_ASSERT(nearly_equal(sum_deg, 180.0));
    
    std::cout << "  constexpr 90° = " << rad_value << " rad" << std::endl;
    std::cout << "  constexpr 90° + 90° = " << sum_deg << "°" << std::endl;
    std::cout << "  ✓ 所有计算在编译期完成" << std::endl;
}

// 测试14: 精度测试
void test_Precision()
{
    std::cout << "验证转换精度..." << std::endl;
    
    // 往返转换
    double original_deg = 123.456789;
    Angle a = Angle::FromDegrees(original_deg);
    double converted_deg = a.AsDegrees();
    
    TEST_ASSERT(nearly_equal(original_deg, converted_deg, 1e-9));
    
    // 弧度往返
    double original_rad = 2.1234567890;
    Angle b = Angle::FromRadians(original_rad);
    double converted_rad = b.AsRadians();
    
    TEST_ASSERT(nearly_equal(original_rad, converted_rad, 1e-12));
    
    std::cout << "  度数往返精度: " 
              << std::scientific << std::setprecision(12)
              << std::abs(original_deg - converted_deg) << std::endl;
    std::cout << "  弧度往返精度: " 
              << std::abs(original_rad - converted_rad) << std::endl;
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "Angle 类型安全角度测试" << std::endl;
    std::cout << "========================================" << std::endl;

    RUN_TEST(test_BasicConstruction);
    RUN_TEST(test_StaticFactoryMethods);
    RUN_TEST(test_Addition);
    RUN_TEST(test_Subtraction);
    RUN_TEST(test_ScalarMultiplication);
    RUN_TEST(test_ScalarDivision);
    RUN_TEST(test_ComparisonOperators);
    RUN_TEST(test_Literals);
    RUN_TEST(test_CommonAngles);
    RUN_TEST(test_NegativeAngles);
    RUN_TEST(test_LargeAngles);
    RUN_TEST(test_MixedOperations);
    RUN_TEST(test_ConstexprSupport);
    RUN_TEST(test_Precision);

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
