#include "antiaim.h"

#include "aimbot.h"
#include "exploits.h"
#include "fakelag.h"
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


float Settings::AntiAim::loff1 = 1;
float Settings::AntiAim::loff2 = 1;

int Settings::AntiAim::fakeL = 0;
int Settings::AntiAim::fakeR = 0;

float Settings::AntiAim::HeadEdge::distance = 25.0f;
ButtonCode_t Settings::AntiAim::left = ButtonCode_t::KEY_LEFT;
ButtonCode_t Settings::AntiAim::right = ButtonCode_t::KEY_RIGHT;

float Settings::AntiAim::yaw = 1.0f;
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


QAngle AntiAim::calculatedDesyncAngle;
QAngle AntiAim::fakeAngle;
QAngle AntiAim::realAngle;
QAngle AntiAim::angle;



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

inline float RandomFloat3(float min, float max)
{
  min = fmod((float)rand(),Settings::AntiAim::loff1);
  max = fmod((float)rand(),Settings::AntiAim::loff2);
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


static float Distance(Vector a, Vector b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

static bool GetBestHeadAngle(QAngle& angle)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    Vector position = localplayer->GetVecOrigin() + localplayer->GetVecViewOffset();

    float closest_distance = 100.0f;

    float radius = Settings::AntiAim::HeadEdge::distance + 0.1f;
    float step = M_PI * 2.0 / 8;

    for (float a = 0; a < (M_PI * 2.0); a += step)
    {
        Vector location(radius * cos(a) + position.x, radius * sin(a) + position.y, position.z);

        Ray_t ray;
        trace_t tr;
        ray.Init(position, location);
        CTraceFilter traceFilter;
        traceFilter.pSkip = localplayer;
        trace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);

        float distance = Distance(position, tr.endpos);

        if (distance < closest_distance)
        {
            closest_distance = distance;
            angle.y = RAD2DEG(a);
        }
    }

    return closest_distance < Settings::AntiAim::HeadEdge::distance;
}
// Pasted from space!hook, but I tried


/*static bool GetBestHeadAngle(QAngle& angle)
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
*/

