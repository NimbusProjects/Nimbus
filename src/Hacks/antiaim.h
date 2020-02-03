#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"

namespace AntiAim
{
    extern QAngle calculatedDesyncAngle;
    extern QAngle fakeAngle;
    extern QAngle realAngle;

    float GetMaxDelta( CCSGOAnimState *animState );

    //Hooks
    void CreateMove(CUserCmd* cmd);
}
