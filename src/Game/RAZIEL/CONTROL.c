#include "common.h"
#include "Game/RAZIEL/CONTROL.h"
#include "Game/PLAYER.h"

EXTERN STATIC Force *ExternalForces;

void SetPhysics(Instance *instance, short gravity, long x, long y, long z)
{
    SetExternalForce(ExternalForces, 0, 0, gravity, 0, 4096);

    instance->xVel = x;
    instance->yVel = y;
    instance->zVel = z;
}

void ResetPhysics(Instance *instance, short gravity)
{
    int i;

    SetExternalForce(ExternalForces, 0, 0, gravity, 0, 4096);

    for (i = 1; i < 4; i++)
    {
        SetExternalForce(&ExternalForces[i], 0, 0, 0, 0, 0);
    }

    instance->xVel = 0;
    instance->yVel = 0;
    instance->zVel = 0;

    instance->xAccl = 0;
    instance->yAccl = 0;
    instance->zAccl = gravity;
}

void SetDampingPhysics(Instance *instance, int damping)
{
    instance->xAccl = -((instance->xVel * damping) / 4096);
    instance->yAccl = -((instance->yVel * damping) / 4096);
    instance->zAccl = -((instance->zVel * damping) / 4096);

    SetExternalForce(ExternalForces, (short)instance->xAccl, (short)instance->yAccl, (short)instance->zAccl, 0, 4096);
}

void SetImpulsePhysics(Instance *instance, Player *player)
{
    int Dot;
    int vLength;

    if (!(player->Mode & 0x40000))
    {
        Dot = -instance->offset.x * player->iVelocity.x;
        Dot += -instance->offset.y * player->iVelocity.y;
        Dot += -instance->offset.z * player->iVelocity.z;

        vLength = player->iVelocity.x * player->iVelocity.x;
        vLength += player->iVelocity.y * player->iVelocity.y;
        vLength += player->iVelocity.z * player->iVelocity.z;

        if (vLength != 0)
        {
            instance->position.x += instance->offset.x + ((Dot * player->iVelocity.x) / vLength);
            instance->position.y += instance->offset.y + ((Dot * player->iVelocity.y) / vLength);
            instance->position.z += instance->offset.z + ((Dot * player->iVelocity.z) / vLength);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/Game/RAZIEL/CONTROL", SetDropPhysics);

INCLUDE_ASM("asm/nonmatchings/Game/RAZIEL/CONTROL", InitExternalForces);

INCLUDE_ASM("asm/nonmatchings/Game/RAZIEL/CONTROL", SetExternalForce);

INCLUDE_ASM("asm/nonmatchings/Game/RAZIEL/CONTROL", SetExternalTransitionForce);

INCLUDE_ASM("asm/nonmatchings/Game/RAZIEL/CONTROL", ProcessPhysics);

INCLUDE_ASM("asm/nonmatchings/Game/RAZIEL/CONTROL", ApplyExternalLocalForces);
