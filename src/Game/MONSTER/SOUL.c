#include "common.h"
#include "Game/STATE.h"
#include "Game/GAMELOOP.h"
#include "Game/MONSTER.h"
#include "Game/SAVEINFO.h"
#include "Game/PHYSICS.h"
#include "Game/MATH3D.h"
#include "Game/MONSTER/MONAPI.h"
#include "Game/MONSTER/MONLIB.h"
#include "Game/MONSTER/MONMSG.h"
#include "Game/MONSTER/MONSENSE.h"

void SOUL_QueueHandler(Instance *instance)
{
    MonsterVars *mv;
    Message *message;

    mv = (MonsterVars *)instance->extraData;

    while ((message = DeMessageQueue(&mv->messageQueue)))
    {
        if ((message != NULL) && (message->ID == 0x100000D))
        {
            if (instance->currentMainState != MONSTER_STATE_DEAD)
            {
                MON_SwitchState(instance, MONSTER_STATE_DEAD);
            }

            continue;
        }

        MON_DefaultMessageHandler(instance, message);
    }
}

void SOUL_Physics(Instance *instance, long time)
{
    MonsterVars *mv;
    int a;

    mv = (MonsterVars *)instance->extraData;

    a = -4;

    if (((mv->speed & 31)) < 16)
    {
        a = 4;
    }

    instance->zAccl += a;

    mv->speed += 1;

    PhysicsMove(instance, &instance->position, time);
}

void SOUL_Fade(Instance *instance)
{
    MonsterVars *mv;
    unsigned long time;

    mv = (MonsterVars *)instance->extraData;

    time = MON_GetTime(instance);

    if (!(gameTrackerX.gameFlags & 0x80))
    {
        if (mv->damageTimer < time)
        {
            if (instance->fadeValue == 0)
            {
                MON_StartSpecialFade(instance, 4096, 100);
            }

            if (instance->fadeValue >= 4096)
            {
                instance->flags2 |= 0x8000000;

                MORPH_SetupInstanceFlags(instance);

                instance->fadeValue = 0;
            }
        }
    }
    else
    {
        mv->damageTimer = time + 6500;
    }
}

void SOUL_MoveToDest(Instance *instance, long maxAccel, long time)
{
    MonsterVars *mv;

    mv = (MonsterVars *)instance->extraData;

    instance->xAccl = (mv->destination.x - instance->position.x) - instance->xVel;
    instance->yAccl = (mv->destination.y - instance->position.y) - instance->yVel;
    instance->zAccl = (mv->destination.z - instance->position.z) - instance->zVel;

    if (instance->xAccl < -maxAccel)
    {
        instance->xAccl = -maxAccel;
    }
    else if (maxAccel < instance->xAccl)
    {
        instance->xAccl = maxAccel;
    }

    if (instance->yAccl < -maxAccel)
    {
        instance->yAccl = -maxAccel;
    }
    else if (maxAccel < instance->yAccl)
    {
        instance->yAccl = maxAccel;
    }

    if (instance->zAccl < -maxAccel)
    {
        instance->zAccl = -maxAccel;
    }
    else if (maxAccel < instance->zAccl)
    {
        instance->zAccl = maxAccel;
    }

    SOUL_Physics(instance, time);
}

void SOUL_MovePastWall(Instance *instance, Instance *sucker)
{
    PCollideInfo pcollideInfo;
    Position newPos;
    Position oldPos;
    Position delta;
    MATRIX *to;
    MATRIX *from;

    from = sucker->matrix;

    to = instance->matrix;

    oldPos.x = from->t[0];
    oldPos.y = from->t[1];
    oldPos.z = from->t[2];

    newPos.x = to->t[0];
    newPos.y = to->t[1];
    newPos.z = to->t[2];

    pcollideInfo.newPoint = (SVECTOR *)&newPos;
    pcollideInfo.oldPoint = (SVECTOR *)&oldPos;

    PHYSICS_CheckLineInWorld(instance, &pcollideInfo);

    SUB_SVEC(Position, &delta, Position, &newPos, Position, &instance->position);

    COLLIDE_UpdateAllTransforms(instance, (SVECTOR *)&delta);

    COLLIDE_MoveAllTransforms(instance, &delta);

    instance->position = newPos;
}

