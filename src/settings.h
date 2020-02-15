#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "ImGUI/imgui.h"
#include "Utils/util_sdk.h"
#include "Utils/util_items.h"
#include "Utils/util.h"

#include "SDK/IClientEntity.h"
#include "SDK/IInputSystem.h"
#include "SDK/definitions.h"
#include "SDK/Materialsystem_config.h"


enum class ResolverType : int
{
	Type1,
	Type2,
	pResolver,
	Type3,
};

enum class DrawingBackend : int {
    SURFACE = 0,
    IMGUI,

    NUMBER_OF_TYPES
};

enum class SmoothType : int
{
	SLOW_END,
	CONSTANT,
	FAST_END,
};

enum class TracerType : int
{
	BOTTOM,
	CURSOR,
};

enum class ClanTagType : int
{
	STATIC,
	MARQUEE,
	WORDS,
	LETTERS,
};

enum class AutostrafeType : int
{
	AS_FORWARDS,
	AS_BACKWARDS,
	AS_LEFTSIDEWAYS,
	AS_RIGHTSIDEWAYS,
	AS_RAGE,
};

enum class ChamsType : int
{
	CHAMS,
	CHAMS_XQZ,
	CHAMS_FLAT,
	CHAMS_FLAT_XQZ,
};

enum class BoxType : int
{
	FLAT_2D,
	FRAME_2D,
	BOX_3D,
	HITBOXES,
};

enum class SpriteType : int
{
	SPRITE_TUX,
};

enum class Sound : int {

	NONE,
	SPONGEBOB,
	HALFLIFE,
	HALFLIFE2,
	HALFLIFE3,
	HALFLIFE4,
	BBGUNBELL,
	DOPAMINE,
	WUB,
	PEDOYES,
	MEME,
	ERROR,
	ORCHESTRAL,
	GAMESENSE,

};

enum class BarType : int
{
	VERTICAL,
	VERTICAL_RIGHT,
	HORIZONTAL,
	HORIZONTAL_UP,
	INTERWEBZ,
};

enum class BarColorType : int
{
	STATIC_COLOR,
	HEALTH_BASED,
};

enum class TeamColorType : int
{
	ABSOLUTE,
	RELATIVE,
};

enum class ArmsType : int
{
	DEFAULT,
	WIREFRAME,
	NONE,
};

enum class WeaponType : int
{
	DEFAULT,
	WIREFRAME,
	NONE,
};

enum class SmokeType : int
{
	WIREFRAME,
	NONE,
};


enum class AimbotType : int
{
	LEGIT,
	RAGE,
};

enum class AimTargetType : int
{
	FOV,
	DISTANCE,
	REAL_DISTANCE,
	HP,
};

enum class SpammerType : int
{
	SPAMMER_NONE,
	SPAMMER_NORMAL,
	SPAMMER_POSITIONS,
};

enum class ShowedAngle : int
{
    REAL,
    FAKE,
    BOTH,
};

enum class AntiAimType: int
{
	RAGE,
	LEGIT,
  DUMP,
  CUSTOM,
  FREESTAND,
};


struct AimbotWeapon_t
{
	bool enabled,
		 silent,
		 friendly,
		 closestBone,
	     engageLock,
		 engageLockTR,
		 aimkeyOnly,
		 smoothEnabled,
		 smoothSaltEnabled,
		 errorMarginEnabled,
		 autoAimEnabled,
		 aimStepEnabled,
		 rcsEnabled,
		 rcsAlwaysOn,
		 spreadLimitEnabled,
		 hitChanceEnabled,
		 autoPistolEnabled,
		 autoShootEnabled,
		 autoScopeEnabled,
		 noShootEnabled,
		 ignoreJumpEnabled,
		 ignoreEnemyJumpEnabled,
		 smokeCheck,
		 flashCheck,
		 autoWallEnabled,
		 autoAimRealDistance,
		 autoSlow,
		 predEnabled,
		 scopeControlEnabled;
	int engageLockTTR = 700;
	Bone bone = BONE_HEAD;
	SmoothType smoothType = SmoothType::SLOW_END;
	ButtonCode_t aimkey = ButtonCode_t ::MOUSE_MIDDLE;
	float smoothAmount = 1.0f,
		  smoothSaltMultiplier = 0.0f,
		  errorMarginValue = 0.0f,
		  autoAimFov = 180.0f,
		  aimStepMin = 25.0f,
		  aimStepMax = 35.0f,
		  rcsAmountX = 2.0f,
		  rcsAmountY = 2.0f,
		  autoWallValue = 10.0f,
		  spreadLimit = 1.0f,
		  HeadMultiPoint = 0.6f,
		  hitChance = 80.0f;
	bool desiredBones[31];

