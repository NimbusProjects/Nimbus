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
bool Settings::AntiAim::Freestanding::enabled = false;

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

// Pasted from space!hook, but I tried. Hi Zede! :3c
static bool GetBestHeadAngle(QAngle& angle)
{
	float b, r, l;

	Vector src3D, dst3D, forward, right, up;

	trace_t tr;
	Ray_t ray, ray2, ray3;
	CTraceFilter filter;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return false;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	viewAngles.x = 0;

	Math::AngleVectors(viewAngles, forward, right, up);

	auto GetTargetEntity = [ & ] ( void )
	{
		float bestFov = FLT_MAX;
		C_BasePlayer* bestTarget = NULL;

		for( int i = 0; i < engine->GetMaxClients(); ++i )
		{
			C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

			if (!player
				|| player == localplayer
				|| player->GetDormant()
				|| !player->GetAlive()
				|| player->GetImmune()
				|| player->GetTeam() == localplayer->GetTeam())
				continue;

			float fov = Math::GetFov(viewAngles, Math::CalcAngle(localplayer->GetEyePosition(), player->GetEyePosition()));

			if( fov < bestFov )
			{
				bestFov = fov;
				bestTarget = player;
			}
		}

		return bestTarget;
	};

	auto target = GetTargetEntity();
	filter.pSkip = localplayer;
	src3D = localplayer->GetEyePosition();
	dst3D = src3D + (forward * 384);

	if (!target)
		return false;

	ray.Init(src3D, dst3D);
	trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	b = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);
	trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	r = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);
	trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	l = (tr.endpos - tr.startpos).Length();

	if (b < r && b < l && l == r)
		return true; //if left and right are equal and better than back

	if (b > r && b > l)
		angle.y -= 180; //if back is the best angle
	else if (r > l && r > b)
		angle.y += 90; //if right is the best angle
	else if (r > l && r == b)
		angle.y += 135; //if right is equal to back
	else if (l > r && l > b)
		angle.y -= 90; //if left is the best angle
	else if (l > r && l == b)
		angle.y -= 135; //if left is equal to back
	else
		return false;

	return true;
}

static void DoAntiAim(AntiAimType type, QAngle& angle, bool bSend, CCSGOAnimState* animState, bool directionSwitch, C_BasePlayer* localplayer, CUserCmd* cmd)
{

     if(cmd->buttons & IN_ATTACK){
        !bSend;
    }



   Settings::AntiAim::swap = bSend;
   if (Settings::AntiAim::States::enabled)
    {
        if (localplayer->GetVelocity().Length() <= 0.3f)
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
            angle.y += directionSwitch ? -maxDelta + Settings::AntiAim::Desync::amount : maxDelta - Settings::AntiAim::Desync::amount;
        else
            angle.y += directionSwitch ? maxDelta - Settings::AntiAim::Desync::amount : -maxDelta + Settings::AntiAim::Desync::amount;

        if (!bSend)
        {

            if (yFlip)
                angle.y += directionSwitch ? -maxDelta + RealOffset: maxDelta - RealOffset;
            else
                angle.y += directionSwitch ? maxDelta - RealOffset : -maxDelta + RealOffset;
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
            angle.y += directionSwitch ?  Settings::AntiAim::fakeL - RealOffset : Settings::AntiAim::fakeR + RealOffset;

       }
        else{
            if(Settings::FakeLag::bSend)
              CreateMove::sendPacket2 = false;

            angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + DesyncOffset : -Settings::AntiAim::Desync::amount - DesyncOffset;
        }


    } break;

    case AntiAimType::DUMP: {
        angle.x = 89.0f;

        static bool yFlip = false;
        static bool dFlip = false;

          if(bSend){
            //angle.y += directionSwitch ? -DesyncOffset : DesyncOffset;
            angle.y += directionSwitch ? (-maxDelta - Settings::AntiAim::Desync::amount) + RealOffset : (maxDelta + Settings::AntiAim::Desync::amount ) - RealOffset;

          if(Settings::FakeLag::bSend)
              CreateMove::sendPacket2 = false;

          }
        else{
            if (yFlip)
              angle.y += directionSwitch ? (-maxDelta - Settings::AntiAim::Desync::amount) + DesyncOffset : (maxDelta + Settings::AntiAim::Desync::amount ) - DesyncOffset;
            else
              angle.y += directionSwitch ? (maxDelta + Settings::AntiAim::Desync::amount) - DesyncOffset : (maxDelta + Settings::AntiAim::Desync::amount ) + DesyncOffset;

            if (dFlip)
              angle.y += directionSwitch ? -maxDelta + DesyncOffset : maxDelta  - DesyncOffset;
            else
              angle.y += directionSwitch ? maxDelta - DesyncOffset : maxDelta + DesyncOffset;






            yFlip = !yFlip;
            dFlip = !dFlip;
        }
    } break;

    case AntiAimType::CUSTOM: {
        angle.x = 89.0f;

        if (Settings::AntiAim::States::enabled)
        {
            if (localplayer->GetVelocity().Length() <= 0.1f)
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

    if(!engine->IsInGame())
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


       if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    if (Settings::AntiAim::AutoDisable::knifeHeld && localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;
    QAngle edge_angle = angle;
  	bool freestanding = Settings::AntiAim::Freestanding::enabled && GetBestHeadAngle(edge_angle);

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
    else
    {
    	DoAntiAim(type, angle, bSend, animState, directionSwitch, localplayer, cmd);

      if(freestanding)
      angle.y = edge_angle.y;

    }

    if (should_clamp)
    {
        Math::NormalizeAngles(angle);
        Math::ClampAngles(angle);
    }

    if (!needToFlick)
        CreateMove::sendPacket = bSend;

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
