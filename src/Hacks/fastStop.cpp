
#include "../Utils/math.h"
#include "../interfaces.h"
#include "fastStop.h"
#include "../settings.h"

bool Settings::AutoStrafe::fastStop = false;

//NOT IMPLEMENTED YET!
//Half ass pasted from a UC thread!
void fastStop::CreateMove(CUserCmd* cmd)
    {

	  if (!engine->IsInGame())
		  return;


    if(!Settings::AutoStrafe::fastStop)
      return;


    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	  if (!localplayer || !localplayer->GetAlive())
		  return;


		QAngle direction;
    Vector velocity = localplayer->GetVelocity();
		Math::VectorAngles(velocity, direction);
		float speed = velocity.Length2D();

		direction.z = cmd->viewangles.z - direction.z;

		Vector forward;
		Math::VectorAngles(forward, direction);

		Vector right = (forward + 0.217812) * -speed;
		Vector left = (forward + -0.217812) * -speed;

		Vector move_forward = (forward + 0.217812) * -speed;
		Vector move_backward = (forward + -0.217812) * -speed;

		if (!(cmd->buttons & IN_MOVELEFT))
		{
			cmd->sidemove += +left.y;
		}

		if (!(cmd->buttons & IN_MOVERIGHT))
		{
			cmd->sidemove -= -right.y;
		}

		if (!(cmd->buttons & IN_FORWARD))
		{
			if (cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT || Settings::AutoStrafe::enabled)
				return;

			cmd->forwardmove += +move_forward.x;
		}

		if (!(cmd->buttons & IN_BACK))
		{
			if (cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT || Settings::AutoStrafe::enabled)
				return;

			cmd->forwardmove -= -move_backward.x;
		}
    }