	bool operator == (const AimbotWeapon_t& another) const
	{
		for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		{
			if( this->desiredBones[bone] != another.desiredBones[bone] )
				return false;
		}

		return this->enabled == another.enabled &&
			this->silent == another.silent &&
			this->friendly == another.friendly &&
			this->closestBone == another.closestBone &&
			this->engageLock == another.engageLock &&
			this->engageLockTR == another.engageLockTR &&
			this->engageLockTTR == another.engageLockTTR &&
			this->bone == another.bone &&
			this->aimkey == another.aimkey &&
			this->aimkeyOnly == another.aimkeyOnly &&
			this->smoothEnabled == another.smoothEnabled &&
			this->smoothAmount == another.smoothAmount &&
			this->smoothType == another.smoothType &&
			this->smoothSaltEnabled == another.smoothSaltEnabled &&
			this->smoothSaltMultiplier == another.smoothSaltMultiplier &&
			this->errorMarginEnabled == another.errorMarginEnabled &&
			this->errorMarginValue == another.errorMarginValue &&
			this->autoAimEnabled == another.autoAimEnabled &&
			this->autoAimFov == another.autoAimFov &&
			this->aimStepEnabled == another.aimStepEnabled &&
			this->aimStepMin == another.aimStepMin &&
			this->aimStepMax == another.aimStepMax &&
			this->rcsEnabled == another.rcsEnabled &&
			this->rcsAlwaysOn == another.rcsAlwaysOn &&
			this->rcsAmountX == another.rcsAmountX &&
			this->rcsAmountY == another.rcsAmountY &&
			this->autoPistolEnabled == another.autoPistolEnabled &&
			this->autoShootEnabled == another.autoShootEnabled &&
			this->autoScopeEnabled == another.autoScopeEnabled &&
			this->noShootEnabled == another.noShootEnabled &&
			this->ignoreJumpEnabled == another.ignoreJumpEnabled &&
			this->ignoreEnemyJumpEnabled == another.ignoreEnemyJumpEnabled &&
			this->smokeCheck == another.smokeCheck &&
			this->flashCheck == another.flashCheck &&
			this->spreadLimitEnabled == another.spreadLimitEnabled &&
			this->spreadLimit == another.spreadLimit &&
			this->HeadMultiPoint == another.HeadMultiPoint &&
			this->hitChanceEnabled == another.hitChanceEnabled &&
			this->hitChance == another.hitChance &&
			this->autoWallEnabled == another.autoWallEnabled &&
			this->autoWallValue == another.autoWallValue &&
			this->autoSlow == another.autoSlow &&
			this->predEnabled == another.predEnabled &&
			this->autoAimRealDistance == another.autoAimRealDistance &&
			this->scopeControlEnabled == another.scopeControlEnabled;
	}
} const defaultSettings{};


class ColorVar
{
public:
	ImColor color;
	bool rainbow;
	float rainbowSpeed;

	ColorVar() {}

	ColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
	}

	ImColor Color()
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : this->color;
		result.Value.w = this->color.Value.w;
		return result;
	}
};

class HealthColorVar : public ColorVar
{
public:
	bool hp;

	HealthColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->hp = false;
	}

	ImColor Color(C_BasePlayer* player)
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : (this->hp ? Color::ToImColor(Util::GetHealthColor(player)) : this->color);
		result.Value.w = this->color.Value.w;
		return result;
	}
};
namespace Settings
{
	namespace UI
	{
		extern ColorVar mainColor;
		extern ColorVar bodyColor;
		extern ColorVar fontColor;
		extern ColorVar accentColor;

