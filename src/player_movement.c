#include "player_movement.h"

#include "global.h"

#include "constants/sndseq.h"

#include "assert.h"
#include "follow_mon.h"
#include "game_stats.h"
#include "gymmick_init.h"
#include "map_object.h"
#include "metatile_behavior.h"
#include "overlay_01.h"
#include "overlay_01_021F1AFC.h"
#include "overlay_01_022053EC.h"
#include "overlay_04.h"
#include "unk_02005D10.h"
#include "unk_02054648.h"
#include "unk_0205FD20.h"
#include "unk_02062108.h"
#include "unk_0206D494.h"

// Static function declarations
static BOOL PlayerMovement_CheckCanMove(PlayerAvatar *playerAvatar, int direction);
static void PlayerMovement_SetInputFlags(PlayerAvatar *playerAvatar, int direction, int inputHeld, u16 inputFlags);
static void PlayerMovement_UpdateStoppedFlag(PlayerAvatar *playerAvatar);
static void PlayerMovement_PlayFootstepSounds(PlayerAvatar *playerAvatar);
static void PlayerMovement_PlayTileSound(LocalMapObject *mapObject, PlayerAvatar *playerAvatar);
static BOOL PlayerMovement_CheckSpecialTile(PlayerAvatar *playerAvatar, int direction);
static int PlayerMovement_GetTileMovementType(PlayerAvatar *playerAvatar, int direction);
static BOOL PlayerMovement_ExecuteTileCallback(PlayerAvatar *playerAvatar, int result, int direction);
static BOOL PlayerMovement_NormalTileCallback(PlayerAvatar *playerAvatar, int direction);
static BOOL PlayerMovement_IceTileCallback(PlayerAvatar *playerAvatar, int direction);
static BOOL PlayerMovement_SpinTileCallback(PlayerAvatar *playerAvatar, int direction);
static void PlayerMovement_ClearSlidingState(PlayerAvatar *playerAvatar);
static int PlayerMovement_GetHeightDelta(PlayerAvatar *playerAvatar, int direction);
static BOOL PlayerMovement_UpdateElevation(PlayerAvatar *playerAvatar, int elevationChange);
static void PlayerMovement_ExecuteSlide(PlayerAvatar *playerAvatar, int direction);
static void PlayerMovement_DispatchByState(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleWalking(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags);
static int PlayerMovement_DetermineWalkType(PlayerAvatar *playerAvatar, int direction);
static int PlayerMovement_MapWalkResult(int result);
static u32 PlayerMovement_GetTileSpeed(LocalMapObject *mapObject, u8 tile, u32 speed);
static int PlayerMovement_GetWalkOrTurn(PlayerAvatar *playerAvatar, int direction);
static void PlayerMovement_SetIdleCommand(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleWalkStep(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleTurn(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleBiking(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags);
static u32 PlayerMovement_GetBikeSpeed(PlayerAvatar *playerAvatar, int direction);
static BOOL PlayerMovement_IncreaseBikeMomentum(PlayerAvatar *playerAvatar);
static BOOL PlayerMovement_DecreaseBikeMomentum(PlayerAvatar *playerAvatar);
static void PlayerMovement_DispatchBikeMove(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags);
static int PlayerMovement_DetermineBikeType(PlayerAvatar *playerAvatar, int direction);
static int PlayerMovement_MapBikeResult(int result);
static int PlayerMovement_GetBikeMoveType(PlayerAvatar *playerAvatar, int direction);
static void PlayerMovement_SetBikeIdleCommand(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleBikeStep(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleBikeTurn(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_HandleBikeCoast(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags);
static void PlayerMovement_SetHeldMovement(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, u32 movementCmd);
static int PlayerMovement_CheckTerrainCollision(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL PlayerMovement_CheckJumpLedge(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL PlayerMovement_CheckSilentCollision(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL PlayerMovement_CheckBlockingTerrain(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL PlayerMovement_CheckBikeBlockingTerrain(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static int PlayerMovement_GetVerticalInput(u16 inputFlags);
static int PlayerMovement_GetHorizontalInput(u16 inputFlags);
static int PlayerMovement_ResolveDirection(PlayerAvatar *playerAvatar, int direction, u16 inputFlags);
static BOOL PlayerMovement_IsIdleMovement(int movementCmd);
static BOOL PlayerMovement_CheckRunButton(PlayerAvatar *playerAvatar, u16 inputFlags);
static void PlayerMovement_GetCoordsInDirection(PlayerAvatar *playerAvatar, int direction, int *x, int *z);
static void PlayerMovement_IncrementStepCounters(PlayerAvatar *playerAvatar);
static BOOL PlayerMovement_CheckForcedDismount(PlayerAvatar *playerAvatar, u16 tile, int direction);

// Typedef for callback functions
typedef BOOL (*MetatileCheckFunc)(u8 tile);
typedef BOOL (*MovementCallback)(PlayerAvatar *playerAvatar, int direction);

// Data tables
static const MovementCallback sMovementCallbacks[] = {
    PlayerMovement_NormalTileCallback,
    PlayerMovement_IceTileCallback,
    PlayerMovement_SpinTileCallback,
};

static const struct {
    MetatileCheckFunc func;
    int value;
} sMetatileCallbackTable[] = {
    { sub_0205B828, 1 },
    { NULL,         3 },
};

static const struct {
    u16 sound;
    u16 useAlt;
} sFootstepSounds[] = {
    { SEQ_SE_GS_ASHIOTO_A_WALK, 0 },
    { SEQ_SE_GS_ASHIOTO_A,      0 },
    { SEQ_SE_GS_OCHIBA,         0 },
    { SEQ_SE_GS_EDAPAKI,        0 },
    { SEQ_SE_GS_KUSA2,          0 },
    { SEQ_SE_GS_ASHIOTO_B,      1 },
    { SEQ_SE_GS_ASHIOTO_D,      1 },
    { SEQ_SE_GS_ASHIOTO_D_2,    0 },
    { SEQ_SE_GS_ASHIOTO_E,      0 },
    { SEQ_SE_GS_ASHIOTO_B,      1 },
    { SEQ_SE_GS_ASHIOTO_F,      1 },
    { SEQ_SE_GS_ASHIOTO_D,      0 },
    { SEQ_SE_GS_ASHIOTO_H,      0 },
    { SEQ_SE_GS_ASHIOTO_G,      0 },
    { SEQ_SE_DP_BOX02,          0 },
    { SEQ_SE_DP_BOX01,          0 },
};

void PlayerAvatar_MoveControl(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags, int allowIdleTransition) {
    int resolvedDir = direction;

    if (resolvedDir == -1) {
        resolvedDir = PlayerMovement_ResolveDirection(playerAvatar, inputHeld, inputFlags);
    }

    if (PlayerMovement_CheckCanMove(playerAvatar, resolvedDir) == FALSE) {
        return;
    }

    PlayerMovement_SetInputFlags(playerAvatar, resolvedDir, inputHeld, inputFlags);
    Field_PlayerAvatar_ApplyTransitionFlags(playerAvatar);

    if (PlayerMovement_CheckSpecialTile(playerAvatar, resolvedDir) == 1) {
        ov01_021F2F24(playerAvatar);
        return;
    }

    if (PlayerAvatar_GetState(playerAvatar) == 0) {
        if (PlayerMovement_DetermineWalkType(playerAvatar, resolvedDir) != 0) {
            ov01_021F2F24(playerAvatar);
        } else if (allowIdleTransition == 1) {
            ov01_021F2EDC(playerAvatar);
        }
    }

    PlayerMovement_DispatchByState(playerAvatar, fieldSysUnk2C, resolvedDir, inputHeld, inputFlags);
    PlayerMovement_UpdateStoppedFlag(playerAvatar);
    PlayerMovement_PlayFootstepSounds(playerAvatar);
}

BOOL PlayerMovement_CanMove(PlayerAvatar *playerAvatar, int direction) {
    return PlayerMovement_CheckCanMove(playerAvatar, direction);
}

static BOOL PlayerMovement_CheckCanMove(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == TRUE) {
        return TRUE;
    }

    if (direction == -1) {
        return FALSE;
    }

    u32 movementCmd = MapObject_GetMovementCommand(mapObject);
    if (PlayerMovement_IsIdleMovement(movementCmd) == TRUE) {
        int result = PlayerMovement_CheckCollision(playerAvatar, mapObject, direction);
        if (result == 0) {
            return TRUE;
        }
        if (result == 0x20 && PlayerAvatar_GetState(playerAvatar) == 2) {
            return TRUE;
        }
        return FALSE;
    }

    return FALSE;
}

static void PlayerMovement_SetInputFlags(PlayerAvatar *playerAvatar, int direction, int inputHeld, u16 inputFlags) {
    int vertDir = PlayerMovement_GetVerticalInput(inputFlags);
    int horizDir = PlayerMovement_GetHorizontalInput(inputFlags);
    PlayerAvatar_SetUnk28Unk2C(playerAvatar, vertDir, horizDir);
    PlayerAvatar_ClearFlag6(playerAvatar);
}

static void PlayerMovement_UpdateStoppedFlag(PlayerAvatar *playerAvatar) {
    if (PlayerAvatar_CheckFlag6(playerAvatar) == TRUE) {
        if (PlayerAvatar_GetUnk10(playerAvatar) == 1) {
            PlayerAvatar_SetFlag1(playerAvatar, FALSE);
        }
    }
}

static void PlayerMovement_PlayFootstepSounds(PlayerAvatar *playerAvatar) {
    if (PlayerAvatar_GetUnk10(playerAvatar) != 1) {
        return;
    }

    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    u8 tile = (u8)sub_0205F504(mapObject);
    u32 movementCmd = MapObject_GetMovementCommand(mapObject);
    int movementDir = sub_02062390(movementCmd);

    u8 facingTile;
    if (movementDir == -1) {
        facingTile = tile;
    } else {
        facingTile = sub_02060FA8(mapObject, movementDir);
    }

    if (sub_0205B984(tile) == TRUE) {
        PlaySE(SEQ_SE_DP_FOOT3_0);
    }

    if (sub_0205B7A4(tile) == TRUE) {
        PlaySE(SEQ_SE_DP_FOOT3_1);
    }

    sub_0205B798(tile);

    if (sub_0205B8AC(tile) == TRUE) {
        PlaySE(SEQ_SE_DP_MARSH_WALK);
    }

    movementCmd = MapObject_GetMovementCommand(mapObject);
    PlayerAvatar_GetState(playerAvatar);

    if (PlayerMovement_IsIdleMovement(movementCmd) == FALSE) {
        if (sub_0205B6F4(tile) == TRUE || sub_0205B6F4(facingTile) == TRUE) {
            PlaySE(SEQ_SE_DP_KUSA);
        }

        if (MetatileBehavior_IsEncounterGrass(tile) == TRUE || MetatileBehavior_IsEncounterGrass(facingTile) == TRUE) {
            PlaySE(SEQ_SE_GS_KUSA2);
        }
    }

    PlayerMovement_PlayTileSound(mapObject, playerAvatar);
}

static void PlayerMovement_PlayTileSound(LocalMapObject *mapObject, PlayerAvatar *playerAvatar) {
    int movementDir = sub_02062390(MapObject_GetMovementCommand(mapObject));
    u8 tile = (u8)sub_0205F504(mapObject);
    u8 facingTile = (u8)sub_0205F504(mapObject);

    if (sub_0205B6F4(facingTile) != FALSE) {
        return;
    }
    if (sub_0205B984(facingTile) == TRUE) {
        return;
    }
    if (sub_0205B7A4(facingTile) == TRUE) {
        return;
    }
    if (MetatileBehavior_IsEncounterGrass(facingTile) != FALSE) {
        return;
    }

    if (movementDir != -1) {
        tile = sub_02060FE0(mapObject, movementDir);
    }

    if (sFootstepSounds[tile].sound == SEQ_SE_GS_EDAPAKI) {
        PlayerAvatar_ResetUnkC(playerAvatar);
    } else {
        if (PlayerAvatar_GetUnk14(playerAvatar) == 0) {
            PlayerAvatar_ResetUnkC(playerAvatar);
        } else {
            PlayerAvatar_ToggleUnkC(playerAvatar);
        }
    }

    if (PlayerAvatar_GetUnkC(playerAvatar) != FALSE) {
        return;
    }

    s32 state = PlayerAvatar_GetState(playerAvatar);
    if (state == 1) {
        return;
    }
    if (state == 2) {
        return;
    }

    if (tile < 0x10) {
        if (tile == 0) {
            if (PlayerMovement_IsRunning(playerAvatar) == TRUE) {
                tile = 1;
            }
        }

        u16 sound;
        if (sFootstepSounds[tile].useAlt == 1) {
            sound = sFootstepSounds[tile].sound;
            sub_02006088(sound);
        } else {
            sound = sFootstepSounds[tile].sound;
            PlaySE(sound);
        }
        sub_02005BA8(sound);
        return;
    }
    GF_ASSERT(tile < 0x10);
}

void PlayerAvatar_UpdateMovement(PlayerAvatar *playerAvatar) {
    u32 movementState = PlayerAvatar_GetUnk10(playerAvatar);
    u32 movementPhase = PlayerAvatar_GetUnk14(playerAvatar);
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    PlayerAvatar_SetUnk14(playerAvatar, 0);

    int result = PlayerMovement_GetTileMovementType(playerAvatar, -1);
    if (result != 0 && result != 2) {
        PlayerAvatar_SetUnk14(playerAvatar, 2);
        return;
    }

    if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == FALSE) {
        switch (movementState) {
        case 0:
            return;
        case 1:
            if (PlayerMovement_IsIdleMovement(MapObject_GetMovementCommand(mapObject)) == TRUE) {
                return;
            }
            if (movementPhase == 0 || movementPhase == 3) {
                PlayerAvatar_SetUnk14(playerAvatar, 1);
                return;
            }
            PlayerAvatar_SetUnk14(playerAvatar, 2);
            return;
        case 2:
            PlayerAvatar_SetUnk14(playerAvatar, 2);
            return;
        }
        return;
    }

    if (MapObject_IsMovementPaused(mapObject) != TRUE) {
        return;
    }

    switch (movementState) {
    case 0:
        return;
    case 1:
        if (movementPhase == 0) {
            return;
        }
        if (movementPhase == 3) {
            PlayerAvatar_SetUnk14(playerAvatar, 0);
            return;
        }
        PlayerAvatar_SetUnk14(playerAvatar, 3);
        return;
    case 2:
        if (movementPhase == 0) {
            return;
        }
        if (movementPhase == 3) {
            PlayerAvatar_SetUnk14(playerAvatar, 0);
            return;
        }
        PlayerAvatar_SetUnk14(playerAvatar, 3);
        return;
    }
}

void PlayerMovement_Reset(PlayerAvatar *playerAvatar) {
    PlayerAvatar_SetUnk10(playerAvatar, 0);
    PlayerAvatar_SetUnk14(playerAvatar, 0);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
}

BOOL PlayerMovement_IsIdle(PlayerAvatar *playerAvatar) {
    u32 movementState = PlayerAvatar_GetUnk10(playerAvatar);
    u32 movementPhase = PlayerAvatar_GetUnk14(playerAvatar);

    if (movementState == 0) {
        return TRUE;
    }
    if (movementState == 2) {
        return TRUE;
    }

    if (movementState == 1) {
        if (movementPhase == 0 || movementPhase == 3) {
            return TRUE;
        }

        LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
        if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == TRUE) {
            return TRUE;
        }

        if (PlayerMovement_IsIdleMovement(MapObject_GetMovementCommand(mapObject)) == TRUE) {
            return TRUE;
        }
    }

    return FALSE;
}

void PlayerMovement_ForceStopAndFace(PlayerAvatar *playerAvatar, int direction) {
    PlayerAvatar_SetUnk10(playerAvatar, 0);
    PlayerAvatar_SetUnk14(playerAvatar, 0);
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    MapObject_SetFacingDirection(mapObject, direction);
    sub_0205F328(mapObject, 0);
    sub_02060F78(mapObject);
    MapObject_ClearHeldMovement(mapObject);
    u32 movementCmd = sub_0206234C(direction, 0);
    MapObject_SetHeldMovement(mapObject, movementCmd);
}

static BOOL PlayerMovement_CheckSpecialTile(PlayerAvatar *playerAvatar, int direction) {
    int result = PlayerMovement_GetTileMovementType(playerAvatar, direction);
    return PlayerMovement_ExecuteTileCallback(playerAvatar, result, direction);
}

static int PlayerMovement_GetTileMovementType(PlayerAvatar *playerAvatar, int direction) {
    int index = 0;
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    u16 tile = sub_0205F504(mapObject);

    if (PlayerMovement_CheckForcedDismount(playerAvatar, tile, direction) == TRUE) {
        return 2;
    }

    if (PlayerAvatar_CheckFlag1(playerAvatar) == TRUE) {
        return 0;
    }

    for (;;) {
        if (sMetatileCallbackTable[index].func((u8)tile) == TRUE) {
            return sMetatileCallbackTable[index].value;
        }
        index++;
        if (sMetatileCallbackTable[index].func == NULL) {
            break;
        }
    }

    return 0;
}

static BOOL PlayerMovement_ExecuteTileCallback(PlayerAvatar *playerAvatar, int result, int direction) {
    if (sMovementCallbacks[result](playerAvatar, direction) == TRUE) {
        return TRUE;
    }
    return FALSE;
}

static BOOL PlayerMovement_NormalTileCallback(PlayerAvatar *playerAvatar, int direction) {
    PlayerMovement_ClearSlidingState(playerAvatar);
    return FALSE;
}

static BOOL PlayerMovement_IceTileCallback(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    int nextDir = MapObject_GetNextFacingDirection(mapObject);
    int elevationChange = PlayerMovement_GetHeightDelta(playerAvatar, nextDir);
    int collisionResult = PlayerMovement_CheckCollision(playerAvatar, mapObject, nextDir);

    PlayerAvatar_SetFlag0(playerAvatar, TRUE);

    if (collisionResult != 0) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        PlayerAvatar_SetFlag1(playerAvatar, TRUE);
        PlayerAvatar_SetUnk10(playerAvatar, 0);

        if (sub_0206D494(fieldSystem) == FALSE) {
            PlayerMovement_ClearSlidingState(playerAvatar);
            return FALSE;
        }
        return TRUE;
    }

    if (PlayerMovement_UpdateElevation(playerAvatar, elevationChange) == FALSE) {
        PlayerMovement_ClearSlidingState(playerAvatar);
        int rvsDir = sub_020611F4(nextDir);

        if (PlayerMovement_CheckCollision(playerAvatar, mapObject, rvsDir) != 0) {
            return FALSE;
        }

        MapObject_SetFlagsBits(mapObject, (MapObjectFlagBits)(6 << 6));
        u32 movementCmd = sub_0206234C(rvsDir, 8);
        PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
        PlayerAvatar_SetFlag1(playerAvatar, TRUE);
        PlayerAvatar_SetFlag0(playerAvatar, TRUE);
        PlayerAvatar_SetUnk28Unk2C(playerAvatar, -1, -1);
    } else {
        MapObject_SetFlagsBits(mapObject, (MapObjectFlagBits)(6 << 6));
        PlayerMovement_ExecuteSlide(playerAvatar, nextDir);
    }

    PlayerAvatar_SetUnk10(playerAvatar, 1);
    return TRUE;
}

static BOOL PlayerMovement_SpinTileCallback(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    if (PlayerMovement_CheckCollision(playerAvatar, mapObject, 1) == 0) {
        u32 movementCmd = sub_0206234C(1, 0x14);
        PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
        PlayerAvatar_SetUnk10(playerAvatar, 1);
        PlayerAvatar_SetUnk24(playerAvatar, 3);
        return TRUE;
    }

    u32 movementCmd = sub_0206234C(1, 1);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    PlayerAvatar_SetUnk10(playerAvatar, 0);
    PlayerAvatar_SetFlag2(playerAvatar, FALSE);
    return TRUE;
}

static void PlayerMovement_ClearSlidingState(PlayerAvatar *playerAvatar) {
    if (PlayerAvatar_CheckFlag0(playerAvatar) != TRUE) {
        return;
    }

    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    MapObject_ClearFlagsBits(mapObject, (MapObjectFlagBits)(6 << 6));

    if (PlayerAvatar_CheckFlag7(playerAvatar) == FALSE) {
        PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    }

    PlayerAvatar_SetFlag0(playerAvatar, FALSE);
    PlayerAvatar_SetFlag7(playerAvatar, FALSE);
    PlayerAvatar_SetFlag5(playerAvatar, FALSE);
}

static int PlayerMovement_GetHeightDelta(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
    VecFx32 currentPos;
    VecFx32 nextPos;

    MapObject_CopyPositionVector(mapObject, &currentPos);
    nextPos = currentPos;
    sub_02061190(direction, &nextPos, 1 << 14);

    if (sub_0206121C((TaskManager *)fieldSystem, &nextPos) == FALSE || currentPos.y == nextPos.y) {
        return 0;
    }

    if (currentPos.y > nextPos.y) {
        return 2;
    }

    return 1;
}

static BOOL PlayerMovement_UpdateElevation(PlayerAvatar *playerAvatar, int elevationChange) {
    s32 elevation = PlayerAvatar_GetUnk24(playerAvatar);

    if (elevationChange == 1) {
        elevation--;
        if (elevation < 0) {
            return FALSE;
        }
    } else if (elevationChange == 2) {
        elevation++;
        if (elevation > 3) {
            elevation = 3;
        }
    }

    PlayerAvatar_SetUnk24(playerAvatar, elevation);
    return TRUE;
}

static void PlayerMovement_ExecuteSlide(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    int speed = 0x10;

    s32 elevation = PlayerAvatar_GetUnk24(playerAvatar);
    switch (elevation) {
    case 1:
        speed = 0x50;
        break;
    case 2:
        speed = 0x14;
        break;
    case 3:
        speed = 0x14;
        break;
    }

    u32 movementCmd = sub_0206234C(direction, speed);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);

    FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
    if (FollowMon_IsActive(fieldSystem) != FALSE) {
        int prevX = MapObject_GetPreviousXCoord(mapObject);
        int prevZ = MapObject_GetPreviousZCoord(mapObject);
        ov01_02205990(movementCmd, prevX, prevZ, (FollowMon *)((u8 *)fieldSystem + 0xe4));
    }
}

static void PlayerMovement_DispatchByState(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags) {
    s32 state = PlayerAvatar_GetState(playerAvatar);
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    switch (state) {
    case 0:
    case 2:
    case 3:
        PlayerMovement_HandleWalking(playerAvatar, mapObject, fieldSysUnk2C, direction, inputHeld, inputFlags);
        return;
    case 1:
        PlayerMovement_HandleBiking(playerAvatar, mapObject, fieldSysUnk2C, direction, inputHeld, inputFlags);
        return;
    default:
        GF_ASSERT(FALSE);
        return;
    }
}

static void PlayerMovement_HandleWalking(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags) {
    int result = PlayerMovement_DetermineWalkType(playerAvatar, direction);

    switch (result) {
    case 0:
        PlayerMovement_SetIdleCommand(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        return;
    case 1:
        PlayerMovement_HandleWalkStep(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        return;
    case 2:
        PlayerMovement_HandleTurn(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        return;
    }
}

static int PlayerMovement_DetermineWalkType(PlayerAvatar *playerAvatar, int direction) {
    int result = PlayerMovement_GetWalkOrTurn(playerAvatar, direction);
    int movementState = PlayerMovement_MapWalkResult(result);
    PlayerAvatar_SetUnk10(playerAvatar, movementState);
    return result;
}

static int PlayerMovement_MapWalkResult(int result) {
    switch (result) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    default:
        GF_ASSERT(FALSE);
        return 0;
    }
}

static u32 PlayerMovement_GetTileSpeed(LocalMapObject *mapObject, u8 tile, u32 speed) {
    return speed;
}

static int PlayerMovement_GetWalkOrTurn(PlayerAvatar *playerAvatar, int direction) {
    if (direction == -1) {
        PlayerAvatar_SetUnk10(playerAvatar, 0);
        return 0;
    }

    int facingDir = PlayerAvatar_GetFacingDirection(playerAvatar);
    if (facingDir != direction) {
        if (PlayerAvatar_GetUnk10(playerAvatar) != 1) {
            PlayerAvatar_SetUnk10(playerAvatar, 2);
            return 2;
        }
    }

    PlayerAvatar_SetUnk10(playerAvatar, 1);
    return 1;
}

static void PlayerMovement_SetIdleCommand(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    int facingDir = MapObject_GetFacingDirection(mapObject);
    u32 movementCmd = sub_0206234C(facingDir, 0);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
}

static void PlayerMovement_HandleWalkStep(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    BOOL playedSound = FALSE;
    u32 movementCmd;

    int collisionResult = PlayerMovement_CheckCollision(playerAvatar, mapObject, direction);

    if (PlayerAvatar_GetState(playerAvatar) != 2) {
        if ((collisionResult & 4) != 0) {
            movementCmd = 0x38;
        } else if (collisionResult != 0) {
            movementCmd = 0x1c;
            if ((collisionResult & 8) == 0) {
                FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
                if (FieldSystem_IsSaveGymmickTypeEqualTo(fieldSystem, 7) != FALSE) {
                    int xCoord = MapObject_GetXCoord(mapObject);
                    int deltaX = GetDeltaXByFacingDirection(direction);
                    int zCoord = MapObject_GetZCoord(mapObject);
                    int deltaY = GetDeltaYByFacingDirection(direction);
                    ov04_022566EC(fieldSystem, xCoord + deltaX, zCoord + deltaY);
                } else {
                    PlaySE(SEQ_SE_DP_WALL_HIT);
                }
            }
            MapObject_SetNextFacingDirection(mapObject, direction);
            playedSound = TRUE;
        } else {
            int speed = 0xc;
            PlayerSaveData *playerSaveData = PlayerAvatar_GetPlayerSaveData(playerAvatar);

            if (PlayerSaveData_CheckRunningShoes(playerSaveData) == TRUE) {
                if (PlayerMovement_CheckRunButton(playerAvatar, inputFlags) == TRUE) {
                    speed = 0x58;
                }
            }

            s32 tile = sub_0205F504(mapObject);
            movementCmd = PlayerMovement_GetTileSpeed(mapObject, tile, speed);
            PlayerMovement_IncrementStepCounters(playerAvatar);
            PlayerAvatar_SetFlag6(playerAvatar);
        }
    } else {
        if (collisionResult == 0 || collisionResult == 0x20) {
            s32 tile = sub_0205F504(mapObject);
            movementCmd = PlayerMovement_GetTileSpeed(mapObject, tile, 0x10);
            PlayerMovement_IncrementStepCounters(playerAvatar);
            PlayerAvatar_SetFlag6(playerAvatar);
        } else {
            movementCmd = 0x1c;
            if ((collisionResult & 8) == 0) {
                PlaySE(SEQ_SE_DP_WALL_HIT);
            }
            MapObject_SetNextFacingDirection(mapObject, direction);
        }
    }

    u32 finalCmd = sub_0206234C(direction, movementCmd);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, finalCmd);

    if (playedSound == FALSE) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        int prevX = MapObject_GetPreviousXCoord(mapObject);
        int prevZ = MapObject_GetPreviousZCoord(mapObject);
        ov01_02205990(finalCmd, prevX, prevZ, (FollowMon *)((u8 *)fieldSystem + 0xe4));
    }
}

static void PlayerMovement_HandleTurn(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    u32 movementCmd = sub_0206234C(direction, 0x28);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
    MapObject_SetNextFacingDirection(mapObject, direction);
    PlayerAvatar_ResetUnkC(playerAvatar);
    PlayerAvatar_ToggleUnkC(playerAvatar);
}

static void PlayerMovement_HandleBiking(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags) {
    PlayerMovement_DispatchBikeMove(playerAvatar, mapObject, fieldSysUnk2C, direction, inputHeld, inputFlags);
}

static u32 PlayerMovement_GetBikeSpeed(PlayerAvatar *playerAvatar, int direction) {
    int speed = 0x4c;

    s32 momentum = PlayerAvatar_GetUnk24(playerAvatar);
    switch (momentum) {
    case 1:
        speed = 0x10;
        break;
    case 2:
        speed = 0x50;
        break;
    case 3:
        speed = 0x14;
        break;
    }

    return sub_0206234C(direction, speed);
}

static BOOL PlayerMovement_IncreaseBikeMomentum(PlayerAvatar *playerAvatar) {
    s32 momentum = PlayerAvatar_Unk24AddWithCeiling(playerAvatar, 1, 3);

    if (PlayerAvatar_CheckFlag2(playerAvatar) == FALSE) {
        if (momentum >= 2) {
            PlayerAvatar_SetFlag2(playerAvatar, TRUE);
        }
    }

    if (momentum == 3) {
        return TRUE;
    }

    return FALSE;
}

static BOOL PlayerMovement_DecreaseBikeMomentum(PlayerAvatar *playerAvatar) {
    BOOL result = TRUE;
    s32 momentum = PlayerAvatar_GetUnk24(playerAvatar) - 1;

    if (momentum < 0) {
        momentum = 0;
        result = FALSE;
    }

    PlayerAvatar_SetUnk24(playerAvatar, momentum);

    if (PlayerAvatar_CheckFlag2(playerAvatar) == TRUE) {
        if (momentum == 0) {
            PlayerAvatar_SetFlag2(playerAvatar, FALSE);
        }
    }

    return result;
}

static void PlayerMovement_DispatchBikeMove(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *fieldSysUnk2C, int direction, u16 inputHeld, u16 inputFlags) {
    int result = PlayerMovement_DetermineBikeType(playerAvatar, direction);

    switch (result) {
    case 0:
        PlayerMovement_SetBikeIdleCommand(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        break;
    case 1:
        PlayerMovement_HandleBikeStep(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        break;
    case 2:
        PlayerMovement_HandleBikeTurn(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        break;
    case 3:
        PlayerMovement_HandleBikeCoast(playerAvatar, mapObject, direction, inputHeld, inputFlags);
        break;
    }
}

static int PlayerMovement_DetermineBikeType(PlayerAvatar *playerAvatar, int direction) {
    int result = PlayerMovement_GetBikeMoveType(playerAvatar, direction);
    int movementState = PlayerMovement_MapBikeResult(result);
    PlayerAvatar_SetUnk10(playerAvatar, movementState);
    return result;
}

static int PlayerMovement_MapBikeResult(int result) {
    switch (result) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 1;
    default:
        GF_ASSERT(FALSE);
        return 0;
    }
}

static int PlayerMovement_GetBikeMoveType(PlayerAvatar *playerAvatar, int direction) {
    s32 momentum = PlayerAvatar_GetUnk24(playerAvatar);

    if (direction == -1) {
        if (momentum < 2) {
            PlayerAvatar_SetUnk10(playerAvatar, 0);
            return 0;
        }
        PlayerAvatar_SetUnk10(playerAvatar, 1);
        return 3;
    }

    int facingDir = PlayerAvatar_GetFacingDirection(playerAvatar);
    if (facingDir != direction) {
        if (PlayerAvatar_GetUnk10(playerAvatar) != 1) {
            if (momentum < 2) {
                PlayerAvatar_SetUnk10(playerAvatar, 2);
                return 2;
            }
            PlayerAvatar_SetUnk10(playerAvatar, 1);
            return 3;
        }
    }

    PlayerAvatar_SetUnk10(playerAvatar, 1);
    return 1;
}

static void PlayerMovement_SetBikeIdleCommand(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    int facingDir = MapObject_GetFacingDirection(mapObject);
    u32 movementCmd = sub_0206234C(facingDir, 0);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
}

static void PlayerMovement_HandleBikeStep(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    u32 movementCmd;

    int collisionResult = PlayerMovement_CheckCollision(playerAvatar, mapObject, direction);

    if ((collisionResult & 4) != 0) {
        movementCmd = sub_0206234C(direction, 0x38);
        PlayerMovement_IncreaseBikeMomentum(playerAvatar);
        PlayerMovement_IncrementStepCounters(playerAvatar);
        PlayerAvatar_SetFlag6(playerAvatar);
    } else if ((collisionResult & 0x10) != 0) {
        if (PlayerAvatar_GetUnk24(playerAvatar) >= 3) {
            movementCmd = PlayerMovement_GetBikeSpeed(playerAvatar, direction);
            PlayerAvatar_SetFlag6(playerAvatar);
            PlayerAvatar_SetFlag0(playerAvatar, TRUE);
        } else {
            movementCmd = sub_0206234C(direction, 0x1c);
            if ((collisionResult & 8) == 0) {
                PlaySE(SEQ_SE_DP_WALL_HIT);
            }
            MapObject_SetNextFacingDirection(mapObject, direction);
            PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
        }
    } else if ((collisionResult & 0x40) != 0) {
        int nextDir = MapObject_GetNextFacingDirection(mapObject);
        movementCmd = sub_0206234C(nextDir, 0);
        MapObject_SetNextFacingDirection(mapObject, nextDir);
        PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
        PlayerAvatar_SetUnk28Unk2C(playerAvatar, -1, -1);
    } else if (collisionResult != 0) {
        movementCmd = sub_0206234C(direction, 0x1c);
        if ((collisionResult & 8) == 0) {
            PlaySE(SEQ_SE_DP_WALL_HIT);
        }
        MapObject_SetNextFacingDirection(mapObject, direction);
        PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    } else {
        movementCmd = PlayerMovement_GetBikeSpeed(playerAvatar, direction);
        PlayerMovement_IncreaseBikeMomentum(playerAvatar);
        PlayerMovement_IncrementStepCounters(playerAvatar);
        PlayerAvatar_SetFlag6(playerAvatar);
    }

    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
}

static void PlayerMovement_HandleBikeTurn(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    u32 movementCmd = sub_0206234C(direction, 0x28);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
    MapObject_SetNextFacingDirection(mapObject, direction);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
}

static void PlayerMovement_HandleBikeCoast(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 inputHeld, u16 inputFlags) {
    u32 movementCmd;

    BOOL shouldContinue = PlayerMovement_DecreaseBikeMomentum(playerAvatar);
    int nextDir = PlayerAvatar_GetNextFacingDirection(playerAvatar);
    int collisionResult = PlayerMovement_CheckCollision(playerAvatar, mapObject, nextDir);

    if ((collisionResult & 4) != 0) {
        movementCmd = sub_0206234C(nextDir, 0x38);
    } else if ((collisionResult & 0x40) != 0) {
        movementCmd = sub_0206234C(nextDir, 0);
        PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    } else if (collisionResult != 0) {
        shouldContinue = FALSE;
        movementCmd = sub_0206234C(nextDir, 0x1c);
        if ((collisionResult & 8) == 0) {
            PlaySE(SEQ_SE_DP_WALL_HIT);
        }
        MapObject_SetNextFacingDirection(mapObject, nextDir);
        PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    } else {
        movementCmd = PlayerMovement_GetBikeSpeed(playerAvatar, nextDir);
    }

    if (shouldContinue == FALSE) {
        PlayerAvatar_SetFlag2(playerAvatar, FALSE);
    }

    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
}

static void PlayerMovement_SetHeldMovement(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, u32 movementCmd) {
    PlayerAvatar_SetUnk8(playerAvatar, movementCmd);
    MapObject_SetHeldMovement(mapObject, movementCmd);
}

int PlayerMovement_CheckCollision(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    int result = 0;
    int flags = PlayerMovement_CheckTerrainCollision(playerAvatar, mapObject, direction);

    if ((flags & 0xa) != 0) {
        result |= 1;
        if (PlayerMovement_CheckSilentCollision(playerAvatar, mapObject, direction) != FALSE) {
            result |= 8;
        }
    }

    if ((flags & 4) != 0) {
        result |= 2;
    }

    if (PlayerMovement_CheckJumpLedge(playerAvatar, mapObject, direction) != FALSE) {
        result |= 4;
    }

    if (PlayerMovement_CheckBlockingTerrain(playerAvatar, mapObject, direction) == TRUE) {
        result |= 0x20;
    }

    if (PlayerMovement_CheckBikeBlockingTerrain(playerAvatar, mapObject, direction) == TRUE) {
        result |= 1;
    }

    return result;
}

static int PlayerMovement_CheckTerrainCollision(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    int xCoord = MapObject_GetXCoord(mapObject);
    int deltaX = GetDeltaXByFacingDirection(direction);
    int yCoord = MapObject_GetYCoord(mapObject);
    int zCoord = MapObject_GetZCoord(mapObject);
    int deltaY = GetDeltaYByFacingDirection(direction);
    VecFx32 currentPos;
    char collisionByte;

    MapObject_CopyPositionVector(mapObject, &currentPos);

    int flags = 0;
    if (sub_02060D94(mapObject, xCoord + deltaX, yCoord, zCoord + deltaY) == TRUE) {
        flags |= 1;
    }

    FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
    if (sub_020549F4(fieldSystem, &currentPos, xCoord + deltaX, zCoord + deltaY, (u32 *)&collisionByte) == TRUE) {
        flags |= 2;
        if (collisionByte != 0) {
            flags |= 8;
        }
    }

    if (sub_02060DEC(mapObject, xCoord + deltaX, zCoord + deltaY, direction) == TRUE) {
        flags |= 2;
    }

    if (sub_02060BFC(mapObject, xCoord + deltaX, yCoord, zCoord + deltaY) == TRUE) {
        flags |= 4;
    }

    return flags;
}

static BOOL PlayerMovement_CheckJumpLedge(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    if (direction != -1) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        int xCoord = MapObject_GetXCoord(mapObject);
        int deltaX = GetDeltaXByFacingDirection(direction);
        int zCoord = MapObject_GetZCoord(mapObject);
        int deltaY = GetDeltaYByFacingDirection(direction);
        u8 tile = GetMetatileBehavior(fieldSystem, xCoord + deltaX, zCoord + deltaY);

        switch (direction) {
        case 0:
            if (sub_0205B7B0(tile) == TRUE) {
                return TRUE;
            }
            break;
        case 1:
            if (sub_0205B7BC(tile) == TRUE) {
                return TRUE;
            }
            break;
        case 2:
            if (sub_0205B7C8(tile) == TRUE) {
                return TRUE;
            }
            break;
        case 3:
            if (sub_0205B7D4(tile) == TRUE) {
                return TRUE;
            }
            break;
        }
    }

    return FALSE;
}

static BOOL PlayerMovement_CheckSilentCollision(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    if (direction != -1) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        int xCoord = MapObject_GetXCoord(mapObject);
        int zCoord = MapObject_GetZCoord(mapObject);
        u8 currentTile = GetMetatileBehavior(fieldSystem, xCoord, zCoord);

        switch (direction) {
        case 0:
            if (sub_0205B730(currentTile) == TRUE) {
                return TRUE;
            }
            // fallthrough
        case 1:
            if (sub_0205B73C(currentTile) == TRUE) {
                return TRUE;
            }
            // fallthrough
        case 2:
            if (sub_0205B724(currentTile) == TRUE) {
                return TRUE;
            }
            // fallthrough
        case 3:
            if (sub_0205B718(currentTile) == TRUE) {
                return TRUE;
            }
            break;
        }

        int deltaX = GetDeltaXByFacingDirection(direction);
        int deltaY = GetDeltaYByFacingDirection(direction);
        u8 nextTile = GetMetatileBehavior(fieldSystem, xCoord + deltaX, zCoord + deltaY);

        if (sub_0205B70C(nextTile) == TRUE) {
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL PlayerMovement_CheckBlockingTerrain(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    if (direction != -1) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        int xCoord = MapObject_GetXCoord(mapObject);
        int deltaX = GetDeltaXByFacingDirection(direction);
        int zCoord = MapObject_GetZCoord(mapObject);
        int deltaY = GetDeltaYByFacingDirection(direction);
        u8 tile = GetMetatileBehavior(fieldSystem, xCoord + deltaX, zCoord + deltaY);

        if (sub_02060E54(mapObject, tile) != FALSE) {
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL PlayerMovement_CheckBikeBlockingTerrain(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    if (direction != -1 && PlayerAvatar_GetState(playerAvatar) == 1) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        int xCoord = MapObject_GetXCoord(mapObject);
        int deltaX = GetDeltaXByFacingDirection(direction);
        int zCoord = MapObject_GetZCoord(mapObject);
        int deltaY = GetDeltaYByFacingDirection(direction);
        u8 tile = GetMetatileBehavior(fieldSystem, xCoord + deltaX, zCoord + deltaY);

        if (sub_02060E54(mapObject, tile) != FALSE) {
            return TRUE;
        }

        if (sub_02060EBC(mapObject, tile) != FALSE) {
            return TRUE;
        }

        if (sub_0205B6F4(tile) != FALSE) {
            return TRUE;
        }

        if (sub_0205B8AC(tile) != FALSE) {
            return TRUE;
        }
    }

    return FALSE;
}

int PlayerMovement_GetInputDirection(PlayerAvatar *playerAvatar, int direction, u16 inputFlags) {
    return PlayerMovement_ResolveDirection(playerAvatar, direction, inputFlags);
}

static int PlayerMovement_GetVerticalInput(u16 inputFlags) {
    if ((inputFlags & 0x20) != 0) {
        return 2;
    }
    if ((inputFlags & 0x10) != 0) {
        return 3;
    }
    return -1;
}

static int PlayerMovement_GetHorizontalInput(u16 inputFlags) {
    if ((inputFlags & 0x40) != 0) {
        return 0;
    }
    if ((inputFlags & 0x80) != 0) {
        return 1;
    }
    return -1;
}

static int PlayerMovement_ResolveDirection(PlayerAvatar *playerAvatar, int direction, u16 inputFlags) {
    int vertDir = PlayerMovement_GetVerticalInput(inputFlags);
    int horizDir = PlayerMovement_GetHorizontalInput(inputFlags);

    if (vertDir == -1) {
        return horizDir;
    }

    if (horizDir == -1) {
        return vertDir;
    }

    int nextDir = PlayerAvatar_GetNextFacingDirection(playerAvatar);
    int lastVertInput = PlayerAvatar_GetUnk28(playerAvatar);
    int lastHorizInput = PlayerAvatar_GetUnk2C(playerAvatar);

    if (nextDir != -1) {
        if (vertDir == lastVertInput && horizDir == lastHorizInput) {
            return nextDir;
        }

        if (horizDir != lastHorizInput) {
            return horizDir;
        }

        return vertDir;
    }

    return horizDir;
}

BOOL PlayerMovement_IsScriptActive(PlayerAvatar *playerAvatar) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == TRUE) {
        return TRUE;
    }

    if (PlayerMovement_IsIdleMovement(MapObject_GetMovementCommand(mapObject)) == TRUE) {
        return TRUE;
    }

    return FALSE;
}

static BOOL PlayerMovement_IsIdleMovement(int movementCmd) {
    switch (movementCmd - 0x1c) {
    case 0:
    case 1:
    case 2:
    case 3:
        return TRUE;
    default:
        return FALSE;
    }
}

static BOOL PlayerMovement_CheckRunButton(PlayerAvatar *playerAvatar, u16 inputFlags) {
    if ((inputFlags & 2) != 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL PlayerMovement_IsRunning(PlayerAvatar *playerAvatar) {
    u32 movementCmd = PlayerAvatar_GetUnk8(playerAvatar);

    switch (movementCmd - 0x58) {
    case 0:
    case 1:
    case 2:
    case 3:
        return TRUE;
    }

    return FALSE;
}

int PlayerMovement_GetMovementResult(PlayerAvatar *playerAvatar, int direction) {
    int result;
    s32 state = PlayerAvatar_GetState(playerAvatar);

    switch (state) {
    case 0:
    case 2:
    case 3:
        result = PlayerMovement_GetWalkOrTurn(playerAvatar, direction);
        result = PlayerMovement_MapWalkResult(result);
        break;
    case 1:
        result = PlayerMovement_GetBikeMoveType(playerAvatar, direction);
        result = PlayerMovement_MapBikeResult(result);
        break;
    default:
        result = 0;
        GF_ASSERT(FALSE);
        break;
    }

    return result;
}

u32 PlayerMovement_CalcMovementCommand(PlayerAvatar *playerAvatar, u16 inputFlags, u16 inputHeld, s32 state, BOOL hasRunningShoes, int collisionResult) {
    int direction = PlayerMovement_ResolveDirection(playerAvatar, inputFlags, inputHeld);
    int result = PlayerMovement_GetMovementResult(playerAvatar, direction);
    PlayerAvatar_SetUnk10(playerAvatar, result);

    if (result == 0) {
        int facingDir = PlayerAvatar_GetFacingDirection(playerAvatar);
        return sub_0206234C(facingDir, 0);
    }

    if (result == 2) {
        return sub_0206234C(direction, 0x28);
    }

    PlayerAvatar_GetMapObject(playerAvatar);
    u32 collRes = (u32)collisionResult;
    int speed = 4;

    if (collRes & 4) {
        speed = 0x38;
    } else if (collRes) {
        speed = 0x1c;
        if (!(collRes & 8)) {
            PlaySE(SEQ_SE_DP_WALL_HIT);
        }
    } else {
        switch (state) {
        case 0:
            speed = 4;
            break;
        case 1:
            speed = 8;
            break;
        case 2:
            speed = 0xc;
            break;
        case 3:
            speed = 0x4c;
            break;
        case 4:
            speed = 0x10;
            break;
        case 5:
            speed = 0x14;
            break;
        default:
            speed = 4;
            break;
        }

        if (hasRunningShoes == TRUE) {
            if (PlayerMovement_CheckRunButton(playerAvatar, inputHeld) == TRUE) {
                speed = 0x58;
            }
        }
    }

    return sub_0206234C(direction, speed);
}

BOOL PlayerMovement_IsScriptInit(PlayerAvatar *playerAvatar) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    return MapObject_AreBitsSetForMovementScriptInit(mapObject);
}

void PlayerMovement_SetMovementCommand(PlayerAvatar *playerAvatar, u32 movementCmd) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    PlayerMovement_SetHeldMovement(playerAvatar, mapObject, movementCmd);
}

int PlayerMovement_GetFacingTileBehavior(PlayerAvatar *playerAvatar, u8 tile) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    return sub_02060FA8(mapObject, tile);
}

static void PlayerMovement_GetCoordsInDirection(PlayerAvatar *playerAvatar, int direction, int *x, int *z) {
    int xCoord = PlayerAvatar_GetXCoord(playerAvatar);
    int deltaX = GetDeltaXByFacingDirection(direction);
    *x = xCoord + deltaX;

    int zCoord = PlayerAvatar_GetZCoord(playerAvatar);
    int deltaY = GetDeltaYByFacingDirection(direction);
    *z = zCoord + deltaY;
}

void PlayerAvatar_GetCoordsInFront(PlayerAvatar *playerAvatar, int *x, int *z) {
    int facingDir = PlayerAvatar_GetFacingDirection(playerAvatar);
    PlayerMovement_GetCoordsInDirection(playerAvatar, facingDir, x, z);
}

static void PlayerMovement_IncrementStepCounters(PlayerAvatar *playerAvatar) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
    GameStats *gameStats = Save_GameStats_Get(fieldSystem->saveData);

    GameStats_Inc(gameStats, 0);

    if (PlayerAvatar_GetState(fieldSystem->playerAvatar) == 1) {
        GameStats_Inc(gameStats, 1);
    }
}

static BOOL PlayerMovement_CheckForcedDismount(PlayerAvatar *playerAvatar, u16 tile, int direction) {
    if (direction != -1) {
        return FALSE;
    }

    if (PlayerAvatar_GetState(playerAvatar) != 1) {
        return FALSE;
    }

    if (PlayerAvatar_CheckBikeStateLocked(playerAvatar) == FALSE) {
        return FALSE;
    }

    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    if (sub_02060EEC(mapObject, tile) == TRUE) {
        return TRUE;
    }

    return FALSE;
}
