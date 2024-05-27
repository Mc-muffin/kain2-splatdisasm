#include "common.h"
#include "Game/MONSTER.h"
#include "Game/MONSTER/MONTABLE.h"
#include "Game/MONSTER/MONAPI.h"

static MonsterFunctionTable DefaultFunctionTable = {
    MON_DefaultInit,
    MON_CleanUp,
    MON_DamageEffect,
    MonsterQuery,
    MonsterMessage,
    NULL,
    monVersion,
    NULL,
};

EXTERN STATIC MonsterStateFunction DefaultStateTable[31];

EXTERN STATIC MonsterChoice functionChoiceTable[8924+6];

void MONTABLE_SetupTablePointer(Object *object)
{
    long whatAmI;
    int i; // not from decls.h

    whatAmI = ((MonsterAttributes *)(object->data))->whatAmI;

    for (i = 0; functionChoiceTable[i].whatAmI != 0; i++)
    {
        if (whatAmI == functionChoiceTable[i].whatAmI)
        {
            object->relocModule = functionChoiceTable[i].table;
        }
    }
}

MonsterStateFunction *MONTABLE_GetStateFuncs(Instance *instance, int state)
{
    MonsterFunctionTable *ft;

    ft = (MonsterFunctionTable *)instance->object->relocModule;

    if (ft != NULL)
    {
        MonsterStateChoice *choice;

        choice = ft->stateFuncs;

        while (choice->state != -1)
        {
            if (state == choice->state)
            {
                return (MonsterStateFunction *)&choice->functions.entryFunction;
            }

            choice++;
        }
    }

    return (MonsterStateFunction *)&DefaultStateTable[state].entryFunction;
}

void *MONTABLE_GetDamageEffectFunc(Instance *instance)
{
    MonsterFunctionTable *ft;

    ft = (MonsterFunctionTable *)instance->object->relocModule;

    if ((ft != NULL) && (ft->damageEffectFunc != NULL))
    {
        return (void *)ft->damageEffectFunc;
    }

    return (void *)DefaultFunctionTable.damageEffectFunc;
}

void *MONTABLE_GetInitFunc(Instance *instance)
{
    MonsterFunctionTable *ft;

    ft = (MonsterFunctionTable *)instance->object->relocModule;

    if ((ft != NULL) && (ft->initFunc != NULL))
    {
        return (void *)ft->initFunc;
    }

    return (void *)DefaultFunctionTable.initFunc;
}

void *MONTABLE_GetCleanUpFunc(Instance *instance)
{
    MonsterFunctionTable *ft;

    ft = (MonsterFunctionTable *)instance->object->relocModule;

    if ((ft != NULL) && (ft->cleanUpFunc != NULL))
    {
        return (void *)ft->cleanUpFunc;
    }

    return (void *)DefaultFunctionTable.cleanUpFunc;
}

void MONTABLE_SetQueryFunc(Instance *instance)
{
    MonsterFunctionTable *ft;

    ft = (MonsterFunctionTable *)instance->object->relocModule;

    if ((ft != NULL) && (ft->queryFunc != NULL))
    {
        instance->queryFunc = ft->queryFunc;
    }
}

void MONTABLE_SetMessageFunc(Instance *instance)
{
    MonsterFunctionTable *ft;

    ft = (MonsterFunctionTable *)instance->object->relocModule;

    if ((ft != NULL) && (ft->messageFunc != NULL))
    {
        instance->messageFunc = ft->messageFunc;
    }
}