		namespace Windows
		{
			namespace Colors
			{
				extern int posX;
				extern int posY;
				extern int sizeX;
				extern int sizeY;
				extern bool open;
				extern bool reload; // True on config load, used to change Window Position.
			}
			namespace Config
			{
				extern int posX;
				extern int posY;
				extern int sizeX;
				extern int sizeY;
				extern bool open;
				extern bool reload; // True on config load, used to change Window Position.
			}
			namespace Main
			{
				extern int posX;
				extern int posY;
				extern int sizeX;
				extern int sizeY;
				extern bool open;
				extern bool reload; // True on config load, used to change Window Position.
			}
			namespace Playerlist
			{
				extern int posX;
				extern int posY;
				extern int sizeX;
				extern int sizeY;
				extern bool open;
				extern bool reload; // True on config load, used to change Window Position.
			}
			namespace Skinmodel
			{
				extern int posX;
				extern int posY;
				extern int sizeX;
				extern int sizeY;
				extern bool open;
				extern bool reload; // True on config load, used to change Window Position.
			}
			namespace Spectators
			{
				extern int posX;
				extern int posY;
				extern int sizeX;
				extern int sizeY;
				extern bool reload; // True on config load, used to change Window Position.
			}
		}
		namespace Fonts
		{
			namespace ESP
			{
				extern char* family;
				extern int size;
				extern int flags;
			}
		}
	}

	namespace Aimbot
	{
		extern bool enabled;
		extern bool silent;
		extern bool friendly;
		extern Bone bone;
		extern ButtonCode_t aimkey;
    //extern curtarget;
    extern AimbotType type;
		extern bool aimkeyOnly;
    extern int targetAimbot;
    /*namespace Multipoint
		{
			extern float scale;
		}
    */
		namespace Smooth
		{
			extern bool enabled;
			extern float value;
			extern SmoothType type;

			namespace Salting
			{
				extern bool enabled;
				extern float multiplier;
			}
		}

		namespace ErrorMargin
		{
			extern bool enabled;
			extern float value;
		}

		namespace AutoAim
		{
			extern bool enabled;
			extern float fov;
      extern float HeadMultiPoint;
			extern bool realDistance;
			extern bool closestBone;
			extern bool desiredBones[];
			extern bool engageLock;
			extern bool engageLockTR;
			extern int engageLockTTR;
		}

		namespace AutoWall
		{
			extern bool enabled;
			extern float value;
		}

		namespace AimStep
		{
			extern bool enabled;
			extern float min;
			extern float max;
		}

		namespace RCS
		{
			extern bool enabled;
			extern bool always_on;
			extern float valueX;
			extern float valueY;
		}

		namespace AutoPistol
		{
			extern bool enabled;
		}

		namespace AutoShoot
		{
			extern bool enabled;
			extern bool velocityCheck;
			extern bool autoscope;
		}

		namespace AutoCrouch
		{
			extern bool enabled;
		}

		namespace AutoSlow
		{
			extern bool enabled;
			extern bool goingToSlow;
		}

		namespace NoShoot
		{
			extern bool enabled;
		}

		namespace IgnoreJump
		{
			extern bool enabled;
		}

		namespace IgnoreEnemyJump
		{
			extern bool enabled;
		}

		namespace SmokeCheck
		{
			extern bool enabled;
		}

		namespace FlashCheck
		{
			extern bool enabled;
		}

		namespace SpreadLimit
		{
			extern bool enabled;
			extern float value;
		}

		namespace HitChance
		{
			extern bool enabled;
			extern float value;
		}

		namespace Prediction
		{
			extern bool enabled;
		}

		namespace ScopeControl
		{
			extern bool enabled;
		}

		extern std::unordered_map<ItemDefinitionIndex, AimbotWeapon_t, Util::IntHash<ItemDefinitionIndex>> weapons;
	}

	namespace Triggerbot
	{
		extern bool enabled;
		extern ButtonCode_t key;