void SOUL_Init(Instance *instance)
{
    MonsterVars *mv;

    mv = (MonsterVars *)instance->extraData;

    MON_DefaultInit(instance);

    mv->mvFlags |= 0x200000 | 0x880;

    instance->maxXVel = 600;
    instance->maxYVel = 600;
    instance->maxZVel = 600;

    instance->flags2 |= 0x20000;

    mv->speed = 0;

    mv->damageTimer = MON_GetTime(instance) + 6500;

    if (instance->parent != NULL)
    {
        mv->soulID = instance->parent->introUniqueID;
    }

    if (!(instance->flags & 0x2))
    {
        instance->flags2 |= 0x8000000;

        MON_SwitchState(instance, MONSTER_STATE_IDLE);
    }
}

void SOUL_CleanUp(Instance *instance)
{
    MON_CleanUp(instance);
}

void SOUL_BirthEntry(Instance *instance)
{
    MonsterVars *mv;

    mv = (MonsterVars *)instance->extraData;

    instance->maxXVel = 15;
    instance->maxYVel = 15;
    instance->maxZVel = 17;

    instance->zAccl = 0;
    instance->zVel = 0;

    instance->position.z += 120;

    mv->generalTimer = MON_GetTime(instance) + 1500;
}

void SOUL_Birth(Instance *instance)
{
    MonsterVars *mv;

    instance->zAccl = 0;

    mv = (MonsterVars *)instance->extraData;

    SOUL_Physics(instance, gameTrackerX.timeMult);

    if (MON_GetTime(instance) > mv->generalTimer)
    {
        MON_SwitchState(instance, MONSTER_STATE_IDLE);
    }

    SOUL_QueueHandler(instance);
}

void SOUL_SoulSuckEntry(Instance *instance)
{
    instance->maxXVel = 600;
    instance->maxYVel = 600;
    instance->maxZVel = 17;

    instance->flags &= ~0x800;
}

long SOUL_CalcAccel(long delta, long vel, long magnitude)
{
    long rv;

    if (delta > 0)
    {
        if (vel < 0)
        {
            rv = (delta * 16) / magnitude;
        }
        else
        {
            rv = (delta * 5) / magnitude;
        }
    }
    else if (vel > 0)
    {
        rv = (delta * 16) / magnitude;
    }
    else
    {
        rv = (delta * 5) / magnitude;
    }

    return rv;
}

INCLUDE_ASM("asm/nonmatchings/Game/MONSTER/SOUL", SOUL_SoulSuck);

void SOUL_WanderEntry(Instance *instance)
{
    MonsterVars *mv;

    mv = (MonsterVars *)instance->extraData;

    instance->maxXVel = 10;
    instance->maxYVel = 10;
    instance->maxZVel = 17;

    if (!(mv->mvFlags & 0x40000))
    {
        if (instance->intro != NULL)
        {
            MON_GetRandomDestinationInWorld(instance, &instance->intro->position, mv->wanderRange);
        }
        else
        {
            MON_GetRandomDestinationInWorld(instance, &instance->position, mv->wanderRange);
        }
    }
}

void SOUL_Wander(Instance *instance)
{
    MonsterVars *mv;

    mv = (MonsterVars *)instance->extraData;

    if (!(mv->mvFlags & 0x40000))
    {
        if (instance->intro != NULL)
        {
            MON_GetRandomDestinationInWorld(instance, &instance->intro->position, mv->wanderRange);

            instance->zAccl = 0;
        }
        else
        {
            MON_GetRandomDestinationInWorld(instance, &instance->position, mv->wanderRange);

            instance->zAccl = 0;
        }
    }
    else if (MATH3D_LengthXY(mv->destination.x - instance->position.x, mv->destination.y - instance->position.y) < 100)
    {
        MON_SwitchState(instance, MONSTER_STATE_IDLE);

        instance->zAccl = 0;
    }
    else
    {
        instance->xAccl = mv->destination.x - instance->position.x - instance->xVel;
        instance->yAccl = mv->destination.y - instance->position.y - instance->yVel;

        if (instance->xAccl >= -2)
        {
            if (instance->xAccl >= 3)
            {
                instance->xAccl = 2;
            }
        }
        else
        {
            instance->xAccl = -2;
        }

        if (instance->yAccl >= -2)
        {
            if (instance->yAccl >= 3)
            {
                instance->yAccl = 2;
            }
        }
        else
        {
            instance->yAccl = -2;
        }

        instance->zAccl = 0;
    }

    SOUL_Physics(instance, gameTrackerX.timeMult);

    if (!(mv->mvFlags & 0x4))
    {
        if (!(instance->flags2 & 0x8000000))
        {
            SOUL_Fade(instance);
        }

        if (mv->enemy != NULL)
        {
            MON_SwitchState(instance, MONSTER_STATE_FLEE);
        }
    }

    SOUL_QueueHandler(instance);
}

