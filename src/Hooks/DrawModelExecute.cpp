#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Hacks/antiaim.h"
#include "../Hacks/lagcomp.h"
#include "../Hacks/chams.h"
#include "../Hacks/esp.h"
#include "../Hacks/thirdperson.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"

typedef void (*DrawModelExecuteFn)(void*, void*, void*,
                                   const ModelRenderInfo_t&, matrix3x4_t*);

IMaterial* material_backtrack;

float Settings::BackTrack::Chams::alpha = 1.0f;
bool Settings::BackTrack::Chams::enabled = false;
bool Settings::BackTrack::Chams::drawlastonly = false;
ColorVar Settings::BackTrack::Chams::firstcolor =
    ColorVar(ImColor(1.f, 1.f, 1.f));
ColorVar Settings::BackTrack::Chams::fadecolor =
    ColorVar(ImColor(1.f, 1.f, 1.f));

void MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out) {
  out[0][column] = in.x;
  out[1][column] = in.y;
  out[2][column] = in.z;
}

void AngleMatrix(const QAngle& angles, matrix3x4_t& matrix) {
  float sr, sp, sy, cr, cp, cy;

  sy = std::sin(DEG2RAD(angles[YAW]));
  sp = std::sin(DEG2RAD(angles[PITCH]));
  sr = std::sin(DEG2RAD(angles[ROLL]));

  cy = std::cos(DEG2RAD(angles[YAW]));
  cp = std::cos(DEG2RAD(angles[PITCH]));
  cr = std::cos(DEG2RAD(angles[ROLL]));

  // matrix = (YAW * PITCH) * ROLL
  matrix[0][0] = cp * cy;
  matrix[1][0] = cp * sy;
  matrix[2][0] = -sp;

  float crcy = cr * cy;
  float crsy = cr * sy;
  float srcy = sr * cy;
  float srsy = sr * sy;
  matrix[0][1] = sp * srcy - crsy;
  matrix[1][1] = sp * srsy + crcy;
  matrix[2][1] = sr * cp;

  matrix[0][2] = (sp * crcy + srsy);
  matrix[1][2] = (sp * crsy - srcy);
  matrix[2][2] = cr * cp;

  matrix[0][3] = 0.0f;
  matrix[1][3] = 0.0f;
  matrix[2][3] = 0.0f;
}

void AngleMatrix(matrix3x4_t& matrix, const QAngle& angles,
                 const Vector& position) {
  AngleMatrix(angles, matrix);
  MatrixSetColumn(position, 3, matrix);
}

void MultiplyMatrix(const matrix3x4_t& in1, const matrix3x4_t& in2,
                    matrix3x4_t& out) {
  out = {};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      out[i][j] = 0;
      for (int k = 0; k < 3; k++) {
        out[i][j] += in1[i][k] * in2[k][j];
      }
    }
  }

  out[0][3] = 0.0f;
  out[1][3] = 0.0f;
  out[2][3] = 0.0f;
}

void VectorRotate(Vector v1, QAngle angle, Vector& out) {
  matrix3x4_t tmpmat;
  AngleMatrix(angle, tmpmat);
  Math::VectorTransform(v1, tmpmat, out);
}