		namespace Filters
		{
			extern bool enemies;
			extern bool allies;
			extern bool walls;
			extern bool smokeCheck;
			extern bool flashCheck;
			extern bool head;
			extern bool chest;
			extern bool stomach;
			extern bool arms;
			extern bool legs;
		}

		namespace RandomDelay
		{
			extern bool enabled;
			extern int lowBound; // in ms
			extern int highBound;// in ms
			extern int lastRoll;
		}
	}


namespace AntiAim
{
		extern bool enabled;
		extern AntiAimType type;


		extern float yaw;
    extern float off1;
    extern float off2;
    extern float roff1;
    extern float roff2;
    extern int fakeL;
    extern int fakeR;
    extern bool swap;
		extern ButtonCode_t left;
		extern ButtonCode_t right;

		namespace AutoDisable
    {
        extern bool knifeHeld;
    }


		namespace Freestanding
		{
			extern bool enabled;
		}

		namespace States
		{
			extern bool enabled;

			namespace Stand
			{
				extern AntiAimType type;
				extern float angle;
			} // namespace Stand

			namespace Walk
			{
				extern AntiAimType type;
				extern float angle;
			} // namespace Walk

			namespace Run
			{
				extern AntiAimType type;
				extern float angle;
			} // namespace Run

			namespace Air
			{
				extern AntiAimType type;
				extern float angle;
			} // namespace Air
		}

    namespace Desync
    {
      extern float amount;
      extern float time;
      extern float interval;
    }

	namespace LBYBreaker
	{
		extern bool enabled;
		extern float offset;
		extern bool manual;
		extern float VelMax;
		extern float NextUpdate;
		extern float SwitchTick;
    extern bool custom;
		extern float CurTime;
	}
}

    namespace Resolver
    {
	    extern bool resolveAll;
      extern float x;
	    extern float y;
	    extern int RandMax;
	    extern ResolverType type;
	}

	namespace ESP
	{
		extern bool enabled;
        extern DrawingBackend backend;
		extern ButtonCode_t key;
		extern TeamColorType teamColorType;
		extern HealthColorVar enemyColor;
		extern HealthColorVar allyColor;
		extern HealthColorVar enemyVisibleColor;
		extern HealthColorVar allyVisibleColor;
		extern HealthColorVar ctColor;
		extern HealthColorVar tColor;
		extern HealthColorVar ctVisibleColor;
		extern HealthColorVar tVisibleColor;
		extern ColorVar bombColor;
		extern ColorVar bombDefusingColor;
		extern ColorVar hostageColor;
		extern ColorVar defuserColor;
		extern ColorVar weaponColor;
		extern ColorVar chickenColor;
		extern ColorVar fishColor;
		extern ColorVar smokeColor;
		extern ColorVar decoyColor;
		extern ColorVar flashbangColor;
		extern ColorVar grenadeColor;
		extern ColorVar molotovColor;
		extern ColorVar mineColor;
		extern ColorVar chargeColor;
		extern ColorVar allyInfoColor;
		extern ColorVar enemyInfoColor;
		extern HealthColorVar localplayerColor;

		namespace Glow
		{
			extern bool enabled;
			extern HealthColorVar allyColor;
			extern HealthColorVar enemyColor;
			extern HealthColorVar enemyVisibleColor;
			extern ColorVar weaponColor;
			extern ColorVar grenadeColor;
			extern ColorVar defuserColor;
			extern ColorVar chickenColor;
			extern HealthColorVar localplayerColor;
		}

		namespace Filters
		{
			extern bool legit;
			extern bool visibilityCheck;
			extern bool smokeCheck;
			extern bool flashCheck;
			extern bool enemies;
			extern bool allies;
			extern bool bomb;
			extern bool hostages;
			extern bool defusers;
			extern bool weapons;
			extern bool chickens;
			extern bool fishes;
			extern bool throwables;
			extern bool localplayer;
		}

		namespace Info
		{
			extern bool name;
			extern bool clan;
			extern bool steamId;
			extern bool rank;
			extern bool health;
			extern bool armor;
			extern bool weapon;
			extern bool scoped;
			extern bool reloading;
			extern bool flashed;
			extern bool planting;
			extern bool hasDefuser;
			extern bool defusing;
			extern bool grabbingHostage;
			extern bool rescuing;
			extern bool location;
			extern bool money;
			extern bool missedShots;
		}

