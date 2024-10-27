#ifndef _PLANCOLL_H_
#define _PLANCOLL_H_

#include "common.h"

int PLANCOLL_DoesStraightLinePathExist(Position *startPos, Position *endPos, int collideType);
int PLANCOLL_DoesLOSExistFinal(Position *startPos, Position *endPos, int collideType, int passThroughHit, int zoffset);

#endif
