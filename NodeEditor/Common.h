//
// Created by engin on 25.03.2019.
//

#ifndef SIL_NODETEST_COMMON_H
#define SIL_NODETEST_COMMON_H

#include "../imgui/imgui.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

#endif //SIL_NODETEST_COMMON_H