static void DoAntiAim(AntiAimType type, QAngle& angle, bool bSend, CCSGOAnimState* animState, bool directionSwitch, C_BasePlayer* localplayer, CUserCmd* cmd)
{

  C_BasePlayer* player;

  float off1 = Settings::AntiAim::off1;
  float off2 = Settings::AntiAim::off2;
  float roff1 = Settings::AntiAim::roff1;
  float roff2 = Settings::AntiAim::roff2;

  float DesyncOffset = RandomFloat(off1, off2);// - (RandomFloat2(off3, off4) + 4);
  float RealOffset = RandomFloat2(roff1, roff2);// - (RandomFloat2(roff3, roff4) + 4);




   Settings::AntiAim::swap = bSend;


    if (Settings::AntiAim::States::enabled)
    {
        if (localplayer->GetVelocity().Length() <= 0.0f)
            type = Settings::AntiAim::States::Stand::type;
        else if (!(localplayer->GetFlags() & FL_ONGROUND))
            type = Settings::AntiAim::States::Air::type;
        else
            type = Settings::AntiAim::States::Run::type;
    }
    else
        type = Settings::AntiAim::type;


    float maxDelta = AntiAim::GetMaxDelta(animState);
    switch (type)
    {
    case AntiAimType::RAGE: {

      if(IN_ATTACK || IN_ATTACK2)
        !bSend;

        static bool yFlip = false;

        angle.x = 89.0f;
        if (yFlip)
            angle.y += directionSwitch ? (maxDelta - (Settings::AntiAim::Desync::amount + DesyncOffset)) : (-maxDelta + (Settings::AntiAim::Desync::amount - DesyncOffset));
        else
            angle.y += directionSwitch ? (-maxDelta + (Settings::AntiAim::Desync::amount - DesyncOffset))/ 2 + -Settings::AntiAim::Desync::amount : (maxDelta - (Settings::AntiAim::Desync::amount + DesyncOffset))/ 2 + -Settings::AntiAim::Desync::amount;

        if (!bSend)
        {
            if (yFlip)
                angle.y += directionSwitch ? (-maxDelta + (Settings::AntiAim::Desync::amount - RealOffset)) : (maxDelta  - (Settings::AntiAim::Desync::amount + RealOffset)) ;
            else
                angle.y += directionSwitch ? (maxDelta - (Settings::AntiAim::Desync::amount + RealOffset )): (-maxDelta + (Settings::AntiAim::Desync::amount - RealOffset));
        }
        else
            yFlip = !yFlip;

    } break;

    case AntiAimType::LEGIT: {

     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;

    if (!bSend)
        angle.y += directionSwitch ? 60 : -60;
    else
        cmd->command_number % 2;

      }break;

      case AntiAimType::DUMP: {
        angle.x = 89.0f;

      if(IN_ATTACK || IN_ATTACK2)
        !bSend;


     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;


     if (!bSend)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + DesyncOffset : -Settings::AntiAim::Desync::amount - DesyncOffset;
     else
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + RealOffset : Settings::AntiAim::Desync::amount - RealOffset;



    } break;

    case AntiAimType::GHETTO: {

      if(IN_ATTACK || IN_ATTACK2)
        !bSend;

     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;

     if (!bSend)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + DesyncOffset : -Settings::AntiAim::Desync::amount - DesyncOffset;
     else
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + RealOffset : Settings::AntiAim::Desync::amount - RealOffset;



    } break;

    case AntiAimType::SCRIM: {
      angle.x = 89.0f;


      if(IN_ATTACK || IN_ATTACK2)
        !bSend;

      static bool bFlip = true;
      angle.x = 89.0f;

     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;

     //this is from !bSend to bSend
     if (bSend){
       if(DesyncOffset >= 1 && DesyncOffset <= 45){
          if(bFlip)
         angle.y += directionSwitch ? 66 : -66;
          else
         angle.y += directionSwitch ? -126 : 126;
       }
        else  if(DesyncOffset >= 46 && DesyncOffset <= 49){
          if(bFlip)
         angle.y += directionSwitch ? 36 : -36;
          else
         angle.y += directionSwitch ? -111 : 111;
        }
        else  if(DesyncOffset >= 50 && DesyncOffset <= 59)
        {
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 344 : -Settings::AntiAim::Desync::amount - 344;
          if(bFlip)
         angle.y += directionSwitch ? 77 : -77;
          else
         angle.y += directionSwitch ? -2 : 2;
        }
        else  if(DesyncOffset >= 60 && DesyncOffset <= 69)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 111 : -Settings::AntiAim::Desync::amount - 111;
        else  if(DesyncOffset >= 70 && DesyncOffset <= 79)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 23 : -Settings::AntiAim::Desync::amount - 23;
        else  if(DesyncOffset >= 80 && DesyncOffset <= 360){
          if(bFlip)
         angle.y += directionSwitch ? 179 : -179;
          else
         angle.y += directionSwitch ? -8 : 13;
        }
      bFlip = !bFlip;
     }
     else{
       if(RealOffset >= 1 && RealOffset <= 45){
          if(bFlip)
         angle.y += directionSwitch ? 179 : -179;
          else
         angle.y += directionSwitch ? -83 : 83;
       }
        else  if(RealOffset >= 46 && RealOffset <= 49){
          if(bFlip)
         angle.y += directionSwitch ? -55 : 55;
          else
         angle.y += directionSwitch ? 151 : -151;
        }
        else  if(RealOffset >= 50 && RealOffset <= 59)
        {
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 344 : Settings::AntiAim::Desync::amount - 344;
          if(bFlip)
         angle.y += directionSwitch ? -93 : 93;
          else
         angle.y += directionSwitch ? 66 : -66;
        }
        else  if(RealOffset >= 60 && RealOffset <= 69)
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 111 : Settings::AntiAim::Desync::amount - 111;
        else  if(RealOffset >= 70 && RealOffset <= 79)
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 23 : Settings::AntiAim::Desync::amount - 23;
        else  if(RealOffset >= 80 && RealOffset <= 360){
          if(bFlip)
         angle.y += directionSwitch ? 3 : -3;
          else
         angle.y += directionSwitch ? -220 : 220;
        }
     }
      bFlip = !bFlip;




    } break;


    case AntiAimType::RUBY: {

      if(IN_ATTACK || IN_ATTACK2)
        !bSend;

      static bool bFlip = true;
      angle.x = 89.0f;

     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;

     if (!bSend){
       if(DesyncOffset >= 1 && DesyncOffset <= 45){
          if(bFlip)
         angle.y += directionSwitch ? 36 : -36;
          else
         angle.y += directionSwitch ? -166 : 166;
       }
        else  if(DesyncOffset >= 46 && DesyncOffset <= 49){
          if(bFlip)
         angle.y += directionSwitch ? 96 : -96;
          else
         angle.y += directionSwitch ? -111 : 111;
        }
        else  if(DesyncOffset >= 50 && DesyncOffset <= 59)
        {
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 344 : -Settings::AntiAim::Desync::amount - 344;
          if(bFlip)
         angle.y += directionSwitch ? 56 : -56;
          else
         angle.y += directionSwitch ? -78 : 76;
        }
        else  if(DesyncOffset >= 60 && DesyncOffset <= 69)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 111 : -Settings::AntiAim::Desync::amount - 111;
        else  if(DesyncOffset >= 70 && DesyncOffset <= 79)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 23 : -Settings::AntiAim::Desync::amount - 23;
        else  if(DesyncOffset >= 80 && DesyncOffset <= 360){
          if(bFlip)
         angle.y += directionSwitch ? 179 : -179;
          else
         angle.y += directionSwitch ? -8 : 13;
        }
      bFlip = !bFlip;
     }
     else{
       if(RealOffset >= 1 && RealOffset <= 45){
          if(bFlip)
         angle.y += directionSwitch ? 139 : -139;
          else
         angle.y += directionSwitch ? -83 : 83;
       }
        else  if(RealOffset >= 46 && RealOffset <= 49){
          if(bFlip)
         angle.y += directionSwitch ? -155 : 155;
          else
         angle.y += directionSwitch ? 15 : -15;
        }
        else  if(RealOffset >= 50 && RealOffset <= 59)
        {
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 344 : Settings::AntiAim::Desync::amount - 344;
          if(bFlip)
         angle.y += directionSwitch ? -9 : 9;
          else
         angle.y += directionSwitch ? 93 : -93;
        }
        else  if(RealOffset >= 60 && RealOffset <= 69)
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 111 : Settings::AntiAim::Desync::amount - 111;
        else  if(RealOffset >= 70 && RealOffset <= 79)
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 23 : Settings::AntiAim::Desync::amount - 23;
        else  if(RealOffset >= 80 && RealOffset <= 360){
          if(bFlip)
         angle.y += directionSwitch ? 3 : -3;
          else
         angle.y += directionSwitch ? -220 : 220;
        }
     }
      bFlip = !bFlip;




    } break;


    case AntiAimType::CROOKED: {

      if(IN_ATTACK || IN_ATTACK2)
        !bSend;

      static bool bFlip = true;
      angle.x = 89.0f;

     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;

     if (!bSend){
       if(DesyncOffset >= 1 && DesyncOffset <= 45){
          if(bFlip)
         angle.y += directionSwitch ? 36 : -36;
          else
         angle.y += directionSwitch ? -166 : 166;
       }
        else  if(DesyncOffset >= 46 && DesyncOffset <= 49){
          if(bFlip)
         angle.y += directionSwitch ? 96 : -96;
          else
         angle.y += directionSwitch ? -111 : 111;
        }
        else  if(DesyncOffset >= 50 && DesyncOffset <= 59)
        {
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 344 : -Settings::AntiAim::Desync::amount - 344;
          if(bFlip)
         angle.y += directionSwitch ? 56 : -56;
          else
         angle.y += directionSwitch ? -78 : 76;
        }
        else  if(DesyncOffset >= 60 && DesyncOffset <= 69)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 111 : -Settings::AntiAim::Desync::amount - 111;
        else  if(DesyncOffset >= 70 && DesyncOffset <= 79)
         angle.y += directionSwitch ? Settings::AntiAim::Desync::amount + 23 : -Settings::AntiAim::Desync::amount - 23;
        else  if(DesyncOffset >= 80 && DesyncOffset <= 360){
          if(bFlip)
         angle.y += directionSwitch ? 179 : -179;
          else
         angle.y += directionSwitch ? -8 : 13;
        }
      bFlip = !bFlip;
     }
     else{
       if(RealOffset >= 1 && RealOffset <= 45){
          if(bFlip)
         angle.y += directionSwitch ? 179 : -179;
          else
         angle.y += directionSwitch ? -83 : 83;
       }
        else  if(RealOffset >= 46 && RealOffset <= 49){
          if(bFlip)
         angle.y += directionSwitch ? -55 : 55;
          else
         angle.y += directionSwitch ? 151 : -151;
        }
        else  if(RealOffset >= 50 && RealOffset <= 59)
        {
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 344 : Settings::AntiAim::Desync::amount - 344;
          if(bFlip)
         angle.y += directionSwitch ? -96 : 26;
          else
         angle.y += directionSwitch ? 108 : -46;
        }
        else  if(RealOffset >= 60 && RealOffset <= 69)
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 111 : Settings::AntiAim::Desync::amount - 111;
        else  if(RealOffset >= 70 && RealOffset <= 79)
         angle.y += directionSwitch ? -Settings::AntiAim::Desync::amount + 23 : Settings::AntiAim::Desync::amount - 23;
        else  if(RealOffset >= 80 && RealOffset <= 360){
          if(bFlip)
         angle.y += directionSwitch ? 3 : -3;
          else
         angle.y += directionSwitch ? -220 : 220;
        }
     }
      bFlip = !bFlip;
    } break;



    case AntiAimType::CUSTOM: {

        angle.x = 89.0f;

     if (std::fabs(cmd->sidemove) < 5.f)
        cmd->sidemove = cmd->buttons & IN_DUCK ? cmd->tick_count & 1 ? 3.25f : -3.25f : cmd->tick_count & 1 ? 1.1f : -1.1f;

        if (Settings::AntiAim::States::enabled)
        {
            if (localplayer->GetVelocity().Length() <= 0.0f)
                angle.y += Settings::AntiAim::States::Stand::angle;
            else if (!(localplayer->GetFlags() & FL_ONGROUND))
                angle.y += Settings::AntiAim::States::Air::angle;
            else
                angle.y += Settings::AntiAim::States::Run::angle;
        }
        else
            angle.y += Settings::AntiAim::yaw;

        if (!bSend)
            angle.y += directionSwitch ? maxDelta : -maxDelta;

    } break;

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

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    if (Settings::AntiAim::AutoDisable::knifeHeld && localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;

    QAngle edge_angle = angle;
	bool freestanding = Settings::AntiAim::Freestanding::enabled && GetBestHeadAngle(edge_angle);

    static bool directionSwitch = false;

    if (inputSystem->IsButtonDown(Settings::AntiAim::left))
		directionSwitch = true;
	else if (inputSystem->IsButtonDown(Settings::AntiAim::right))
		directionSwitch = false;

    CCSGOAnimState* animState = localplayer->GetAnimState();

    float maxDelta = AntiAim::GetMaxDelta(animState);
    float lbyOffset = Settings::AntiAim::LBYBreaker::enabled ? Settings::AntiAim::LBYBreaker::offset : maxDelta;

    bool needToFlick = false;

    float vel2D = localplayer->GetVelocity().Length2D();//localplayer->GetAnimState()->verticalVelocity + localplayer->GetAnimState()->horizontalVelocity;
    static float lastCheck;
    static bool lbyBreak = false;

    float loff1 = Settings::AntiAim::loff1;
    float loff2 = Settings::AntiAim::loff2;



    if( Settings::AntiAim::LBYBreaker::enabled ){

        float LBYRand = RandomFloat3(loff1, loff2);// - (RandomFloat2(off3, off4) + 4);
        if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
            lbyBreak = false;
            lastCheck = globalVars->curtime;
            CreateMove::sendPacket = true;
        } else {
            if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                angle.y += directionSwitch ? -Settings::AntiAim::LBYBreaker::offset - LBYRand : Settings::AntiAim::LBYBreaker::offset + LBYRand;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
                CreateMove::sendPacket = false;
            } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                angle.y += directionSwitch ? -Settings::AntiAim::LBYBreaker::offset - LBYRand : Settings::AntiAim::LBYBreaker::offset + LBYRand;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
                CreateMove::sendPacket = false;
            }
        }
    }

    if(Settings::AntiAim::enabled && !needToFlick)
    {
      static bool bSend = true;
      bSend = !bSend;

      CreateMove::sendPacket = bSend;
      static AntiAimType type;

    	DoAntiAim(type, angle, bSend, animState, directionSwitch, localplayer, cmd);

      Airstuck::CreateMove(cmd, bSend);
        if (freestanding)
            angle.y = edge_angle.y;
    }

    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
