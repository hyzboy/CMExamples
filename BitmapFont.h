#pragma once
#include<hgl/type/DataType.h>

using namespace hgl;

bool LoadBitmapFont();
void ClearBitmapFont();

const uint8 *Get8x8Char(const char ch);
const uint8 *Get8x16Char(const char ch);