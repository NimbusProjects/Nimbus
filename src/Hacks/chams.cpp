#include "chams.h"

#include "lagcomp.h"

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../Hacks/lagcomp.h"

bool Settings::ESP::Chams::enabled = false;
bool Settings::ESP::Chams::Arms::enabled = false;
bool Settings::ESP::Chams::Weapon::enabled = false;
ArmsType Settings::ESP::Chams::Arms::type = ArmsType::DEFAULT;
WeaponType Settings::ESP::Chams::Weapon::type = WeaponType::DEFAULT;
HealthColorVar Settings::ESP::Chams::allyColor = ImColor(0, 0, 255, 255);
HealthColorVar Settings::ESP::Chams::allyVisibleColor = ImColor(0, 255, 0, 255);
HealthColorVar Settings::ESP::Chams::enemyColor = ImColor(255, 0, 0, 255);
HealthColorVar Settings::ESP::Chams::enemyVisibleColor = ImColor(255, 255, 0, 255);
HealthColorVar Settings::ESP::Chams::localplayerColor = ImColor(0, 255, 255, 255);
HealthColorVar Settings::ESP::Chams::backtrackColor = ImColor(0, 255, 255, 255);
ColorVar Settings::ESP::Chams::Arms::color = ImColor(255, 255, 255, 255);
ColorVar Settings::ESP::Chams::Weapon::color = ImColor(255, 255, 255, 255);
ChamsType Settings::ESP::Chams::type = ChamsType::CHAMS;

/*
float Settings::BackTrack::Chams::alpha = 1.0f;
bool Settings::BackTrack::Chams::enabled = false;
bool Settings::BackTrack::Chams::drawlastonly = false;
ColorVar Settings::BackTrack::Chams::firstcolor =
    ColorVar(ImColor(1.f, 1.f, 1.f));
ColorVar Settings::BackTrack::Chams::fadecolor =
    ColorVar(ImColor(1.f, 1.f, 1.f));
*/


//IMaterial* material_backtrack;
IMaterial* materialChams;
IMaterial* materialChamsIgnorez;
IMaterial* materialChamsFlat;
IMaterial* materialChamsFlatIgnorez;
IMaterial* materialChamsArms;
IMaterial* materialChamsWeapons;

typedef void (*DrawModelExecuteFn) (void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

static void DrawPlayer(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!Settings::ESP::Chams::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);
	if (!entity
		|| entity->GetDormant()
		|| !entity->GetAlive())
		return;

	if (entity == localplayer && !Settings::ESP::Filters::localplayer)
		return;

	if (!Entity::IsTeamMate(entity, localplayer) && !Settings::ESP::Filters::enemies)
		return;

	if (entity != localplayer && Entity::IsTeamMate(entity,localplayer) && !Settings::ESP::Filters::allies)
		return;

	IMaterial* visible_material = nullptr;
	IMaterial* hidden_material = nullptr;

	switch (Settings::ESP::Chams::type)
	{
		case ChamsType::CHAMS:
		case ChamsType::CHAMS_XQZ:
			visible_material = materialChams;
			hidden_material = materialChamsIgnorez;
			break;
		case ChamsType::CHAMS_FLAT:
		case ChamsType::CHAMS_FLAT_XQZ:
			visible_material = materialChamsFlat;
			hidden_material = materialChamsFlatIgnorez;
			break;
	}

	visible_material->AlphaModulate(1.0f);
	hidden_material->AlphaModulate(1.0f);

	if (entity == localplayer)
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::localplayerColor.Color(entity));
		Color color = visColor;
		color *= 0.45f;

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
	}
	else if (Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::allyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::allyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);
	}
	else if (!Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::enemyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::enemyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);
	}
	else
	{
		return;
	}

	if (entity->GetImmune())
	{
		visible_material->AlphaModulate(0.5f);
		hidden_material->AlphaModulate(0.5f);
	}

	if (!Settings::ESP::Filters::legit && (Settings::ESP::Chams::type == ChamsType::CHAMS_XQZ || Settings::ESP::Chams::type == ChamsType::CHAMS_FLAT_XQZ))
	{
		modelRender->ForcedMaterialOverride(hidden_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}

	modelRender->ForcedMaterialOverride(visible_material);

}



static void DrawRecord(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
/*
	if (!Settings::LagComp::enabled)
        return;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;
	if (LagComp::ticks.empty())
		return;

 	IMaterial* visible_material = materialChams;
	Color visColor = Color::FromImColor(Settings::ESP::Chams::backtrackColor.Color(localplayer));
 	visible_material->ColorModulate(visColor);
 	visible_material->AlphaModulate(0.1f);
	auto &tick = LagComp::ticks.back();
    for (auto& tick : LagComp::ticks)
    {
	for (auto &record : tick.records)
	{
		if (!record.boneMatrix)
			continue;

		(Vector)pInfo.origin = record.origin;
		modelRender->ForcedMaterialOverride(visible_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, (matrix3x4_t*)record.boneMatrix);
		}
	}*/


/*
  if (pInfo.entity_index < engine->GetMaxClients() &&
      pInfo.entity_index == engine->GetLocalPlayer()) {

    if (Settings::BackTrack::Chams::enabled) {
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
  }*/
}

static void DrawWeapon(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Weapon::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = materialChamsWeapons;

	if (!Settings::ESP::Chams::Weapon::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Weapon::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Weapon::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Weapon::type == WeaponType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Weapon::type == WeaponType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

static void DrawArms(const ModelRenderInfo_t& pInfo)
{
	if (!Settings::ESP::Chams::Arms::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial* mat = materialChamsArms;

	if (!Settings::ESP::Chams::Arms::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Arms::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Arms::type == ArmsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Arms::type == ArmsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

void Chams::DrawModelExecute(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!engine->IsInGame())
		return;

	if (!Settings::ESP::enabled)
		return;

	if (!pInfo.pModel)
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(pInfo.entity_index);

	static bool materialsCreated = false;
	if (!materialsCreated)
	{
    //material_backtrack = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"),false, true, true, true, true); //say hi OwO
		materialChams = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsIgnorez = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
		materialChamsFlat = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsFlatIgnorez = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
		materialChamsArms = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsWeapons = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialsCreated = true;
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);


	if (modelName.find(XORSTR("models/player")) != std::string::npos){
		DrawPlayer(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}
	else if (modelName.find(XORSTR("arms")) != std::string::npos)
		DrawArms(pInfo);
	else if (modelName.find(XORSTR("weapon")) != std::string::npos)
		DrawWeapon(pInfo);
}
