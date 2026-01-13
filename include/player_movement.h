#ifndef POKEHEARTGOLD_PLAYER_MOVEMENT_H
#define POKEHEARTGOLD_PLAYER_MOVEMENT_H

#include "player_avatar.h"

void PlayerAvatar_MoveControl(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags, int allowIdleTransition);
BOOL PlayerMovement_CanMove(PlayerAvatar *playerAvatar, int direction);
void PlayerAvatar_UpdateMovement(PlayerAvatar *playerAvatar);
void PlayerMovement_Reset(PlayerAvatar *playerAvatar);
BOOL PlayerMovement_IsIdle(PlayerAvatar *playerAvatar);
void PlayerMovement_ForceStopAndFace(PlayerAvatar *playerAvatar, int direction);
int PlayerMovement_CheckCollision(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
int PlayerMovement_GetInputDirection(PlayerAvatar *playerAvatar, int direction, u16 inputFlags);
BOOL PlayerMovement_IsScriptActive(PlayerAvatar *playerAvatar);
BOOL PlayerMovement_IsRunning(PlayerAvatar *playerAvatar);
int PlayerMovement_GetMovementResult(PlayerAvatar *playerAvatar, int direction);
u32 PlayerMovement_CalcMovementCommand(PlayerAvatar *playerAvatar, u16 inputFlags, u16 inputHeld, s32 state, BOOL hasRunningShoes, int collisionResult);
BOOL PlayerMovement_IsScriptInit(PlayerAvatar *playerAvatar);
void PlayerMovement_SetMovementCommand(PlayerAvatar *playerAvatar, u32 movementCmd);
int PlayerMovement_GetFacingTileBehavior(PlayerAvatar *playerAvatar, u8 tile);
void PlayerAvatar_GetCoordsInFront(PlayerAvatar *playerAvatar, int *x, int *z);

#endif // POKEHEARTGOLD_PLAYER_MOVEMENT_H
