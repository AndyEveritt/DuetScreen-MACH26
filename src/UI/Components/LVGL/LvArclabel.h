#pragma once

#include "UI/Components/LVGL/generated/LvArclabel.gen.h"

namespace UI
{
    class LvArclabel : public LvArclabelGen
    {
      public:
        using LvArclabelGen::LvArclabelGen;

        void setText(const std::string& text) { LvArclabelGen::setText(text.c_str()); }
        void setTextStatic(const std::string& text) { LvArclabelGen::setTextStatic(text.c_str()); }
    };
}
