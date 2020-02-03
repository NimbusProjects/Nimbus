#include "antiaim.h"

#include "aimbot.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"

bool Settings::AntiAim::enabled = false;
bool Settings::AntiAim::AutoDisable::knifeHeld = false;
AntiAimType Settings::AntiAim::type = AntiAimType::LEGIT;
bool Settings::AntiAim::swap = false;

float Settings::AntiAim::off1 = 1;
float Settings::AntiAim::off2 = 1;
float Settings::AntiAim::roff1 = 1;
float Settings::AntiAim::roff2 = 1;

int Settings::AntiAim::fakeL = 0;
int Settings::AntiAim::fakeR = 0;

ButtonCode_t Settings::AntiAim::left = ButtonCode_t::KEY_LEFT;
ButtonCode_t Settings::AntiAim::right = ButtonCode_t::KEY_RIGHT;

float Settings::AntiAim::yaw = 180.0f;
bool Settings::AntiAim::States::enabled = false;

float Settings::AntiAim::States::Stand::angle = 180.0f;
float Settings::AntiAim::States::Walk::angle = 180.0f;
float Settings::AntiAim::States::Run::angle = 180.0f;
float Settings::AntiAim::States::Air::angle = 180.0f;

bool Settings::FakeLag::bSend = false;

AntiAimType Settings::AntiAim::States::Stand::type = AntiAimType::LEGIT;
AntiAimType Settings::AntiAim::States::Walk::type = AntiAimType::CUSTOM;
AntiAimType Settings::AntiAim::States::Run::type = AntiAimType::CUSTOM;
AntiAimType Settings::AntiAim::States::Air::type = AntiAimType::LEGIT;


bool Settings::AntiAim::LBYBreaker::enabled = false;
bool Settings::AntiAim::LBYBreaker::custom = false;
float Settings::AntiAim::LBYBreaker::offset = 180.0f;
float Settings::AntiAim::Desync::amount = 45.0f;
float Settings::AntiAim::Desync::time = 1.1f;
float Settings::AntiAim::Desync::interval = 0.22f;

QAngle AntiAim::realAngle;
QAngle AntiAim::fakeAngle;
QAngle AntiAim::calculatedDesyncAngle;



inline float RandomFloat(float min, float max)
{
  min = fmod((float)rand(),Settings::AntiAim::off1);
  max = fmod((float)rand(),Settings::AntiAim::off2);
  return (min, max);
}

