#include "lagcomp.h"

#include "../Utils/math.h"
#include "../interfaces.h"
#include "../settings.h"

//bool enabled = true;
bool Settings::BackTrack::enabled = false;
int Settings::BackTrack::time = 16;
float Settings::BackTrack::fov = 180.f;

std::vector<BackTrack::BackTrackFrameInfo> BackTrack::backtrack_frames;

void RemoveInvalidTicks() {
  const auto ticks =
      ((Settings::BackTrack::time > 0) && (Settings::BackTrack::time < 40))
          ? Settings::BackTrack::time
          : 24;
  // TODO: xd
  using namespace BackTrack;
  for (auto it = backtrack_frames.begin(); it != backtrack_frames.end(); it++) {

  }

  while (BackTrack::backtrack_frames.size() > ticks)
    BackTrack::backtrack_frames.pop_back();
}

void RegisterTicks() {
  const auto current_frame = BackTrack::backtrack_frames.insert(
      BackTrack::backtrack_frames.begin(),
      {globalVars->tickcount, globalVars->curtime});

  const auto local_player =
      (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
  for (int i = 1; i < engine->GetMaxClients(); ++i) {
    const auto player = (C_BasePlayer*)entityList->GetClientEntity(i);

    // skip if player is not alive or other shit
    if (!player || player == local_player || player->GetDormant() ||
        !player->GetAlive() || Entity::IsTeamMate(player, local_player) ||
        player->GetImmune())
      continue;

    BackTrack::BackTrackRecord record;
    record.entity = player;
    record.origin = player->GetVecOrigin();
    record.head = player->GetBonePosition(BONE_HEAD);
    if (player->SetupBones(record.bone_matrix, 128, BONE_USED_BY_HITBOX,
                           globalVars->curtime))
      current_frame->records.push_back(record);
  }
}

void UpdateBackTrack(CUserCmd* cmd) {
  const auto local_player =
      (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
  if (!local_player || !local_player->GetAlive()) return;

  float serverTime =
      local_player->GetTickBase() * globalVars->interval_per_tick;
  const auto weapon =
      (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(
          local_player->GetActiveWeapon());

  QAngle my_angle;
  engine->GetViewAngles(my_angle);
  QAngle my_angle_rcs = my_angle + *local_player->GetAimPunchAngle();

  if (cmd->buttons & IN_ATTACK &&
      weapon->GetNextPrimaryAttack() <= serverTime) {
    float fov = Settings::BackTrack::fov;
    int tickcount = 0;
    bool has_target = false;

    for (auto&& frame : BackTrack::backtrack_frames) {
      for (auto& record : frame.records) {
        float tmpFOV = Math::GetFov(
            my_angle_rcs,
            Math::CalcAngle(local_player->GetEyePosition(), record.head));

        if (tmpFOV < fov) {
          fov = tmpFOV;
          tickcount = frame.tick_count;
          has_target = true;
        }
      }
    }

    if (has_target) {
      cmd->tick_count = tickcount;
    }
  }
}

void BackTrack::CreateMove(CUserCmd* cmd) {
  if (!Settings::BackTrack::enabled) return;
  RemoveInvalidTicks();
  RegisterTicks();
  UpdateBackTrack(cmd);
}
