#ifndef POKEHEARTGOLD_UNK_0205FD20_H
#define POKEHEARTGOLD_UNK_0205FD20_H

#include "map_object.h"
#include "task.h"

void sub_02060F78(LocalMapObject *object);
BOOL sub_02060D94(LocalMapObject *object, int x, int y, int z);
BOOL sub_02060DEC(LocalMapObject *object, int x, int z, int direction);
BOOL sub_02060EEC(LocalMapObject *object, int tile);
int sub_02060FA8(LocalMapObject *object, int direction);
int sub_02060FE0(LocalMapObject *object, int direction);
void sub_02061070(LocalMapObject *object);
void sub_02061190(int direction, VecFx32 *pos, int value);
u32 sub_020611F4(u32 direction);
BOOL sub_02060E54(LocalMapObject *object, int tile);
BOOL sub_02060EBC(LocalMapObject *object, int tile);
int GetDeltaXByFacingDirection(int direction);
int GetDeltaYByFacingDirection(int direction);
void sub_020611C8(int x, int y, VecFx32 *a2);
BOOL sub_0206121C(TaskManager *taskman, VecFx32 *pos);

#endif // POKEHEARTGOLD_UNK_0205FD20_H