inline float RandomFloat2(float min, float max)
{
  min = fmod((float)rand(),Settings::AntiAim::roff1);
  max = fmod((float)rand(),Settings::AntiAim::roff2);
  return (min, max);
}




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
static void DoAntiAim(AntiAimType type, QAngle& angle, bool bSend, CCSGOAnimState* animState, bool directionSwitch, C_BasePlayer* localplayer, CUserCmd* cmd)
{

   if (Settings::AntiAim::States::enabled)
    {
        if (localplayer->GetVelocity().Length() <= 0.0f)
            type = Settings::AntiAim::States::Stand::type;
        else if (!(localplayer->GetFlags() & FL_ONGROUND))
            type = Settings::AntiAim::States::Air::type;
//        else if (cmd->buttons & IN_WALK)
//           type = Settings::AntiAim::States::Walk::type;
        else
            type = Settings::AntiAim::States::Run::type;
    }
    else
        type = Settings::AntiAim::type;

	float maxDelta = AntiAim::GetMaxDelta(animState);

  float off1 = Settings::AntiAim::off1;
  float off2 = Settings::AntiAim::off2;
//	float off3 = Settings::AntiAim::off1;
//  float off4 = Settings::AntiAim::off2;

  float roff1 = Settings::AntiAim::roff1;
  float roff2 = Settings::AntiAim::roff2;
//	float roff3 = Settings::AntiAim::roff1;
//  float roff4 = Settings::AntiAim::roff2;



  float DesyncOffset = RandomFloat(off1, off2);// - (RandomFloat2(off3, off4) + 4);
  float RealOffset = RandomFloat2(roff1, roff2);// - (RandomFloat2(roff3, roff4) + 4);

    switch (type)
    {
    case AntiAimType::RAGE: {
        static bool yFlip = false;

        angle.x = 89.0f;
        if (yFlip)
            angle.y += directionSwitch ? -maxDelta - Settings::AntiAim::Desync::amount : maxDelta + Settings::AntiAim::Desync::amount;
        else
            angle.y += directionSwitch ? maxDelta - Settings::AntiAim::Desync::amount : -maxDelta + Settings::AntiAim::Desync::amount;

        if (!bSend)
        {

//         if(Settings::FakeLag::bSend)
//              CreateMove::sendPacket2 = true;


            if (yFlip)
                angle.y += directionSwitch ? -maxDelta - RealOffset: maxDelta + RealOffset;
            else
                angle.y += directionSwitch ? maxDelta + RealOffset : -maxDelta - RealOffset;
        }
        else{
            if(Settings::FakeLag::bSend)
              CreateMove::sendPacket2 = false;

            yFlip = !yFlip;
        }
    }
    break;
    case AntiAimType::LEGIT: {
        if (!bSend){
            angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + DesyncOffset : -Settings::AntiAim::Desync::amount - DesyncOffset;

 //      if(Settings::FakeLag::bSend)
 //             CreateMove::sendPacket2 = true;
        }
        else{
            if(Settings::FakeLag::bSend)
              CreateMove::sendPacket2 = false;

            angle.y += directionSwitch ?  Settings::AntiAim::fakeL - RealOffset : Settings::AntiAim::fakeR + RealOffset;
        }
    } break;

    case AntiAimType::DUMP: {
        angle.x = 89.0f;

        static bool yFlip = false;

          if(bSend){
            //angle.y += directionSwitch ? -DesyncOffset : DesyncOffset;
            angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount - RealOffset : Settings::AntiAim::Desync::amount + RealOffset;

          if(Settings::FakeLag::bSend)
              CreateMove::sendPacket2 = false;

          }
        else{
        angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount - DesyncOffset : Settings::AntiAim::Desync::amount + DesyncOffset;
            angle.x = 89.0f;
        }
    } break;

    case AntiAimType::CUSTOM: {
        angle.x = 89.0f;

        if (Settings::AntiAim::States::enabled)
        {
            if (localplayer->GetVelocity().Length() <= 0.0f)
                angle.y += Settings::AntiAim::States::Stand::angle;
            else if (!(localplayer->GetFlags() & FL_ONGROUND))
                angle.y += Settings::AntiAim::States::Air::angle;
            else if (cmd->buttons & IN_WALK)
                angle.y += Settings::AntiAim::States::Walk::angle;
            else
                angle.y += Settings::AntiAim::States::Run::angle;
        }
        else
            angle.y += Settings::AntiAim::yaw;

        if (!bSend)
        {
            angle.y += directionSwitch ? maxDelta : -maxDelta;

        }
        else{
            if(Settings::FakeLag::bSend)
              CreateMove::sendPacket2 = false;
        }
    }


    default:
        break;
    }
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    if (!Settings::AntiAim::enabled)
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
/*
    if (localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;
*/
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
            nextUpdate = globalVars->curtime + Settings::AntiAim::Desync::interval;
        }
        else if (!lbyBreak && (globalVars->curtime - lastCheck) > Settings::AntiAim::Desync::interval || lbyBreak && (globalVars->curtime - lastCheck) > Settings::AntiAim::Desync::time)
        {
            lbyBreak = true;
            lastCheck = globalVars->curtime;
            nextUpdate = globalVars->curtime + Settings::AntiAim::Desync::time;
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


    static AntiAimType type;

    if (needToFlick)
    {
        CreateMove::sendPacket = false;
        if (!Settings::AntiAim::LBYBreaker::custom)
            angle.y += directionSwitch ? Settings::AntiAim::LBYBreaker::offset : -Settings::AntiAim::LBYBreaker::offset;
        else
        {
            static float maxDelta = AntiAim::GetMaxDelta(animState);

            if (type != AntiAimType::RAGE)
                angle.y += directionSwitch ? -maxDelta : maxDelta;
            else
                angle.y += directionSwitch ? -90.0f : 90.0f;
        }
    }
    else{
    	DoAntiAim(type, angle, bSend, animState, directionSwitch, localplayer, cmd);
    }

    if (should_clamp)
    {
        Math::NormalizeAngles(angle);
        Math::ClampAngles(angle);
    }

    if (!needToFlick)
        CreateMove::sendPacket = bSend;

    if(Settings::AntiAim::enabled && Settings::Aimbot::silent && cmd->buttons & IN_ATTACK)
    {
      DoAntiAim(type, angle, bSend, animState, directionSwitch, localplayer, cmd);
      bSend = false;
    }

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