void SOUL_FleeEntry(Instance *instance)
{
    instance->maxXVel = 15;
    instance->maxYVel = 15;
    instance->maxZVel = 17;

    MON_FleeEntry(instance);
}

void SOUL_Flee(Instance *instance)
{
    MonsterVars *mv;
    short temp; // not from decls.h

    mv = (MonsterVars *)instance->extraData;

    if (mv->enemy == NULL)
    {
        MON_SwitchState(instance, MONSTER_STATE_IDLE);
    }
    else
    {
        Instance *enemy;
        int dx;
        int dy;

        enemy = mv->enemy->instance;

        temp = MONSENSE_GetClosestFreeDirection(instance, MATH3D_AngleFromPosToPos(&enemy->position, &instance->position), 500);

        dx = (rsin(temp) * 2000) / 4096;
        dy = -(rcos(temp) * 2000) / 4096;

        instance->xAccl = dx - instance->xVel;
        instance->yAccl = dy - instance->yVel;

        if (instance->xAccl < -2)
        {
            instance->xAccl = -2;
        }
        else if (instance->xAccl > 2)
        {
            instance->xAccl = 2;
        }

        if (instance->yAccl < -2)
        {
            instance->yAccl = -2;
        }
        else if (instance->yAccl > 2)
        {
            instance->yAccl = 2;
        }

        instance->zAccl = 0;
    }

    SOUL_Physics(instance, gameTrackerX.timeMult);

    SOUL_QueueHandler(instance);

    if (!(instance->flags2 & 0x8000000))
    {
        SOUL_Fade(instance);
    }
}

void SOUL_IdleEntry(Instance *instance)
{
    MonsterVars *mv;

    mv = (MonsterVars *)instance->extraData;

    instance->maxXVel = 15;
    instance->maxYVel = 15;
    instance->maxZVel = 17;

    mv->generalTimer = MON_GetTime(instance) + 3000 + (rand() % 3000);

    mv->mvFlags &= ~0x40000;
}

void SOUL_Idle(Instance *instance)
{
    MonsterVars *mv;
    long xAccl;
    long yAccl;

    mv = (MonsterVars *)instance->extraData;

    xAccl = 3;

    if (instance->xVel >= -3)
    {
        xAccl = -3;

        if (instance->xVel < 4)
        {
            xAccl = -instance->xVel;
        }
    }

    yAccl = 3;

    if (instance->yVel >= -3)
    {
        yAccl = -3;

        if (instance->yVel < 4)
        {
            yAccl = -instance->yVel;
        }
    }

    instance->xAccl = xAccl;
    instance->yAccl = yAccl;
    instance->zAccl = 0;

    SOUL_Physics(instance, gameTrackerX.timeMult);

    if (!(mv->mvFlags & 0x4))
    {
        if (mv->enemy != NULL)
        {
            MON_SwitchState(instance, MONSTER_STATE_FLEE);
        }
        else if (MON_GetTime(instance) > mv->generalTimer)
        {
            MON_SwitchState(instance, MONSTER_STATE_WANDER);
        }

        if (!(instance->flags2 & 0x8000000))
        {
            SOUL_Fade(instance);
        }
    }

    SOUL_QueueHandler(instance);
}

void SOUL_ReanimateEntry(Instance *instance)
{
    MonsterVars *mv;
    Instance *body;

    mv = (MonsterVars *)instance->extraData;

    body = INSTANCE_Find(mv->soulID);

    mv->destination.x = body->position.x;
    mv->destination.y = body->position.y;
    mv->destination.z = body->position.z + 160;

    MON_TurnOffBodySpheres(instance);
}

void SOUL_Reanimate(Instance *instance)
{
    MonsterVars *mv;
    Instance *body;

    mv = (MonsterVars *)instance->extraData;

    SOUL_MoveToDest(instance, 16, gameTrackerX.timeMult);

    if (MATH3D_LengthXY(instance->position.x - mv->destination.x, instance->position.y - mv->destination.y) < 250)
    {
        if (mv->soulID != 0)
        {
            body = INSTANCE_Find(mv->soulID);

            if (body != NULL)
            {
                INSTANCE_Post(body, 0x100000D, 0);
            }

            SAVE_DeleteInstance(instance);
        }

        MON_KillMonster(instance);
    }
    else
    {
        while (DeMessageQueue(&mv->messageQueue) != NULL);
    }
}

void SOUL_Effect()
{
}
