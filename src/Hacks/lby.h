#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"

namespace LBY
{
    float GetMaxDelta( CCSGOAnimState *animState );
    void CreateMove(CUserCmd* cmd);
}
