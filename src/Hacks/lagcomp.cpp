#include "lagcomp.h"

#include "../Utils/math.h"
#include "../interfaces.h"
#include "../settings.h"

std::vector<LagComp::LagCompTickInfo> LagComp::lagCompTicks;

static void RemoveInvalidTicks()
{
	while (LagComp::lagCompTicks.size() > Settings::LagComp::value)
		LagComp::lagCompTicks.pop_back();
}

static void RegisterTicks()
{
	const auto curTick = LagComp::lagCompTicks.insert(LagComp::lagCompTicks.begin(), {globalVars->tickcount, globalVars->curtime});
	const auto localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	for (int i = 1; i < engine->GetMaxClients(); ++i)
	{
		const auto player = (C_BasePlayer *)entityList->GetClientEntity(i);

		if (!player
		|| player == localplayer
		|| player->GetDormant()
		|| !player->GetAlive()
		|| Entity::IsTeamMate(player, localplayer)
		|| player->GetImmune())
			continue;

		LagComp::LagCompRecord record;

		record.entity = player;
		record.origin = player->GetVecOrigin();
		record.head = player->GetBonePosition(BONE_HEAD);

		if (player->SetupBones(record.bone_matrix, 128, BONE_USED_BY_HITBOX, globalVars->curtime))
			curTick->records.push_back(record);
	}
}

void LagComp::CreateMove(CUserCmd *cmd)
{
	if (!Settings::LagComp::enabled)
		return;

	RemoveInvalidTicks();
	RegisterTicks();

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	const auto weapon = (C_BaseCombatWeapon *)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	
	if (!weapon)
		return;

	float serverTime = localplayer->GetTickBase() * globalVars->interval_per_tick;

	QAngle my_angle;
	engine->GetViewAngles(my_angle);
	QAngle my_angle_rcs = my_angle + *localplayer->GetAimPunchAngle();

	if (cmd->buttons & IN_ATTACK && weapon->GetNextPrimaryAttack() <= serverTime)
	{
		float fov = Settings::Aimbot::AutoAim::fov * 2;

		int tickcount = 0;
		bool has_target = false;

		for (auto &&Tick : LagComp::lagCompTicks)
		{
			for (auto &record : Tick.records)
			{
				float tmpFOV = Math::GetFov(my_angle_rcs, Math::CalcAngle(localplayer->GetEyePosition(), record.head));

				if (tmpFOV < fov)
				{
					fov = tmpFOV;
					tickcount = Tick.tickCount;
					has_target = true;
				}
			}
		}

		if (has_target)
		{
			cmd->tick_count = tickcount;
		}
	}
}
