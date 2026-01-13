#ifndef POKEHEARTGOLD_UNK_0205CB48_H
#define POKEHEARTGOLD_UNK_0205CB48_H

#include "player_avatar.h"

void PlayerAvatar_MoveControl(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *a1, int a2, u16 a3, u16 a4, int a5);
BOOL sub_0205CBE4(PlayerAvatar *playerAvatar, int direction);
void PlayerAvatar_UpdateMovement(PlayerAvatar *playerAvatar);
void sub_0205CF44(PlayerAvatar *playerAvatar);
BOOL sub_0205CF60(PlayerAvatar *playerAvatar);
void sub_0205CFBC(PlayerAvatar *playerAvatar, int direction);
int sub_0205DA34(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
int sub_0205DD94(PlayerAvatar *playerAvatar, int direction, u16 inputFlags);
BOOL sub_0205DE38(PlayerAvatar *playerAvatar);
BOOL sub_0205DE98(PlayerAvatar *playerAvatar);
int sub_0205DEC0(PlayerAvatar *playerAvatar, int direction);
u32 sub_0205DF0C(PlayerAvatar *playerAvatar, u16 inputFlags, u16 a2, s32 state, BOOL hasRunningShoes, int collisionResult);
BOOL sub_0205DFC8(PlayerAvatar *playerAvatar);
void sub_0205DFD4(PlayerAvatar *playerAvatar, u32 movementCmd);
int sub_0205DFEC(PlayerAvatar *playerAvatar, u8 tile);
void PlayerAvatar_GetCoordsInFront(PlayerAvatar *playerAvatar, int *x, int *z);

#endif // POKEHEARTGOLD_UNK_0205CB48_H
