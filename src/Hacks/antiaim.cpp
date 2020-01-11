#include "antiaim.h"

#include "aimbot.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"

bool Settings::AntiAim::enabled = false;
bool Settings::AntiAim::AutoDisable::knifeHeld = false;
AntiAimType Settings::AntiAim::type = AntiAimType::RAGE;

ButtonCode_t Settings::AntiAim::left = ButtonCode_t::KEY_X;
ButtonCode_t Settings::AntiAim::right = ButtonCode_t::KEY_C;

bool Settings::AntiAim::LBYBreaker::enabled = false;
float Settings::AntiAim::LBYBreaker::offset = 180.0f;

QAngle AntiAim::realAngle;
QAngle AntiAim::fakeAngle;

float AntiAim::GetMaxDelta(CCSGOAnimState *animState) {

    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}

static void DoAntiAim(QAngle& angle, bool bSend, CCSGOAnimState* animState, bool directionSwitch)
{
	float maxDelta = AntiAim::GetMaxDelta(animState);

    switch (Settings::AntiAim::type)
    {
    case AntiAimType::RAGE: {
        static bool yFlip = false;

        angle.x = 89.0f;
        if (yFlip)
            angle.y += directionSwitch ? maxDelta / 2 : -maxDelta / 2;
        else
            angle.y += directionSwitch ? -maxDelta / 2 + 180.0f : maxDelta / 2 + 180.0f;

        if (!bSend)
        {
            if (yFlip)
                angle.y += directionSwitch ? -maxDelta : maxDelta;
            else
                angle.y += directionSwitch ? maxDelta : -maxDelta;
        }
        else
            yFlip = !yFlip;
        } break;
    
    case AntiAimType::LEGIT: {
        if (!bSend)
            angle.y += directionSwitch ? maxDelta : -maxDelta;
    } break;
    
    default:
        break;
    }
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    if (!Settings::AntiAim::enabled && !Settings::AntiAim::LBYBreaker::enabled)
        return;

    if (Settings::Aimbot::AimStep::enabled && Aimbot::aimStepInProgress)
        return;

    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

    QAngle angle = cmd->viewangles;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return;

    if (activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeWeapon;

        if (csGrenade->GetThrowTime() > 0.f)
            return;
    }

    if (localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    if (Settings::AntiAim::AutoDisable::knifeHeld && localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;

    static bool bSend = true;
    bSend = !bSend;

    bool should_clamp = true;
    bool needToFlick = false;
    static bool lbyBreak = false;
    static float lastCheck;
    static float nextUpdate = FLT_MAX;
    float vel2D = localplayer->GetVelocity().Length2D();//localplayer->GetAnimState()->verticalVelocity + localplayer->GetAnimState()->horizontalVelocity;
    CCSGOAnimState* animState = localplayer->GetAnimState();

    if (Settings::AntiAim::LBYBreaker::enabled)
    {
        if (vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN)
        {
            lbyBreak = false;
            lastCheck = globalVars->curtime;
            nextUpdate = globalVars->curtime + 0.22;
        }
        else if (!lbyBreak && (globalVars->curtime - lastCheck) > 0.22 || lbyBreak && (globalVars->curtime - lastCheck) > 1.1)
        {
            lbyBreak = true;
            lastCheck = globalVars->curtime;
            nextUpdate = globalVars->curtime + 1.1;
            needToFlick = true;
        }
    }

    if ((nextUpdate - globalVars->interval_per_tick) >= globalVars->curtime && nextUpdate <= globalVars->curtime)
        CreateMove::sendPacket = false;

    static bool directionSwitch = false;

    if (inputSystem->IsButtonDown(Settings::AntiAim::left))
		directionSwitch = true;
	else if (inputSystem->IsButtonDown(Settings::AntiAim::right))
		directionSwitch = false;

    if (needToFlick)
    {
        CreateMove::sendPacket = false;
        angle.y += directionSwitch ? Settings::AntiAim::LBYBreaker::offset : -Settings::AntiAim::LBYBreaker::offset;
    }
    else
    	DoAntiAim(angle, bSend, animState, directionSwitch);

    if (should_clamp)
    {
        Math::NormalizeAngles(angle);
        Math::ClampAngles(angle);
    }

    if (!needToFlick)
        CreateMove::sendPacket = bSend;

    if (CreateMove::sendPacket)
        AntiAim::realAngle = angle;
    else
        AntiAim::fakeAngle = angle;

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