		namespace Skeleton
		{
			extern bool enabled;
			extern ColorVar enemyColor;
			extern ColorVar allyColor;
		}

		namespace Boxes
		{
			extern bool enabled;
			extern BoxType type;
		}

		namespace Sprite
		{
			extern bool enabled;
			extern SpriteType type;
		}

		namespace Bars
		{
			extern bool enabled;
			extern BarType type;
			extern BarColorType colorType;
		}

		namespace Tracers
		{
			extern bool enabled;
			extern TracerType type;
		}

		namespace BulletTracers
		{
			extern bool enabled;
		}

		namespace Bomb
		{
			extern bool enabled;
		}

		namespace FOVCrosshair
		{
			extern bool enabled;
			extern bool filled;
			extern ColorVar color;
		}

		namespace Chams
		{
			extern bool enabled;
			extern HealthColorVar allyColor;
			extern HealthColorVar allyVisibleColor;
			extern HealthColorVar enemyColor;
			extern HealthColorVar enemyVisibleColor;
			extern HealthColorVar localplayerColor;
			extern HealthColorVar backtrackColor;
			extern ChamsType type;

			namespace Arms
			{
				extern bool enabled;
				extern ColorVar color;
				extern ArmsType type;
			}

			namespace Weapon
			{
				extern bool enabled;
				extern ColorVar color;
				extern WeaponType type;
			}
		}


    namespace ZeusRadius
    {
      extern bool enabled;
			extern ColorVar ZeusColor;
    }
		namespace Sounds
		{
			extern bool enabled;
			extern int time;
		}

		namespace Hitmarker
		{
			extern bool enabled;
			extern bool enemies;
			extern bool allies;
			extern ColorVar color;
			extern int duration;
			extern int size;
			extern int innerGap;

			namespace Damage
			{
				extern bool enabled;
			}
			namespace Sounds {
				extern bool enabled;
				extern Sound sound;
			}
		}

		namespace HeadDot
		{
			extern bool enabled;
			extern float size;
		}

		namespace backtrack
		{
		  extern bool enabled;

		}

		namespace Spread
		{
			extern bool enabled; // show current spread
			extern bool spreadLimit; // show spreadLimit value
			extern ColorVar color;
			extern ColorVar spreadLimitColor;
		}

		namespace DangerZone
		{
			extern int drawDist;
			extern bool drawDistEnabled;
			extern bool upgrade;
			extern bool lootcrate;
			extern bool radarjammer;
			extern bool barrel;
			extern bool ammobox;
			extern bool safe;
			extern bool dronegun;
			extern bool drone;
			extern bool cash;
			extern bool tablet;
			extern bool healthshot;
			extern bool melee;
			extern ColorVar upgradeColor;
			extern ColorVar lootcrateColor;
			extern ColorVar radarjammerColor;
			extern ColorVar barrelColor;
			extern ColorVar ammoboxColor;
			extern ColorVar safeColor;
			extern ColorVar dronegunColor;
			extern ColorVar droneColor;
			extern ColorVar cashColor;
			extern ColorVar tabletColor;
			extern ColorVar healthshotColor;
			extern ColorVar meleeColor;
		}
	}

	namespace MaterialConfig {
		extern bool enabled;
		extern MaterialSystem_Config_t config;
	}

	namespace Dlights
	{
		extern bool enabled;
		extern float radius;
	}

	namespace Eventlog
	{
		extern bool showEnemies;
		extern bool showTeammates;
		extern bool showLocalplayer;
		extern bool filterDamage;
		extern bool filterPurchases;
		extern bool filterBombsite;
		extern bool filterDefuse;
		extern bool filterPlant;
		extern bool filterPickup;
		extern bool filterDrop;
		extern float duration;
		extern float lines;
		extern ColorVar color;
	}

	namespace Spammer
	{
		extern SpammerType type;
		extern bool say_team;

