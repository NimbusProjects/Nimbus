#ifndef FUZION_BACKTRACK_H_
#define FUZION_BACKTRACK_H_

#include "../SDK/CBaseClientState.h"
#include "../SDK/IInputSystem.h"
#include "../Utils/entity.h"

#include <vector>

namespace BackTrack {
// information about single mfking player
struct BackTrackRecord {
  C_BasePlayer* entity;
  Vector head, origin;
  matrix3x4_t bone_matrix[128];
};

// stores information about all players for one tick
struct BackTrackFrameInfo {
  int tick_count;
  float simulation_time;
  std::vector<BackTrackRecord> records;
};

void CreateMove(CUserCmd* cmd);

extern std::vector<BackTrack::BackTrackFrameInfo> backtrack_frames;

}  // namespace BackTrack

#endif  // FUZION_BACKTRACK_H_
