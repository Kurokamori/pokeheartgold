#include "unk_0205CB48.h"

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
static BOOL sub_0205CBEC(PlayerAvatar *playerAvatar, int direction);
static void sub_0205CC4C(PlayerAvatar *playerAvatar, int direction, int a2, u16 inputFlags);
static void sub_0205CC74(PlayerAvatar *playerAvatar);
static void sub_0205CC94(PlayerAvatar *playerAvatar);
static void sub_0205CD70(LocalMapObject *mapObject, PlayerAvatar *playerAvatar);
static BOOL sub_0205D004(PlayerAvatar *playerAvatar, int direction);
static int sub_0205D01C(PlayerAvatar *playerAvatar, int direction);
static BOOL sub_0205D07C(PlayerAvatar *playerAvatar, int result, int direction);
static BOOL sub_0205D09C(PlayerAvatar *playerAvatar, int direction);
static BOOL sub_0205D0A8(PlayerAvatar *playerAvatar, int direction);
static BOOL sub_0205D190(PlayerAvatar *playerAvatar, int direction);
static void sub_0205D1FC(PlayerAvatar *playerAvatar);
static int sub_0205D240(PlayerAvatar *playerAvatar, int direction);
static BOOL sub_0205D2A0(PlayerAvatar *playerAvatar, int unk24Change);
static void sub_0205D2D0(PlayerAvatar *playerAvatar, int direction);
static void sub_0205D340(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *a1, int direction, u16 a3, u16 inputFlags);
static void sub_0205D3A8(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *a2, int direction, u16 a4, u16 inputFlags);
static int sub_0205D40C(PlayerAvatar *playerAvatar, int direction);
static int sub_0205D428(int result);
static u32 sub_0205D44C(LocalMapObject *mapObject, u8 tile, u32 speed);
static int sub_0205D450(PlayerAvatar *playerAvatar, int direction);
static void sub_0205D494(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205D4B4(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205D610(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205D640(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *a2, int direction, u16 a4, u16 inputFlags);
static u32 sub_0205D658(PlayerAvatar *playerAvatar, int direction);
static BOOL sub_0205D684(PlayerAvatar *playerAvatar);
static BOOL sub_0205D6B4(PlayerAvatar *playerAvatar);
static void sub_0205D6E8(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *a2, int direction, u16 a4, u16 inputFlags);
static int sub_0205D75C(PlayerAvatar *playerAvatar, int direction);
static int sub_0205D778(int result);
static int sub_0205D7AC(PlayerAvatar *playerAvatar, int direction);
static void sub_0205D818(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205D83C(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205D948(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205D978(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags);
static void sub_0205DA1C(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, u32 movementCmd);
static int sub_0205DAA8(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL sub_0205DB68(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL sub_0205DBF4(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL sub_0205DCA0(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static BOOL sub_0205DCFC(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction);
static int sub_0205DD9C(u16 inputFlags);
static int sub_0205DDB8(u16 inputFlags);
static int sub_0205DDD4(PlayerAvatar *playerAvatar, int direction, u16 inputFlags);
static BOOL sub_0205DE64(int movementCmd);
static BOOL sub_0205DE88(PlayerAvatar *playerAvatar, u16 inputFlags);
static void sub_0205DFFC(PlayerAvatar *playerAvatar, int direction, int *x, int *z);
static void sub_0205E048(PlayerAvatar *playerAvatar);
static BOOL sub_0205E078(PlayerAvatar *playerAvatar, u16 tile, int direction);

// Typedef for callback functions
typedef BOOL (*MetatileCheckFunc)(u8 tile);
typedef BOOL (*MovementCallback)(PlayerAvatar *playerAvatar, int direction);

// Data tables
static const MovementCallback sMovementCallbacks[] = {
    sub_0205D09C,
    sub_0205D0A8,
    sub_0205D190,
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

void PlayerAvatar_MoveControl(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *a1, int a2, u16 a3, u16 a4, int a5) {
    int direction = a2;

    if (direction == -1) {
        direction = sub_0205DDD4(playerAvatar, a3, a4);
    }

    if (sub_0205CBEC(playerAvatar, direction) == FALSE) {
        return;
    }

    sub_0205CC4C(playerAvatar, direction, a3, a4);
    Field_PlayerAvatar_ApplyTransitionFlags(playerAvatar);

    if (sub_0205D004(playerAvatar, direction) == 1) {
        ov01_021F2F24(playerAvatar);
        return;
    }

    if (PlayerAvatar_GetState(playerAvatar) == 0) {
        if (sub_0205D40C(playerAvatar, direction) != 0) {
            ov01_021F2F24(playerAvatar);
        } else if (a5 == 1) {
            ov01_021F2EDC(playerAvatar);
        }
    }

    sub_0205D340(playerAvatar, a1, direction, a3, a4);
    sub_0205CC74(playerAvatar);
    sub_0205CC94(playerAvatar);
}

BOOL sub_0205CBE4(PlayerAvatar *playerAvatar, int direction) {
    return sub_0205CBEC(playerAvatar, direction);
}

static BOOL sub_0205CBEC(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == TRUE) {
        return TRUE;
    }

    if (direction == -1) {
        return FALSE;
    }

    u32 movementCmd = MapObject_GetMovementCommand(mapObject);
    if (sub_0205DE64(movementCmd) == TRUE) {
        int result = sub_0205DA34(playerAvatar, mapObject, direction);
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

static void sub_0205CC4C(PlayerAvatar *playerAvatar, int direction, int a2, u16 inputFlags) {
    int unk28 = sub_0205DD9C(inputFlags);
    int unk2C = sub_0205DDB8(inputFlags);
    PlayerAvatar_SetUnk28Unk2C(playerAvatar, unk28, unk2C);
    PlayerAvatar_ClearFlag6(playerAvatar);
}

static void sub_0205CC74(PlayerAvatar *playerAvatar) {
    if (PlayerAvatar_CheckFlag6(playerAvatar) == TRUE) {
        if (PlayerAvatar_GetUnk10(playerAvatar) == 1) {
            PlayerAvatar_SetFlag1(playerAvatar, FALSE);
        }
    }
}

static void sub_0205CC94(PlayerAvatar *playerAvatar) {
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

    if (sub_0205DE64(movementCmd) == FALSE) {
        if (sub_0205B6F4(tile) == TRUE || sub_0205B6F4(facingTile) == TRUE) {
            PlaySE(SEQ_SE_DP_KUSA);
        }

        if (MetatileBehavior_IsEncounterGrass(tile) == TRUE || MetatileBehavior_IsEncounterGrass(facingTile) == TRUE) {
            PlaySE(SEQ_SE_GS_KUSA2);
        }
    }

    sub_0205CD70(mapObject, playerAvatar);
}

static void sub_0205CD70(LocalMapObject *mapObject, PlayerAvatar *playerAvatar) {
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
            if (sub_0205DE98(playerAvatar) == TRUE) {
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
    u32 unk10 = PlayerAvatar_GetUnk10(playerAvatar);
    u32 unk14 = PlayerAvatar_GetUnk14(playerAvatar);
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    PlayerAvatar_SetUnk14(playerAvatar, 0);

    int result = sub_0205D01C(playerAvatar, -1);
    if (result != 0 && result != 2) {
        PlayerAvatar_SetUnk14(playerAvatar, 2);
        return;
    }

    if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == FALSE) {
        switch (unk10) {
        case 0:
            return;
        case 1:
            if (sub_0205DE64(MapObject_GetMovementCommand(mapObject)) == TRUE) {
                return;
            }
            if (unk14 == 0 || unk14 == 3) {
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

    switch (unk10) {
    case 0:
        return;
    case 1:
        if (unk14 == 0) {
            return;
        }
        if (unk14 == 3) {
            PlayerAvatar_SetUnk14(playerAvatar, 0);
            return;
        }
        PlayerAvatar_SetUnk14(playerAvatar, 3);
        return;
    case 2:
        if (unk14 == 0) {
            return;
        }
        if (unk14 == 3) {
            PlayerAvatar_SetUnk14(playerAvatar, 0);
            return;
        }
        PlayerAvatar_SetUnk14(playerAvatar, 3);
        return;
    }
}

void sub_0205CF44(PlayerAvatar *playerAvatar) {
    PlayerAvatar_SetUnk10(playerAvatar, 0);
    PlayerAvatar_SetUnk14(playerAvatar, 0);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
}

BOOL sub_0205CF60(PlayerAvatar *playerAvatar) {
    u32 unk10 = PlayerAvatar_GetUnk10(playerAvatar);
    u32 unk14 = PlayerAvatar_GetUnk14(playerAvatar);

    if (unk10 == 0) {
        return TRUE;
    }
    if (unk10 == 2) {
        return TRUE;
    }

    if (unk10 == 1) {
        if (unk14 == 0 || unk14 == 3) {
            return TRUE;
        }

        LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
        if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == TRUE) {
            return TRUE;
        }

        if (sub_0205DE64(MapObject_GetMovementCommand(mapObject)) == TRUE) {
            return TRUE;
        }
    }

    return FALSE;
}

void sub_0205CFBC(PlayerAvatar *playerAvatar, int direction) {
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

static BOOL sub_0205D004(PlayerAvatar *playerAvatar, int direction) {
    int result = sub_0205D01C(playerAvatar, direction);
    return sub_0205D07C(playerAvatar, result, direction);
}

static int sub_0205D01C(PlayerAvatar *playerAvatar, int direction) {
    int index = 0;
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    u16 tile = sub_0205F504(mapObject);

    if (sub_0205E078(playerAvatar, tile, direction) == TRUE) {
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

static BOOL sub_0205D07C(PlayerAvatar *playerAvatar, int result, int direction) {
    if (sMovementCallbacks[result](playerAvatar, direction) == TRUE) {
        return TRUE;
    }
    return FALSE;
}

static BOOL sub_0205D09C(PlayerAvatar *playerAvatar, int direction) {
    sub_0205D1FC(playerAvatar);
    return FALSE;
}

static BOOL sub_0205D0A8(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    int nextDir = MapObject_GetNextFacingDirection(mapObject);
    int unk24Change = sub_0205D240(playerAvatar, nextDir);
    int collisionResult = sub_0205DA34(playerAvatar, mapObject, nextDir);

    PlayerAvatar_SetFlag0(playerAvatar, TRUE);

    if (collisionResult != 0) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        PlayerAvatar_SetFlag1(playerAvatar, TRUE);
        PlayerAvatar_SetUnk10(playerAvatar, 0);

        if (sub_0206D494(fieldSystem) == FALSE) {
            sub_0205D1FC(playerAvatar);
            return FALSE;
        }
        return TRUE;
    }

    if (sub_0205D2A0(playerAvatar, unk24Change) == FALSE) {
        sub_0205D1FC(playerAvatar);
        int rvsDir = sub_020611F4(nextDir);

        if (sub_0205DA34(playerAvatar, mapObject, rvsDir) != 0) {
            return FALSE;
        }

        MapObject_SetFlagsBits(mapObject, (MapObjectFlagBits)(6 << 6));
        u32 movementCmd = sub_0206234C(rvsDir, 8);
        sub_0205DA1C(playerAvatar, mapObject, movementCmd);
        PlayerAvatar_SetFlag1(playerAvatar, TRUE);
        PlayerAvatar_SetFlag0(playerAvatar, TRUE);
        PlayerAvatar_SetUnk28Unk2C(playerAvatar, -1, -1);
    } else {
        MapObject_SetFlagsBits(mapObject, (MapObjectFlagBits)(6 << 6));
        sub_0205D2D0(playerAvatar, nextDir);
    }

    PlayerAvatar_SetUnk10(playerAvatar, 1);
    return TRUE;
}

static BOOL sub_0205D190(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    if (sub_0205DA34(playerAvatar, mapObject, 1) == 0) {
        u32 movementCmd = sub_0206234C(1, 0x14);
        sub_0205DA1C(playerAvatar, mapObject, movementCmd);
        PlayerAvatar_SetUnk10(playerAvatar, 1);
        PlayerAvatar_SetUnk24(playerAvatar, 3);
        return TRUE;
    }

    u32 movementCmd = sub_0206234C(1, 1);
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    PlayerAvatar_SetUnk10(playerAvatar, 0);
    PlayerAvatar_SetFlag2(playerAvatar, FALSE);
    return TRUE;
}

static void sub_0205D1FC(PlayerAvatar *playerAvatar) {
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

static int sub_0205D240(PlayerAvatar *playerAvatar, int direction) {
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

static BOOL sub_0205D2A0(PlayerAvatar *playerAvatar, int unk24Change) {
    s32 unk24 = PlayerAvatar_GetUnk24(playerAvatar);

    if (unk24Change == 1) {
        unk24--;
        if (unk24 < 0) {
            return FALSE;
        }
    } else if (unk24Change == 2) {
        unk24++;
        if (unk24 > 3) {
            unk24 = 3;
        }
    }

    PlayerAvatar_SetUnk24(playerAvatar, unk24);
    return TRUE;
}

static void sub_0205D2D0(PlayerAvatar *playerAvatar, int direction) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    int speed = 0x10;

    s32 unk24 = PlayerAvatar_GetUnk24(playerAvatar);
    switch (unk24) {
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
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);

    FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
    if (FollowMon_IsActive(fieldSystem) != FALSE) {
        int prevX = MapObject_GetPreviousXCoord(mapObject);
        int prevZ = MapObject_GetPreviousZCoord(mapObject);
        ov01_02205990(movementCmd, prevX, prevZ, (FollowMon *)((u8 *)fieldSystem + 0xe4));
    }
}

static void sub_0205D340(PlayerAvatar *playerAvatar, FieldSystemUnkSub2C *a1, int direction, u16 a3, u16 inputFlags) {
    s32 state = PlayerAvatar_GetState(playerAvatar);
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    switch (state) {
    case 0:
    case 2:
    case 3:
        sub_0205D3A8(playerAvatar, mapObject, a1, direction, a3, inputFlags);
        return;
    case 1:
        sub_0205D640(playerAvatar, mapObject, a1, direction, a3, inputFlags);
        return;
    default:
        GF_ASSERT(FALSE);
        return;
    }
}

static void sub_0205D3A8(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *a2, int direction, u16 a4, u16 inputFlags) {
    int result = sub_0205D40C(playerAvatar, direction);

    switch (result) {
    case 0:
        sub_0205D494(playerAvatar, mapObject, direction, a4, inputFlags);
        return;
    case 1:
        sub_0205D4B4(playerAvatar, mapObject, direction, a4, inputFlags);
        return;
    case 2:
        sub_0205D610(playerAvatar, mapObject, direction, a4, inputFlags);
        return;
    }
}

static int sub_0205D40C(PlayerAvatar *playerAvatar, int direction) {
    int result = sub_0205D450(playerAvatar, direction);
    int unk10 = sub_0205D428(result);
    PlayerAvatar_SetUnk10(playerAvatar, unk10);
    return result;
}

static int sub_0205D428(int result) {
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

static u32 sub_0205D44C(LocalMapObject *mapObject, u8 tile, u32 speed) {
    return speed;
}

static int sub_0205D450(PlayerAvatar *playerAvatar, int direction) {
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

static void sub_0205D494(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    int facingDir = MapObject_GetFacingDirection(mapObject);
    u32 movementCmd = sub_0206234C(facingDir, 0);
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
}

static void sub_0205D4B4(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    BOOL playedSound = FALSE;
    u32 movementCmd;

    int collisionResult = sub_0205DA34(playerAvatar, mapObject, direction);

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
                if (sub_0205DE88(playerAvatar, inputFlags) == TRUE) {
                    speed = 0x58;
                }
            }

            s32 tile = sub_0205F504(mapObject);
            movementCmd = sub_0205D44C(mapObject, tile, speed);
            sub_0205E048(playerAvatar);
            PlayerAvatar_SetFlag6(playerAvatar);
        }
    } else {
        if (collisionResult == 0 || collisionResult == 0x20) {
            s32 tile = sub_0205F504(mapObject);
            movementCmd = sub_0205D44C(mapObject, tile, 0x10);
            sub_0205E048(playerAvatar);
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
    sub_0205DA1C(playerAvatar, mapObject, finalCmd);

    if (playedSound == FALSE) {
        FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
        int prevX = MapObject_GetPreviousXCoord(mapObject);
        int prevZ = MapObject_GetPreviousZCoord(mapObject);
        ov01_02205990(finalCmd, prevX, prevZ, (FollowMon *)((u8 *)fieldSystem + 0xe4));
    }
}

static void sub_0205D610(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    u32 movementCmd = sub_0206234C(direction, 0x28);
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
    MapObject_SetNextFacingDirection(mapObject, direction);
    PlayerAvatar_ResetUnkC(playerAvatar);
    PlayerAvatar_ToggleUnkC(playerAvatar);
}

static void sub_0205D640(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *a2, int direction, u16 a4, u16 inputFlags) {
    sub_0205D6E8(playerAvatar, mapObject, a2, direction, a4, inputFlags);
}

static u32 sub_0205D658(PlayerAvatar *playerAvatar, int direction) {
    int speed = 0x4c;

    s32 unk24 = PlayerAvatar_GetUnk24(playerAvatar);
    switch (unk24) {
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

static BOOL sub_0205D684(PlayerAvatar *playerAvatar) {
    s32 unk24 = PlayerAvatar_Unk24AddWithCeiling(playerAvatar, 1, 3);

    if (PlayerAvatar_CheckFlag2(playerAvatar) == FALSE) {
        if (unk24 >= 2) {
            PlayerAvatar_SetFlag2(playerAvatar, TRUE);
        }
    }

    if (unk24 == 3) {
        return TRUE;
    }

    return FALSE;
}

static BOOL sub_0205D6B4(PlayerAvatar *playerAvatar) {
    BOOL result = TRUE;
    s32 unk24 = PlayerAvatar_GetUnk24(playerAvatar) - 1;

    if (unk24 < 0) {
        unk24 = 0;
        result = FALSE;
    }

    PlayerAvatar_SetUnk24(playerAvatar, unk24);

    if (PlayerAvatar_CheckFlag2(playerAvatar) == TRUE) {
        if (unk24 == 0) {
            PlayerAvatar_SetFlag2(playerAvatar, FALSE);
        }
    }

    return result;
}

static void sub_0205D6E8(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, FieldSystemUnkSub2C *a2, int direction, u16 a4, u16 inputFlags) {
    int result = sub_0205D75C(playerAvatar, direction);

    switch (result) {
    case 0:
        sub_0205D818(playerAvatar, mapObject, direction, a4, inputFlags);
        break;
    case 1:
        sub_0205D83C(playerAvatar, mapObject, direction, a4, inputFlags);
        break;
    case 2:
        sub_0205D948(playerAvatar, mapObject, direction, a4, inputFlags);
        break;
    case 3:
        sub_0205D978(playerAvatar, mapObject, direction, a4, inputFlags);
        break;
    }
}

static int sub_0205D75C(PlayerAvatar *playerAvatar, int direction) {
    int result = sub_0205D7AC(playerAvatar, direction);
    int unk10 = sub_0205D778(result);
    PlayerAvatar_SetUnk10(playerAvatar, unk10);
    return result;
}

static int sub_0205D778(int result) {
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

static int sub_0205D7AC(PlayerAvatar *playerAvatar, int direction) {
    s32 unk24 = PlayerAvatar_GetUnk24(playerAvatar);

    if (direction == -1) {
        if (unk24 < 2) {
            PlayerAvatar_SetUnk10(playerAvatar, 0);
            return 0;
        }
        PlayerAvatar_SetUnk10(playerAvatar, 1);
        return 3;
    }

    int facingDir = PlayerAvatar_GetFacingDirection(playerAvatar);
    if (facingDir != direction) {
        if (PlayerAvatar_GetUnk10(playerAvatar) != 1) {
            if (unk24 < 2) {
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

static void sub_0205D818(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    int facingDir = MapObject_GetFacingDirection(mapObject);
    u32 movementCmd = sub_0206234C(facingDir, 0);
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
}

static void sub_0205D83C(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    u32 movementCmd;

    int collisionResult = sub_0205DA34(playerAvatar, mapObject, direction);

    if ((collisionResult & 4) != 0) {
        movementCmd = sub_0206234C(direction, 0x38);
        sub_0205D684(playerAvatar);
        sub_0205E048(playerAvatar);
        PlayerAvatar_SetFlag6(playerAvatar);
    } else if ((collisionResult & 0x10) != 0) {
        if (PlayerAvatar_GetUnk24(playerAvatar) >= 3) {
            movementCmd = sub_0205D658(playerAvatar, direction);
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
        movementCmd = sub_0205D658(playerAvatar, direction);
        sub_0205D684(playerAvatar);
        sub_0205E048(playerAvatar);
        PlayerAvatar_SetFlag6(playerAvatar);
    }

    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
}

static void sub_0205D948(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
    u32 movementCmd = sub_0206234C(direction, 0x28);
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
    MapObject_SetNextFacingDirection(mapObject, direction);
    PlayerAvatar_ClearUnk24ClearFlag2(playerAvatar);
}

static void sub_0205D978(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction, u16 a3, u16 inputFlags) {
    u32 movementCmd;

    BOOL shouldContinue = sub_0205D6B4(playerAvatar);
    int nextDir = PlayerAvatar_GetNextFacingDirection(playerAvatar);
    int collisionResult = sub_0205DA34(playerAvatar, mapObject, nextDir);

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
        movementCmd = sub_0205D658(playerAvatar, nextDir);
    }

    if (shouldContinue == FALSE) {
        PlayerAvatar_SetFlag2(playerAvatar, FALSE);
    }

    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
}

static void sub_0205DA1C(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, u32 movementCmd) {
    PlayerAvatar_SetUnk8(playerAvatar, movementCmd);
    MapObject_SetHeldMovement(mapObject, movementCmd);
}

int sub_0205DA34(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
    int result = 0;
    int flags = sub_0205DAA8(playerAvatar, mapObject, direction);

    if ((flags & 0xa) != 0) {
        result |= 1;
        if (sub_0205DBF4(playerAvatar, mapObject, direction) != FALSE) {
            result |= 8;
        }
    }

    if ((flags & 4) != 0) {
        result |= 2;
    }

    if (sub_0205DB68(playerAvatar, mapObject, direction) != FALSE) {
        result |= 4;
    }

    if (sub_0205DCA0(playerAvatar, mapObject, direction) == TRUE) {
        result |= 0x20;
    }

    if (sub_0205DCFC(playerAvatar, mapObject, direction) == TRUE) {
        result |= 1;
    }

    return result;
}

static int sub_0205DAA8(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
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

static BOOL sub_0205DB68(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
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

static BOOL sub_0205DBF4(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
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

static BOOL sub_0205DCA0(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
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

static BOOL sub_0205DCFC(PlayerAvatar *playerAvatar, LocalMapObject *mapObject, int direction) {
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

int sub_0205DD94(PlayerAvatar *playerAvatar, int direction, u16 inputFlags) {
    return sub_0205DDD4(playerAvatar, direction, inputFlags);
}

static int sub_0205DD9C(u16 inputFlags) {
    if ((inputFlags & 0x20) != 0) {
        return 2;
    }
    if ((inputFlags & 0x10) != 0) {
        return 3;
    }
    return -1;
}

static int sub_0205DDB8(u16 inputFlags) {
    if ((inputFlags & 0x40) != 0) {
        return 0;
    }
    if ((inputFlags & 0x80) != 0) {
        return 1;
    }
    return -1;
}

static int sub_0205DDD4(PlayerAvatar *playerAvatar, int direction, u16 inputFlags) {
    int vertDir = sub_0205DD9C(inputFlags);
    int horizDir = sub_0205DDB8(inputFlags);

    if (vertDir == -1) {
        return horizDir;
    }

    if (horizDir == -1) {
        return vertDir;
    }

    int nextDir = PlayerAvatar_GetNextFacingDirection(playerAvatar);
    int unk28 = PlayerAvatar_GetUnk28(playerAvatar);
    int unk2C = PlayerAvatar_GetUnk2C(playerAvatar);

    if (nextDir != -1) {
        if (vertDir == unk28 && horizDir == unk2C) {
            return nextDir;
        }

        if (horizDir != unk2C) {
            return horizDir;
        }

        return vertDir;
    }

    return horizDir;
}

BOOL sub_0205DE38(PlayerAvatar *playerAvatar) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);

    if (MapObject_AreBitsSetForMovementScriptInit(mapObject) == TRUE) {
        return TRUE;
    }

    if (sub_0205DE64(MapObject_GetMovementCommand(mapObject)) == TRUE) {
        return TRUE;
    }

    return FALSE;
}

static BOOL sub_0205DE64(int movementCmd) {
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

static BOOL sub_0205DE88(PlayerAvatar *playerAvatar, u16 inputFlags) {
    if ((inputFlags & 2) != 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL sub_0205DE98(PlayerAvatar *playerAvatar) {
    u32 unk8 = PlayerAvatar_GetUnk8(playerAvatar);

    switch (unk8 - 0x58) {
    case 0:
    case 1:
    case 2:
    case 3:
        return TRUE;
    }

    return FALSE;
}

int sub_0205DEC0(PlayerAvatar *playerAvatar, int direction) {
    int result;
    s32 state = PlayerAvatar_GetState(playerAvatar);

    switch (state) {
    case 0:
    case 2:
    case 3:
        result = sub_0205D450(playerAvatar, direction);
        result = sub_0205D428(result);
        break;
    case 1:
        result = sub_0205D7AC(playerAvatar, direction);
        result = sub_0205D778(result);
        break;
    default:
        result = 0;
        GF_ASSERT(FALSE);
        break;
    }

    return result;
}

u32 sub_0205DF0C(PlayerAvatar *playerAvatar, u16 inputFlags, u16 a2, s32 state, BOOL hasRunningShoes, int collisionResult) {
    int direction = sub_0205DDD4(playerAvatar, inputFlags, a2);
    int result = sub_0205DEC0(playerAvatar, direction);
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
            if (sub_0205DE88(playerAvatar, a2) == TRUE) {
                speed = 0x58;
            }
        }
    }

    return sub_0206234C(direction, speed);
}

BOOL sub_0205DFC8(PlayerAvatar *playerAvatar) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    return MapObject_AreBitsSetForMovementScriptInit(mapObject);
}

void sub_0205DFD4(PlayerAvatar *playerAvatar, u32 movementCmd) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    sub_0205DA1C(playerAvatar, mapObject, movementCmd);
}

int sub_0205DFEC(PlayerAvatar *playerAvatar, u8 tile) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    return sub_02060FA8(mapObject, tile);
}

static void sub_0205DFFC(PlayerAvatar *playerAvatar, int direction, int *x, int *z) {
    int xCoord = PlayerAvatar_GetXCoord(playerAvatar);
    int deltaX = GetDeltaXByFacingDirection(direction);
    *x = xCoord + deltaX;

    int zCoord = PlayerAvatar_GetZCoord(playerAvatar);
    int deltaY = GetDeltaYByFacingDirection(direction);
    *z = zCoord + deltaY;
}

void PlayerAvatar_GetCoordsInFront(PlayerAvatar *playerAvatar, int *x, int *z) {
    int facingDir = PlayerAvatar_GetFacingDirection(playerAvatar);
    sub_0205DFFC(playerAvatar, facingDir, x, z);
}

static void sub_0205E048(PlayerAvatar *playerAvatar) {
    LocalMapObject *mapObject = PlayerAvatar_GetMapObject(playerAvatar);
    FieldSystem *fieldSystem = MapObject_GetFieldSystem(mapObject);
    GameStats *gameStats = Save_GameStats_Get(fieldSystem->saveData);

    GameStats_Inc(gameStats, 0);

    if (PlayerAvatar_GetState(fieldSystem->playerAvatar) == 1) {
        GameStats_Inc(gameStats, 1);
    }
}

static BOOL sub_0205E078(PlayerAvatar *playerAvatar, u16 tile, int direction) {
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