		namespace KillSpammer
		{
			extern bool enabled;
			extern bool sayTeam;
			extern std::vector<std::string> messages;
		}

		namespace RadioSpammer
		{
			extern bool enabled;
		}

		namespace NormalSpammer
		{
			extern std::vector<std::string> messages;
		}

		namespace PositionSpammer
		{
			extern int team;
			extern bool showName;
			extern bool showWeapon;
			extern bool showRank;
			extern bool showWins;
			extern bool showHealth;
			extern bool showMoney;
			extern bool showLastplace;
		}
	}

	namespace BHop
	{
		extern bool enabled;

		namespace Chance
		{
			extern bool enabled;
			extern int value;
		}

		namespace Hops
		{
			extern bool enabledMax;
			extern int Max;
			extern bool enabledMin;
			extern int Min;
		}
	}

  	namespace FakeWalk
	{
		extern bool enabled;
		extern ButtonCode_t key;
	}

  	namespace FakeCrouch
	{
		extern bool enabled;
		extern ButtonCode_t key;
	}


	namespace NoDuckCooldown
	{
		extern bool enabled;
	}
/*
	namespace LagComp
	{
	  extern bool enabled;
	  extern int time;
	  extern float fov;
	}
*/
	namespace AutoStrafe
	{
		extern bool enabled;
		extern bool fastStop;
		extern AutostrafeType type;
		extern bool silent;
	}

	namespace Noflash
	{
		extern bool enabled;
		extern float value;
	}


	namespace FOVChanger
	{
		extern bool enabled;
		extern bool viewmodelEnabled;
		extern float value;
		extern float viewmodelValue;
		extern bool ignoreScope;
	}

	namespace Radar
	{
		extern bool enabled;
		extern float zoom;
		extern bool enemies;
		extern bool allies;
		extern bool bomb;
		extern bool defuser;
		extern bool legit;
		extern bool visibilityCheck;
		extern bool smokeCheck;
		extern TeamColorType teamColorType;
		extern HealthColorVar enemyColor;
		extern HealthColorVar enemyVisibleColor;
		extern HealthColorVar allyColor;
		extern HealthColorVar allyVisibleColor;
		extern HealthColorVar tColor;
		extern HealthColorVar tVisibleColor;
		extern HealthColorVar ctColor;
		extern HealthColorVar ctVisibleColor;
		extern ColorVar bombColor;
		extern ColorVar bombDefusingColor;
		extern ColorVar defuserColor;
		extern float iconsScale;
		extern ImVec2 pos;

		namespace InGame
		{
			extern bool enabled;
		}
	}

	namespace Recoilcrosshair
	{
		extern bool enabled;
		extern bool showOnlyWhenShooting;
	}

	namespace Airstuck
	{
		extern bool enabled;
		extern ButtonCode_t key;
	}

	namespace Autoblock
	{
		extern bool enabled;
		extern ButtonCode_t key;
	}

	namespace Skinchanger
	{
		namespace Skins
		{
			extern bool enabled;
			extern bool perTeam;
		}

		namespace Models
		{
			extern bool enabled;
		}

		extern std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsCT;
		extern std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsT;
	}

	namespace ShowRanks
	{
		extern bool enabled;
	}

	namespace ShowSpectators
	{
		extern bool enabled;
	}

	namespace ClanTagChanger
	{
		extern char value[30];
		extern bool animation;
		extern int animationSpeed;
		extern bool enabled;
		extern ClanTagType type;
	}

	namespace FakeVote
	{
		extern char message[128];
		extern char cmd[128];
	}

	namespace View
	{
		namespace NoAimPunch
		{
			extern bool enabled;
		}

		namespace NoViewPunch
		{
			extern bool enabled;
		}
	}

	namespace FakeLag
	{
		extern bool enabled;
		extern int value;
		extern int minTicks;
		extern int maxTicks;
		extern bool adaptive;
    extern bool lagSpike;
    extern bool bSend;

    namespace States
		{
			extern bool enabled;

			namespace Standing
			{
				extern bool enabled;
				extern int value;
			}

			namespace Moving
			{
				extern bool enabled;
				extern int value;
			}