void Hooks::DrawModelExecute(void* thisptr, void* context, void* state,
                             const ModelRenderInfo_t& pInfo,
                             matrix3x4_t* pCustomBoneToWorld) {
  if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot()) {
    Chams::DrawModelExecute(thisptr, context, state, pInfo, pCustomBoneToWorld);
  }
  if (!material_backtrack)
    material_backtrack = Util::CreateMaterial(XORSTR("VertexLitGeneric"),
                                              XORSTR("VGUI/white_additive"),
                                              false, true, true, true, true);

  if (pInfo.entity_index < engine->GetMaxClients() &&
      pInfo.entity_index == engine->GetLocalPlayer()) {
    const auto localplayer = entityList->GetClientEntity(pInfo.entity_index);

    if (Settings::ThirdPerson::enabled &&
        Settings::ThirdPerson::type == ShowedAngle::BOTH) {

      matrix3x4_t custom_bones[MAXSTUDIOBONES];
      //QAngle fake_angle = AntiAim::calculatedDesyncAngle;
      QAngle fake_angle = CreateMove::lastTickViewAngles;

      Math::NormalizeAngles(fake_angle);
      matrix3x4_t rotation_matrix;
      AngleMatrix(fake_angle, rotation_matrix);
      for (int i = 0; i < MAXSTUDIOBONES; i++) {
        Vector BonePos{}, OutPos{};
        MultiplyMatrix(rotation_matrix, pCustomBoneToWorld[i], custom_bones[i]);
        BonePos =
            Vector(pCustomBoneToWorld[i][0][3], pCustomBoneToWorld[i][1][3],
                   pCustomBoneToWorld[i][2][3]) -
            pInfo.origin;
        VectorRotate(BonePos, fake_angle, OutPos);
        OutPos += pInfo.origin;
        custom_bones[i][0][3] = OutPos.x;
        custom_bones[i][1][3] = OutPos.y;
        custom_bones[i][2][3] = OutPos.z;
      }

      material_backtrack->ColorModulate(1.f, 1.f, 0.f);
      material_backtrack->AlphaModulate(Settings::BackTrack::Chams::alpha);

      modelRender->ForcedMaterialOverride(material_backtrack);
      modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(
          thisptr, context, state, pInfo, custom_bones);
      modelRender->ForcedMaterialOverride(nullptr);
    }
  } else if (Settings::BackTrack::Chams::enabled) {
    const auto first_color = Color::FromImColor(
                   Settings::BackTrack::Chams::firstcolor.Color()),
               fade_color = Color::FromImColor(
                   Settings::BackTrack::Chams::fadecolor.Color());
    Color color;

    const auto max_ticks = BackTrack::backtrack_frames.size();

    if (Settings::BackTrack::Chams::drawlastonly &&
        !BackTrack::backtrack_frames.empty()) {
      for (auto&& ticks : BackTrack::backtrack_frames.back().records) {
        if (pInfo.entity_index < engine->GetMaxClients() &&
            entityList->GetClientEntity(pInfo.entity_index) == ticks.entity) {
          auto tick_difference =
              (globalVars->tickcount -
               BackTrack::backtrack_frames.back().tick_count);
          if (tick_difference <= 1) continue;
          color.r = first_color.r +
                    (fade_color.r - first_color.r) *
                        (1 - (float)tick_difference / (float)max_ticks);
          color.g = first_color.g +
                    (fade_color.g - first_color.g) *
                        (1 - (float)tick_difference / (float)max_ticks);
          color.b = first_color.b +
                    (fade_color.b - first_color.b) *
                        (1 - (float)tick_difference / (float)max_ticks);

          material_backtrack->ColorModulate(color);
          material_backtrack->AlphaModulate(
              0.3 + 0.5 * (1 - (float)tick_difference / (float)max_ticks));

          modelRender->ForcedMaterialOverride(material_backtrack);
          modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(
              thisptr, context, state, pInfo, (matrix3x4_t*)ticks.bone_matrix);
          modelRender->ForcedMaterialOverride(nullptr);
        }
      }
    } else
      for (auto&& frame : BackTrack::backtrack_frames) {
        for (auto&& ticks : frame.records) {
          if (pInfo.entity_index < engine->GetMaxClients() &&
              entityList->GetClientEntity(pInfo.entity_index) == ticks.entity) {
            auto tick_difference = (globalVars->tickcount - frame.tick_count);
            if (tick_difference <= 1) continue;
            color.r = first_color.r +
                      (fade_color.r - first_color.r) *
                          (1 - (float)tick_difference / (float)max_ticks);
            color.g = first_color.g +
                      (fade_color.g - first_color.g) *
                          (1 - (float)tick_difference / (float)max_ticks);
            color.b = first_color.b +
                      (fade_color.b - first_color.b) *
                          (1 - (float)tick_difference / (float)max_ticks);

            material_backtrack->ColorModulate(color);
            material_backtrack->AlphaModulate(
                0.3 + 0.5 * (1 - (float)tick_difference / (float)max_ticks));

            modelRender->ForcedMaterialOverride(material_backtrack);
            modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(
                thisptr, context, state, pInfo,
                (matrix3x4_t*)ticks.bone_matrix);
            modelRender->ForcedMaterialOverride(nullptr);
          }
        }
      }
  }

  modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(
      thisptr, context, state, pInfo, pCustomBoneToWorld);
  modelRender->ForcedMaterialOverride(nullptr);

  if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot()) {
    ESP::DrawModelExecute();
  }
}
