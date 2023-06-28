#pragma once
#include<hgl/type/DataType.h>

using namespace hgl;

bool LoadBitmapFont();
void ClearBitmapFont();

const uint GetCharWidth();
const uint GetCharHeight();

const uint8 *GetBitmapChar(const char ch);