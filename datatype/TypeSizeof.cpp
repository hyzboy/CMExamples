#include<hgl/type/DataType.h>
#include<hgl/math/Math.h>
#include<hgl/type/RectScope.h>

#include<iostream>

using namespace hgl;

#define OUTPUT_SIZEOF(type) std::cout<<" sizeof(" #type ") = "<<sizeof(type)<<std::endl;

void main()
{
    OUTPUT_SIZEOF(char)
    OUTPUT_SIZEOF(char8_t)
    OUTPUT_SIZEOF(char16_t)
    OUTPUT_SIZEOF(char32_t)
    OUTPUT_SIZEOF(wchar_t)

    OUTPUT_SIZEOF(short)
    OUTPUT_SIZEOF(int)
    OUTPUT_SIZEOF(long)
    OUTPUT_SIZEOF(long long)
    OUTPUT_SIZEOF(float)
    OUTPUT_SIZEOF(double)
    OUTPUT_SIZEOF(long double)

    OUTPUT_SIZEOF(void *)
    OUTPUT_SIZEOF(ptrdiff_t)

    OUTPUT_SIZEOF(Vector2f)
    OUTPUT_SIZEOF(Vector3f)
    OUTPUT_SIZEOF(Vector4f)

    OUTPUT_SIZEOF(RectScope2s)
    OUTPUT_SIZEOF(RectScope2i)
    OUTPUT_SIZEOF(RectScope2f)
    OUTPUT_SIZEOF(RectScope2d)

    OUTPUT_SIZEOF(Matrix2f)
    OUTPUT_SIZEOF(Matrix3f)
    OUTPUT_SIZEOF(Matrix4f)
    OUTPUT_SIZEOF(Matrix2x4f)
    OUTPUT_SIZEOF(Matrix3x4f)
    OUTPUT_SIZEOF(Matrix4x2f)
    OUTPUT_SIZEOF(Matrix4x3f)

    OUTPUT_SIZEOF(Quatf)

    OUTPUT_SIZEOF(Transform)
}