			namespace Air
			{
				extern bool enabled;
				extern int value;
			}
		}
	}

	namespace FakeLatency
  {
    extern float value;
  }

	namespace BackTrack {
		extern bool enabled;
		extern int time;
		extern float fov;
		namespace Chams {
			extern bool enabled;
			extern bool drawlastonly;
			extern ColorVar firstcolor;
			extern ColorVar fadecolor;
			extern float alpha;
		}
	}

	namespace AutoAccept
	{
		extern bool enabled;
	}

	namespace NoSky
	{
		extern bool enabled;
		extern ColorVar color;
	}

	namespace SkyBox
	{
		extern bool enabled;
		extern int skyBoxNumber; // number in skyBoxNames
	}

	namespace ASUSWalls
	{
		extern bool enabled;
		extern ColorVar color;
	}

	namespace NoScopeBorder
	{
		extern bool enabled;
	}

	namespace SniperCrosshair
	{
		extern bool enabled;
	}

	namespace AutoDefuse
	{
		extern bool enabled;
		extern bool silent;
	}

	namespace NoSmoke
	{
		extern bool enabled;
		extern SmokeType type;
	}

	namespace ScreenshotCleaner
	{
		extern bool enabled;
	}

	namespace EdgeJump
	{
		extern bool enabled;
		extern ButtonCode_t key;
	}

	namespace NameStealer
	{
		extern bool enabled;
		extern int team;
	}

	namespace ThirdPerson
	{
		extern bool enabled;
		extern float distance;
        extern ShowedAngle type;
        extern ButtonCode_t key;
	}

	namespace JumpThrow
	{
		extern bool enabled;
		extern ButtonCode_t key;
	}

	namespace ProfileChanger
	{
		extern int coinID;
		extern int musicID;
		extern int compRank;
		extern int weaponStatus;
		extern int weaponRarity;
	}

	namespace NoFall
	{
		extern bool enabled;
	}

	namespace SvCheats
	{
		extern bool enabled;

  	  namespace gravity{
        extern bool enabled;
        extern int amount;
    }

    namespace viewmodel{
        extern bool enabled;
        extern float fov;
        extern float x;
        extern float y;
        extern float z;
    }



	}

	namespace DisablePostProcessing
	{
		extern bool enabled;
	}

	namespace GrenadeHelper
	{
		extern std::vector<GrenadeInfo> grenadeInfos;
		extern bool enabled;
		extern bool onlyMatchingInfos;
		extern bool aimAssist;
		extern float aimStep;
		extern float aimDistance;
		extern float aimFov;
		extern ColorVar aimDot;
		extern ColorVar aimLine;
		extern ColorVar infoHE;
		extern ColorVar infoSmoke;
		extern ColorVar infoFlash;
		extern ColorVar infoMolotov;
		extern std::string actMapName;
	}

	namespace GrenadePrediction
	{
		extern bool enabled;
		extern ColorVar color;
	}

	namespace TracerEffects
	{
		extern bool enabled;
		extern bool serverSide;
		extern TracerEffects_t effect;
		extern int frequency;

	}
	namespace AutoKnife
 	{
 		extern bool enabled;
 		extern bool onKey;

 		namespace Filters
 		{
 			extern bool enemies;
 			extern bool allies;
 		}
 	}


 	namespace LogShots
	{
		extern bool enabled;
	}

	namespace AngleIndicator
	{
		extern bool enabled;
	}
    namespace Debug
    {
        namespace AutoWall
        {
            extern bool debugView;
        }
		namespace AutoAim
		{
			extern bool drawTarget;
			extern Vector target;
		}
		namespace BoneMap
		{
			extern bool draw;
			extern bool justDrawDots;
			extern int modelID; // in econItemMap, not itemdefindex
		}
		namespace AnimLayers
		{
			extern bool draw;
		}
    }

	void LoadDefaultsOrSave(std::string path);
	void LoadConfig(std::string path);
	void LoadSettings();
	void DeleteConfig(std::string path);
	void SaveGrenadeInfo(std::string path);
	void LoadGrenadeInfo(std::string path);
}
