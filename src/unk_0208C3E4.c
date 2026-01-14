#include "unk_0208C3E4.h"

#include <cw/function_target.h>

#include "global.h"

#include "constants/heap.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/ribbon.h"

#include "msgdata/msg.naix"
#include "msgdata/msg/msg_0300.h"

#include "bg_window.h"
#include "font.h"
#include "heap.h"
#include "message_format.h"
#include "message_printer.h"
#include "move.h"
#include "msgdata.h"
#include "pokedex.h"
#include "pokedex_util.h"
#include "pokemon.h"
#include "render_text.h"
#include "render_window.h"
#include "ribbon.h"
#include "string_util.h"
#include "trainer_memo.h"
#include "unk_0200CE7C.h"
#include "unk_02088288.h"

#define PSS_NUM_WINDOWS 34

struct PokemonSummaryScreenData {
    BgConfig *bgConfig;              // 0x000
    Window windows[PSS_NUM_WINDOWS]; // 0x004 - 0x223
    Window *dynamicWindows;          // 0x224
    u32 numDynamicWindows;           // 0x228
    PokemonSummaryArgs *args;        // 0x22C
    String *unk_230;                 // 0x230 - used in sub_0208CC88
    String *nameString;              // 0x234 - used for display
    String *nameStringCmp;           // 0x238 - used for comparison
    u16 unk_23C;                     // 0x23C
    u16 heldItem;                    // 0x23E
    u8 unk_240;                      // 0x240
    u8 unk_241;                      // 0x241
    u8 unk_242;                      // 0x242
    u8 unk_243;                      // 0x243
    u32 unk_244;                     // 0x244
    u32 unk_248;                     // 0x248
    u32 unk_24C;                     // 0x24C
    u32 unk_250;                     // 0x250
    u16 unk_254;                     // 0x254
    u16 unk_256;                     // 0x256
    u16 unk_258;                     // 0x258
    u16 unk_25A;                     // 0x25A
    u16 unk_25C;                     // 0x25C
    u16 unk_25E;                     // 0x25E
    u16 unk_260;                     // 0x260
    u8 unk_262;                      // 0x262
    u8 unk_263;                      // 0x263 - nature
    u16 moves[4];                    // 0x264 - move IDs
    u8 curPP[4];                     // 0x26C - current PP
    u8 maxPP[4];                     // 0x270 - max PP
    u8 unk_274;                      // 0x274
    u8 unk_275[0x27B - 0x275];
    u8 unk_27B; // 0x27B
    u8 unk_27C[0x280 - 0x27C];
    u32 unk_280; // 0x280
    u8 unk_284[0x294 - 0x284];
    u8 unk_294; // 0x294
    u8 unk_295; // 0x295
    u8 unk_296; // 0x296
    u8 unk_297; // 0x297
    u8 unk_298; // 0x298
    u8 unk_299; // 0x299
    u8 unk_29A[0x2CC - 0x29A];
    void *unk_2CC; // 0x2CC
    u8 unk_2D0[0x79C - 0x2D0];
    void *unk_79C;          // 0x79C
    MsgData *msgData;       // 0x7A0
    MsgData *ribbonMsgData; // 0x7A4
    MessageFormat *msgFmt;  // 0x7A8
    String *tmpString;      // 0x7AC
    String *unk_7B0;        // 0x7B0
    MsgData *moveMsgData;   // 0x7B4
    void *narc;             // 0x7B8
    s8 mode;                // 0x7BC
    u8 unk_7BD;             // 0x7BD
    u8 unk_7BE;             // 0x7BE
    u8 unk_7BF;             // 0x7BF
    u8 unk_7C0[0x7C4 - 0x7C0];
    u8 unk_7C4; // 0x7C4
    u8 unk_7C5; // 0x7C5
    u8 unk_7C6; // 0x7C6
    u8 unk_7C7; // 0x7C7
};

extern const s8 gNatureStatMods[25][5];
extern Pokemon *sub_0208A520(PokemonSummaryScreenData *data);
extern int sub_0208C73C(PokemonSummaryScreenData *data);

#include "filesystem_files_def.h"

static void sub_0208C778(PokemonSummaryScreenData *data, Window *window, u32 color, int align);
static void sub_0208C7F8(PokemonSummaryScreenData *data, int windowIdx, int msgId, int statIdx, int align);
static void sub_0208C850(PokemonSummaryScreenData *data, int windowIdx, int msgId, u32 color, int align);
static void sub_0208C87C(PokemonSummaryScreenData *data, int msgId, u32 value, int digits, u8 printingMode);
static void sub_0208C8C8(PokemonSummaryScreenData *data, int windowIdx, int labelMsgId, int valueMsgId, int valueMsgId2, u16 value1, u16 value2, u8 digits, u32 xPos, u8 yOverride);
static void sub_0208C9A4(PokemonSummaryScreenData *data);
static void sub_0208CC88(PokemonSummaryScreenData *data);
static void sub_0208CFB4(Window *window, Pokemon *mon, int isMine);
static int sub_0208D078(PokemonSummaryScreenData *data);
static void sub_0208D0A4(PokemonSummaryScreenData *data);
static void sub_0208D178(PokemonSummaryScreenData *data);
static void sub_0208D474(PokemonSummaryScreenData *data);
static void sub_0208D520(PokemonSummaryScreenData *data);
static void sub_0208D6B8(PokemonSummaryScreenData *data);
static void sub_0208D728(PokemonSummaryScreenData *data);
static void sub_0208D884(PokemonSummaryScreenData *data, int moveIdx);

// Rodata in order matching assembly
ALIGN(4)
static const WindowTemplate sWindowTemplates_Mode2[8] = {
    { 0x04, 0x0D, 0x11, 0x05, 0x02, 0x0D, 0x021F },
    { 0x04, 0x01, 0x10, 0x15, 0x02, 0x0D, 0x0229 },
    { 0x04, 0x01, 0x12, 0x1E, 0x04, 0x0D, 0x0253 },
    { 0x01, 0x01, 0x03, 0x0A, 0x02, 0x0D, 0x021F },
    { 0x01, 0x01, 0x07, 0x0A, 0x02, 0x0D, 0x0233 },
    { 0x01, 0x01, 0x0B, 0x0A, 0x02, 0x0D, 0x0247 },
    { 0x01, 0x01, 0x0F, 0x0A, 0x02, 0x0D, 0x025B },
    { 0x01, 0x01, 0x13, 0x0A, 0x02, 0x0D, 0x026F },
};

static const WindowTemplate sWindowTemplates_Mode0[8] = {
    { 0x04, 0x0C, 0x01, 0x03, 0x02, 0x0D, 0x021F },
    { 0x04, 0x09, 0x03, 0x09, 0x02, 0x0D, 0x0227 },
    { 0x04, 0x09, 0x07, 0x09, 0x02, 0x0D, 0x0239 },
    { 0x04, 0x0B, 0x09, 0x05, 0x02, 0x0D, 0x024B },
    { 0x04, 0x0A, 0x0D, 0x07, 0x02, 0x0D, 0x0255 },
    { 0x04, 0x0B, 0x11, 0x06, 0x02, 0x0D, 0x0263 },
    { 0x01, 0x00, 0x03, 0x12, 0x12, 0x0D, 0x021F },
    { 0x01, 0x01, 0x16, 0x0B, 0x02, 0x0D, 0x0363 },
};

static const WindowTemplate sWindowTemplates_Mode1[18] = {
    { 0x01, 0x0B, 0x03, 0x07, 0x02, 0x0D, 0x021F },
    { 0x01, 0x0D, 0x06, 0x03, 0x02, 0x0D, 0x022D },
    { 0x01, 0x0D, 0x08, 0x03, 0x02, 0x0D, 0x0233 },
    { 0x01, 0x0D, 0x0A, 0x03, 0x02, 0x0D, 0x0239 },
    { 0x01, 0x0D, 0x0C, 0x03, 0x02, 0x0D, 0x023F },
    { 0x01, 0x0D, 0x0E, 0x03, 0x02, 0x0D, 0x0245 },
    { 0x01, 0x09, 0x11, 0x09, 0x02, 0x0D, 0x024B },
    { 0x01, 0x00, 0x13, 0x13, 0x04, 0x0D, 0x025D },
    { 0x04, 0x05, 0x01, 0x0B, 0x04, 0x0D, 0x021F },
    { 0x04, 0x05, 0x05, 0x0B, 0x04, 0x0D, 0x024B },
    { 0x04, 0x05, 0x09, 0x0B, 0x04, 0x0D, 0x0277 },
    { 0x04, 0x05, 0x0D, 0x0B, 0x04, 0x0D, 0x02A3 },
    { 0x04, 0x05, 0x13, 0x0B, 0x04, 0x0D, 0x02CF },
    { 0x04, 0x1B, 0x06, 0x03, 0x02, 0x0D, 0x02FB },
    { 0x04, 0x1B, 0x08, 0x03, 0x02, 0x0D, 0x0301 },
    { 0x04, 0x11, 0x0A, 0x0F, 0x0A, 0x0D, 0x0307 },
    { 0x04, 0x01, 0x14, 0x0F, 0x02, 0x0D, 0x039D },
    { 0x04, 0x01, 0x11, 0x0A, 0x02, 0x0D, 0x039D },
};

// FIXME : This really needs cleaned up. This cannot possibly be the best way to do this.
static const WindowTemplate sWindowTemplates[PSS_NUM_WINDOWS] = {
    { 0x04, 0x14, 0x01, 0x0B, 0x02, 0x0D, 0x0001 },
    { 0x01, 0x14, 0x01, 0x0B, 0x02, 0x0D, 0x0017 },
    { 0x01, 0x14, 0x01, 0x0B, 0x02, 0x0D, 0x002D },
    { 0x01, 0x01, 0x00, 0x0B, 0x02, 0x0D, 0x0043 },
    { 0x04, 0x14, 0x01, 0x0B, 0x02, 0x0D, 0x0059 },
    { 0x01, 0x14, 0x01, 0x0B, 0x02, 0x0D, 0x006F },
    { 0x01, 0x14, 0x14, 0x06, 0x02, 0x0D, 0x0085 },
    { 0x04, 0x01, 0x01, 0x09, 0x02, 0x0D, 0x0091 },
    { 0x04, 0x01, 0x03, 0x05, 0x02, 0x0D, 0x00A3 },
    { 0x04, 0x01, 0x05, 0x05, 0x02, 0x0D, 0x00AD },
    { 0x04, 0x01, 0x07, 0x05, 0x02, 0x0D, 0x00B7 },
    { 0x04, 0x01, 0x09, 0x05, 0x02, 0x0D, 0x00C1 },
    { 0x04, 0x01, 0x0B, 0x0F, 0x02, 0x0D, 0x00CB },
    { 0x04, 0x01, 0x0F, 0x0C, 0x02, 0x0D, 0x00E9 },
    { 0x04, 0x06, 0x11, 0x03, 0x02, 0x0D, 0x0101 },
    { 0x01, 0x05, 0x03, 0x02, 0x02, 0x0D, 0x0107 },
    { 0x01, 0x03, 0x06, 0x06, 0x02, 0x0D, 0x010B },
    { 0x01, 0x03, 0x08, 0x06, 0x02, 0x0D, 0x0117 },
    { 0x01, 0x03, 0x0A, 0x06, 0x02, 0x0D, 0x0123 },
    { 0x01, 0x03, 0x0C, 0x06, 0x02, 0x0D, 0x012F },
    { 0x01, 0x03, 0x0E, 0x06, 0x02, 0x0D, 0x013B },
    { 0x01, 0x00, 0x11, 0x07, 0x02, 0x0D, 0x0147 },
    { 0x01, 0x01, 0x16, 0x06, 0x02, 0x0D, 0x0147 },
    { 0x04, 0x19, 0x15, 0x05, 0x02, 0x0D, 0x0153 },
    { 0x04, 0x12, 0x04, 0x06, 0x02, 0x0D, 0x015D },
    { 0x04, 0x12, 0x06, 0x06, 0x02, 0x0D, 0x0169 },
    { 0x04, 0x12, 0x08, 0x08, 0x02, 0x0D, 0x0175 },
    { 0x01, 0x12, 0x0B, 0x09, 0x02, 0x0D, 0x0185 },
    { 0x01, 0x02, 0x0D, 0x0C, 0x02, 0x0D, 0x0197 },
    { 0x04, 0x01, 0x11, 0x0C, 0x02, 0x0D, 0x01AF },
    { 0x04, 0x14, 0x01, 0x0B, 0x02, 0x0D, 0x01C7 },
    { 0x01, 0x14, 0x16, 0x0C, 0x02, 0x0D, 0x01DD },
    { 0x01, 0x14, 0x06, 0x06, 0x02, 0x0D, 0x01F5 },
    { 0x01, 0x16, 0x04, 0x09, 0x02, 0x0D, 0x0201 },
};

void sub_0208C3E4(PokemonSummaryScreenData *data) {
    Window *windowArray;
    u16 i;

    i = 0;
    windowArray = &data->windows[0];

    do {
        AddWindow(data->bgConfig, &windowArray[i], &sWindowTemplates[i]);
        FillWindowPixelBuffer(&windowArray[i], 0);
        i++;
    } while (i < PSS_NUM_WINDOWS);

    sub_0208C9A4(data);
    sub_0208C57C(data);
    sub_0208C614(data);
    sub_0208C6B4(data);
}

#ifdef NONMATCHING
// FIXME: Register allocation issue - compiler uses cmp r0, r5 instead of cmp r0, #0
void sub_0208C42C(PokemonSummaryScreenData *data) {
    u32 i;
    u32 winOffset;
    const WindowTemplate *templates;

    switch (data->mode) {
    case 0:
        templates = sWindowTemplates_Mode0;
        data->numDynamicWindows = 8;
        break;
    case 1:
        templates = sWindowTemplates_Mode1;
        data->numDynamicWindows = 18;
        break;
    case 2:
        templates = sWindowTemplates_Mode2;
        data->numDynamicWindows = 8;
        break;
    case 3:
        if (data->args->unk12 != 3 && data->args->unk12 != 4) {
            return;
        }
        data->numDynamicWindows = 3;
        break;
    default:
        return;
    }

    data->dynamicWindows = AllocWindows(HEAP_ID_19, (u8)data->numDynamicWindows);

    i = 0;
    if (data->numDynamicWindows > i) {
        winOffset = i;
        do {
            AddWindow(data->bgConfig, (Window *)((u8 *)data->dynamicWindows + winOffset), templates);
            i++;
            templates++;
            winOffset += sizeof(Window);
        } while (i < data->numDynamicWindows);
    }
}
#else
// clang-format off
asm void sub_0208C42C(PokemonSummaryScreenData *data) {
    push {r3, r4, r5, r6, r7, lr}
    add r4, r0, #0
    ldr r0, =0x000007BC
    ldrsb r0, [r4, r0]
    cmp r0, #3
    bhi _0208C4CE
    add r0, r0, r0
    add r0, pc
    ldrh r0, [r0, #6]
    lsl r0, r0, #0x10
    asr r0, r0, #0x10
    add pc, r0
// _0208C444: jump table (encoded as instructions)
    lsl r6, r0, #0 // .short 0x0006 = _0208C44C - _0208C444 - 2
    lsl r2, r2, #0 // .short 0x0012 = _0208C458 - _0208C444 - 2
    lsl r6, r3, #0 // .short 0x001E = _0208C464 - _0208C444 - 2
    lsl r2, r5, #0 // .short 0x002A = _0208C470 - _0208C444 - 2
_0208C44C:
    mov r0, #0x8a
    mov r1, #8
    lsl r0, r0, #2
    ldr r7, =sWindowTemplates_Mode0
    str r1, [r4, r0]
    b _0208C48C
_0208C458:
    mov r0, #0x8a
    mov r1, #0x12
    lsl r0, r0, #2
    ldr r7, =sWindowTemplates_Mode1
    str r1, [r4, r0]
    b _0208C48C
_0208C464:
    mov r0, #0x8a
    mov r1, #8
    lsl r0, r0, #2
    ldr r7, =sWindowTemplates_Mode2
    str r1, [r4, r0]
    b _0208C48C
_0208C470:
    mov r0, #0x8b
    lsl r0, r0, #2
    ldr r0, [r4, r0]
    ldrb r0, [r0, #0x12]
    cmp r0, #3
    beq _0208C480
    cmp r0, #4
    bne _0208C4CE
_0208C480:
    mov r0, #0x8a
    mov r1, #3
    lsl r0, r0, #2
    str r1, [r4, r0]
    b _0208C48C
// _0208C48A:
    pop {r3, r4, r5, r6, r7, pc}
_0208C48C:
    mov r1, #0x8a
    lsl r1, r1, #2
    ldr r1, [r4, r1]
    mov r0, #0x13
    lsl r1, r1, #0x18
    lsr r1, r1, #0x18
    bl AllocWindows
    mov r1, #0x89
    lsl r1, r1, #2
    str r0, [r4, r1]
    add r0, r1, #4
    ldr r0, [r4, r0]
    mov r5, #0
    cmp r0, #0
    bls _0208C4CE
    add r6, r5, #0
_0208C4AE:
    mov r1, #0x89
    lsl r1, r1, #2
    ldr r1, [r4, r1]
    ldr r0, [r4, #0]
    add r1, r1, r6
    add r2, r7, #0
    bl AddWindow
    mov r0, #0x8a
    lsl r0, r0, #2
    ldr r0, [r4, r0]
    add r5, r5, #1
    add r7, #8
    add r6, #0x10
    cmp r5, r0
    blo _0208C4AE
_0208C4CE:
    pop {r3, r4, r5, r6, r7, pc}
}
// clang-format on
#endif

void sub_0208C4E0(PokemonSummaryScreenData *data) {
    u32 i;
    u32 winOffset;

    switch (data->mode) {
    case 0:
    case 1:
        i = 0;
        if (data->numDynamicWindows > i) {
            winOffset = i;
            do {
                ClearWindowTilemapAndCopyToVram((Window *)((u8 *)data->dynamicWindows + winOffset));
                i++;
                winOffset += sizeof(Window);
            } while (i < data->numDynamicWindows);
        }
        goto cleanup;
    case 2:
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0]);
        goto cleanup;
    case 3:
        if (data->args->unk12 == 3) {
            goto cleanup;
        }
        if (data->args->unk12 == 4) {
            goto cleanup;
        }
        return;
    default:
        return;
    }

cleanup:
    WindowArray_Delete(data->dynamicWindows, (u8)data->numDynamicWindows);
}

void sub_0208C560(PokemonSummaryScreenData *data) {
    u32 winOffset;
    u32 i;

    sub_0208C4E0(data);

    i = 0;
    winOffset = 4; // offset to windows[] array
    do {
        RemoveWindow((Window *)((u8 *)data + winOffset));
        i++;
        winOffset += sizeof(Window);
    } while (i < PSS_NUM_WINDOWS);
}

void sub_0208C57C(PokemonSummaryScreenData *data) {
    Window *window = &data->windows[33];
    u32 tmp;

    FillWindowPixelBuffer(window, 0);
    AddTextPrinterParameterizedWithColor(window, 0, data->nameString, 0, 0, 0xFF, 0x000E0F00, NULL);

    tmp = data->unk_242 << 24;
    if (tmp >> 31) {
        goto end;
    }
    tmp = data->unk_243 << 30;
    tmp >>= 30;
    if (tmp == 0) {
        ReadMsgDataIntoString(data->msgData, 1, data->tmpString);
        sub_0208C778(data, window, 0xC1 << 10, 1);
    } else if (tmp == 1) {
        ReadMsgDataIntoString(data->msgData, 2, data->tmpString);
        sub_0208C778(data, window, 0x00050600, 1);
    }
end:
    ScheduleWindowCopyToVram(window);
}

void sub_0208C614(PokemonSummaryScreenData *data) {
    Window *window = &data->windows[32];
    String *tmpStr;
    u32 tmp;

    FillWindowPixelBuffer(window, 0);

    tmp = data->unk_280 << 3;
    if (tmp >> 31) {
        goto end;
    }

    sub_0200CDAC(data->unk_79C, 1, window, 0, 5);

    tmpStr = NewString_ReadMsgData(data->msgData, 3);

    tmp = data->unk_242 << 25;
    tmp >>= 25;
    BufferIntegerAsString(data->msgFmt, 0, (int)tmp, 3, (PrintingMode)0, 1);
    StringExpandPlaceholders(data->msgFmt, data->tmpString, tmpStr);
    String_Delete(tmpStr);

    AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, 16, 0, 0xFF, 0x204 << 7, NULL);

end:
    ScheduleWindowCopyToVram(window);
}

void sub_0208C6B4(PokemonSummaryScreenData *data) {
    String *tmpStr;

    FillWindowPixelBuffer(&data->windows[31], 0);

    if (data->heldItem != 0) {
        BufferItemName(data->msgFmt, 0, data->heldItem);
        tmpStr = NewString_ReadMsgData(data->msgData, 5);
        StringExpandPlaceholders(data->msgFmt, data->tmpString, tmpStr);
        String_Delete(tmpStr);
    } else {
        ReadMsgDataIntoString(data->msgData, 6, data->tmpString);
    }

    sub_0208C778(data, &data->windows[31], 0x00010200, 0);
    ScheduleWindowCopyToVram(&data->windows[6]);
    ScheduleWindowCopyToVram(&data->windows[31]);
}

int sub_0208C73C(PokemonSummaryScreenData *data) {
    if (data->unk_244 != *(u32 *)((u8 *)data->args + 0xC)) {
        goto fail;
    }
    if (data->unk_274 != *((u8 *)data->args + 0x10)) {
        goto fail;
    }
    if (String_Compare(data->nameStringCmp, data->unk_7B0) != 0) {
        goto fail;
    }
    return 1;
fail:
    return 0;
}

static void sub_0208C778(PokemonSummaryScreenData *data, Window *window, u32 color, int align) {
    u8 x;
    u8 strWidth;
    u32 tmp;

    switch (align) {
    case 0:
        x = 0;
        break;
    case 1:
        strWidth = FontID_String_GetWidth(0, data->tmpString, 0);
        tmp = GetWindowWidth(window) << 27;
        x = (u8)((tmp >> 24) - strWidth);
        break;
    case 2:
        strWidth = FontID_String_GetWidth(0, data->tmpString, 0);
        tmp = GetWindowWidth(window) << 27;
        x = (u8)(((s32)((tmp >> 24) - strWidth)) / 2);
        break;
    default:
        break;
    }

    AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, x, 0, 0xFF, color, NULL);
}

static void sub_0208C7F8(PokemonSummaryScreenData *data, int windowIdx, int msgId, int statIdx, int align) {
    u32 color = 0x000E0F00;
    s8 mod = gNatureStatMods[data->unk_263][statIdx];

    if (mod < 0) {
        color = 0x000E0800;
    } else if (mod > 0) {
        color = 0x000E0700;
    }

    ReadMsgDataIntoString(data->msgData, msgId, data->tmpString);
    sub_0208C778(data, &data->windows[windowIdx], color, align);
}

static void sub_0208C850(PokemonSummaryScreenData *data, int windowIdx, int msgId, u32 color, int align) {
    ReadMsgDataIntoString(data->msgData, msgId, data->tmpString);
    sub_0208C778(data, &data->windows[windowIdx], color, align);
}

static void sub_0208C87C(PokemonSummaryScreenData *data, int msgId, u32 value, int digits, u8 printingMode) {
    String *tmpStr = NewString_ReadMsgData(data->msgData, msgId);
    BufferIntegerAsString(data->msgFmt, 0, value, digits, (PrintingMode)printingMode, 1);
    StringExpandPlaceholders(data->msgFmt, data->tmpString, tmpStr);
    String_Delete(tmpStr);
}

#ifdef NONMATCHING
static void sub_0208C8C8(PokemonSummaryScreenData *data, int windowIdx, int labelMsgId, int valueMsgId, int valueMsgId2, u16 value1, u16 value2, u8 digits, u32 xPos, u8 yOverride) {
    Window *window = &data->dynamicWindows[windowIdx];
    u16 x1, x2;
    u32 width;

    ReadMsgDataIntoString(data->msgData, labelMsgId, data->tmpString);
    width = FontID_String_GetWidth(0, data->tmpString, 0);
    x1 = (u8)xPos - (width / 2);
    x2 = x1 + width;

    AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, x1, yOverride, 0xFF, 0x00010200, NULL);

    sub_0208C87C(data, valueMsgId, value1, digits, 0);
    width = FontID_String_GetWidth(0, data->tmpString, 0);
    AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, x1 - width, yOverride, 0xFF, 0x00010200, NULL);

    sub_0208C87C(data, valueMsgId2, value2, digits, 0);
    AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, x2, yOverride, 0xFF, 0x00010200, NULL);
}
#else
// clang-format off
asm static void sub_0208C8C8(PokemonSummaryScreenData *data, int windowIdx, int labelMsgId, int valueMsgId, int valueMsgId2, u16 value1, u16 value2, u8 digits, u32 xPos, u8 yOverride) {
    push {r3, r4, r5, r6, r7, lr}
    sub sp, #0x20
    add r4, r0, #0
    str r3, [sp, #0x10]
    mov r0, #0x89
    mov r3, #0x7a
    lsl r0, r0, #2
    lsl r7, r1, #4
    lsl r3, r3, #4
    ldr r5, [r4, r0]
    ldr r0, [r4, r3]
    add r3, #0xc
    add r1, r2, #0
    ldr r2, [r4, r3]
    bl ReadMsgDataIntoString
    ldr r1, =0x000007AC
    mov r0, #0
    ldr r1, [r4, r1]
    add r2, r0, #0
    bl FontID_String_GetWidth
    add r1, sp, #0x48
    ldrb r2, [r1]
    lsr r1, r0, #1
    sub r1, r2, r1
    lsl r1, r1, #0x10
    lsr r1, r1, #0x10
    add r0, r1, r0
    str r1, [sp, #0x14]
    lsl r0, r0, #0x10
    lsr r0, r0, #0x10
    str r0, [sp, #0x18]
    add r0, sp, #0x4c
    ldrb r0, [r0]
    mov r1, #0
    ldr r2, =0x000007AC
    str r0, [sp, #0x1c]
    str r0, [sp]
    mov r0, #0xff
    str r0, [sp, #4]
    ldr r0, =0x00010200
    ldr r3, [sp, #0x14]
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    ldr r2, [r4, r2]
    add r0, r5, r7
    bl AddTextPrinterParameterizedWithColor
    mov r0, #0
    str r0, [sp]
    add r3, sp, #0x28
    ldrh r2, [r3, #0x14]
    ldrb r3, [r3, #0x1c]
    ldr r1, [sp, #0x10]
    add r0, r4, #0
    bl sub_0208C87C
    ldr r1, =0x000007AC
    mov r0, #0
    ldr r1, [r4, r1]
    add r2, r0, #0
    bl FontID_String_GetWidth
    mov ip, r0
    ldr r0, [sp, #0x1c]
    mov r1, #0
    str r0, [sp]
    mov r0, #0xff
    str r0, [sp, #4]
    ldr r0, =0x00010200
    ldr r2, =0x000007AC
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    ldr r3, [sp, #0x14]
    mov r6, ip
    ldr r2, [r4, r2]
    add r0, r5, r7
    sub r3, r3, r6
    bl AddTextPrinterParameterizedWithColor
    mov r0, #0
    str r0, [sp]
    add r3, sp, #0x28
    ldrh r2, [r3, #0x18]
    ldrb r3, [r3, #0x1c]
    ldr r1, [sp, #0x38]
    add r0, r4, #0
    bl sub_0208C87C
    ldr r0, [sp, #0x1c]
    mov r1, #0
    str r0, [sp]
    mov r0, #0xff
    str r0, [sp, #4]
    ldr r0, =0x00010200
    ldr r2, =0x000007AC
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    ldr r2, [r4, r2]
    ldr r3, [sp, #0x18]
    add r0, r5, r7
    bl AddTextPrinterParameterizedWithColor
    add sp, #0x20
    pop {r3, r4, r5, r6, r7, pc}
}
// clang-format on
#endif

static void sub_0208C9A4(PokemonSummaryScreenData *data) {
    sub_0208C850(data, 0, 7, 0x000E0F00, 0);
    sub_0208C850(data, 1, 0x17, 0x000E0F00, 0);
    sub_0208C850(data, 2, 0x6D, 0x000E0F00, 0);
    sub_0208C850(data, 3, 0x7E, 0x000E0F00, 0);
    sub_0208C850(data, 4, 0x80, 0x000E0F00, 0);

    if (data->args->isFlag982Set != 0) {
        sub_0208C850(data, 5, 0x9D, 0x000E0F00, 0);
    } else {
        FillWindowPixelBuffer(&data->windows[5], 0);
    }

    sub_0208C850(data, 6, 4, 0x000E0F00, 0);
    sub_0208C850(data, 7, 8, 0x000E0F00, 0);
    sub_0208C850(data, 8, 0xA, 0x000E0F00, 0);
    sub_0208C850(data, 9, 0xC, 0x000E0F00, 0);
    sub_0208C850(data, 0xA, 0xD, 0x000E0F00, 0);
    sub_0208C850(data, 0xB, 0xF, 0x000E0F00, 0);
    sub_0208C850(data, 0xC, 0x11, 0x000E0F00, 0);
    sub_0208C850(data, 0xD, 0x13, 0x000E0F00, 0);
    sub_0208C850(data, 0xF, 0x6E, 0x000E0F00, 0);

    sub_0208C7F8(data, 0x10, 0x6F, 0, 0);
    sub_0208C7F8(data, 0x11, 0x70, 1, 0);
    sub_0208C7F8(data, 0x12, 0x71, 3, 0);
    sub_0208C7F8(data, 0x13, 0x72, 4, 0);
    sub_0208C7F8(data, 0x14, 0x73, 2, 0);

    ReadMsgDataIntoString(data->msgData, 0x74, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->windows[0x15], 0, data->tmpString, 3, 0, 255, 0x000E0F00, NULL);

    ReadMsgDataIntoString(data->msgData, 0x92, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->windows[0x17], 4, data->tmpString, 0, 0, 255, 0x000E0F00, NULL);

    sub_0208C850(data, 0x18, 0x95, 0x000E0F00, 0);
    sub_0208C850(data, 0x19, 0x93, 0x000E0F00, 0);
    sub_0208C850(data, 0x1A, 0x94, 0x000E0F00, 0);
    sub_0208C850(data, 0x1B, 0xA2, 0x000E0F00, 2);
    sub_0208C850(data, 0x1C, 0xA0, 0x00010200, 2);
    sub_0208C850(data, 0x1D, 0xB6, 0x00010200, 0);
    sub_0208C850(data, 0x1E, 0xB3, 0x000E0F00, 0);
}

void sub_0208CBD4(PokemonSummaryScreenData *data) {
    FillBgTilemapRect(data->bgConfig, 1, 0, 0, 0, 32, 2, 16);
    FillBgTilemapRect(data->bgConfig, 1, 0, 0, 2, 18, 22, 16);
    FillBgTilemapRect(data->bgConfig, 4, 0, 0, 1, 18, 23, 16);

    switch (data->mode) {
    case 0:
        sub_0208CC88(data);
        sub_0208D0A4(data);
        break;
    case 1:
        sub_0208D178(data);
        sub_0208D474(data);
        break;
    case 2:
        sub_0208D520(data);
        sub_0208D6B8(data);
        break;
    case 3:
        sub_0208D728(data);
        break;
    }
}

static void sub_0208CC88(PokemonSummaryScreenData *data) {
    int x;
    u32 dexNo;
    u32 level;

    ScheduleWindowCopyToVram(&data->windows[23]);

    if ((data->unk_280 << 3) >> 31) {
        ClearWindowTilemapAndScheduleTransfer(&data->windows[0]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[7]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[8]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[9]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[10]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[11]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[12]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[13]);
        ClearWindowTilemapAndScheduleTransfer(&data->windows[14]);
        ClearWindowTilemapAndScheduleTransfer(&data->dynamicWindows[0]);
        ClearWindowTilemapAndScheduleTransfer(&data->dynamicWindows[1]);
        ClearWindowTilemapAndScheduleTransfer(&data->dynamicWindows[2]);
        ClearWindowTilemapAndScheduleTransfer(&data->dynamicWindows[3]);
        ClearWindowTilemapAndScheduleTransfer(&data->dynamicWindows[4]);
        ClearWindowTilemapAndScheduleTransfer(&data->dynamicWindows[5]);
        return;
    }

    ScheduleWindowCopyToVram(&data->windows[0]);
    ScheduleWindowCopyToVram(&data->windows[7]);
    ScheduleWindowCopyToVram(&data->windows[8]);
    ScheduleWindowCopyToVram(&data->windows[9]);
    ScheduleWindowCopyToVram(&data->windows[10]);
    ScheduleWindowCopyToVram(&data->windows[11]);
    ScheduleWindowCopyToVram(&data->windows[12]);
    ScheduleWindowCopyToVram(&data->windows[13]);
    ScheduleWindowCopyToVram(&data->windows[14]);

    FillWindowPixelBuffer(&data->dynamicWindows[0], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[1], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[2], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[3], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[4], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[5], 0);

    dexNo = Pokedex_ConvertToCurrentDexNo(data->args->natDexEnabled, data->unk_23C);
    if (dexNo != 0) {
        sub_0208C87C(data, 9, dexNo, 3, 2);
    } else {
        ReadMsgDataIntoString(data->msgData, 0x16, data->tmpString);
    }

    if (!((data->unk_280 << 2) >> 31)) {
        sub_0208C778(data, &data->dynamicWindows[0], 0x00010200, 2);
    } else {
        sub_0208C778(data, &data->dynamicWindows[0], 0x00050600, 2);
    }

    x = FontID_String_GetCenterAlignmentX(0, data->unk_230, 0, 0x48);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[1], 0, data->unk_230, x, 0, 0xFF, 0x00010200, NULL);

    if (data->unk_274 == 0) {
        x = FontID_String_GetCenterAlignmentX(0, data->nameStringCmp, 0, 0x48);
        AddTextPrinterParameterizedWithColor(&data->dynamicWindows[2], 0, data->nameStringCmp, x, 0, 0xFF, 0xC1 << 10, NULL);
    } else {
        x = FontID_String_GetCenterAlignmentX(0, data->nameStringCmp, 0, 0x48);
        AddTextPrinterParameterizedWithColor(&data->dynamicWindows[2], 0, data->nameStringCmp, x, 0, 0xFF, 0x00050600, NULL);
    }

    sub_0208C87C(data, 0x10, (u16)data->unk_244, 5, 2);
    sub_0208C778(data, &data->dynamicWindows[3], 0x00010200, 2);

    sub_0208C87C(data, 0x12, data->unk_248, 7, 0);
    sub_0208C778(data, &data->dynamicWindows[4], 0x00010200, 1);

    level = data->unk_242 << 25;
    if (((u32)level >> 25) < 100) {
        sub_0208C87C(data, 0x15, data->unk_250 - data->unk_248, 7, 0);
    } else {
        sub_0208C87C(data, 0x15, 0, 7, 0);
    }
    sub_0208C778(data, &data->dynamicWindows[5], 0x00010200, 1);

    ScheduleWindowCopyToVram(&data->dynamicWindows[0]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[1]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[2]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[3]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[4]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[5]);
}

Unk0208E600 *sub_0208E600();

static void sub_0208CFB4(Window *window, Pokemon *mon, int isMine) {
    Unk0208E600 *memoData = sub_0208E600(mon, isMine, HEAP_ID_19);

    if (memoData->notepad.nature != NULL) {
        AddTextPrinterParameterizedWithColor(window, 0, memoData->notepad.nature, 6, (memoData->notepad.natureLine - 1) * 16, 0xFF, 0x00010200, NULL);
    }
    if (memoData->notepad.dateLocationMet != NULL) {
        AddTextPrinterParameterizedWithColor(window, 0, memoData->notepad.dateLocationMet, 6, (memoData->notepad.dateLocationMetLine - 1) * 16, 0xFF, 0x00010200, NULL);
    }
    if (memoData->notepad.characteristic != NULL) {
        AddTextPrinterParameterizedWithColor(window, 0, memoData->notepad.characteristic, 6, (memoData->notepad.characteristicLine - 1) * 16, 0xFF, 0x00010200, NULL);
    }
    if (memoData->notepad.eggWatch != NULL) {
        AddTextPrinterParameterizedWithColor(window, 0, memoData->notepad.eggWatch, 6, (memoData->notepad.eggWatchLine - 1) * 16, 0xFF, 0x00010200, NULL);
    }

    FillBgTilemapRect(*(BgConfig **)window, 1, 0, 0x10, 5, 2, 0x12, 0);
    sub_0208E994(memoData);
}

static int sub_0208D078(PokemonSummaryScreenData *data) {
    return data->unk_294 + data->unk_295 + data->unk_296 + data->unk_297 + data->unk_298 + data->unk_299;
}

static void sub_0208D0A4(PokemonSummaryScreenData *data) {
    Pokemon *mon;
    Pokemon *tmpMon;
    int same;
    String *tmpStr;

    ScheduleWindowCopyToVram(&data->windows[1]);
    FillWindowPixelBuffer(&data->dynamicWindows[6], 0);

    mon = sub_0208A520(data);
    same = sub_0208C73C(data);

    if (data->args->unk11 == 2) {
        tmpMon = AllocMonZeroed(HEAP_ID_19);
        CopyBoxPokemonToPokemon((BoxPokemon *)mon, tmpMon);
        sub_0208CFB4(&data->dynamicWindows[6], tmpMon, same);
        Heap_Free(tmpMon);
    } else {
        sub_0208CFB4(&data->dynamicWindows[6], mon, same);
    }

    ScheduleWindowCopyToVram(&data->dynamicWindows[6]);

    if (sub_0208D078(data) != 0) {
        FillWindowPixelBuffer(&data->dynamicWindows[7], 0);
        tmpStr = NewString_ReadMsgData(data->msgData, 0xC0);
        AddTextPrinterParameterizedWithColor(&data->dynamicWindows[7], 0, tmpStr, 0, 0, 0, 0x00010200, NULL);
        ScheduleWindowCopyToVram(&data->dynamicWindows[7]);
        String_Delete(tmpStr);
    }
}

static void sub_0208D178(PokemonSummaryScreenData *data) {
    u32 windowWidth;
    String *tmpStr;
    MsgData *natureMsgData;

    sub_0208C7F8(data, 0x10, 0x6F, 0, 0);
    sub_0208C7F8(data, 0x11, 0x70, 1, 0);
    sub_0208C7F8(data, 0x12, 0x71, 3, 0);
    sub_0208C7F8(data, 0x13, 0x72, 4, 0);
    sub_0208C7F8(data, 0x14, 0x73, 2, 0);

    ScheduleWindowCopyToVram(&data->windows[2]);
    ScheduleWindowCopyToVram(&data->windows[15]);
    ScheduleWindowCopyToVram(&data->windows[16]);
    ScheduleWindowCopyToVram(&data->windows[17]);
    ScheduleWindowCopyToVram(&data->windows[18]);
    ScheduleWindowCopyToVram(&data->windows[19]);
    ScheduleWindowCopyToVram(&data->windows[20]);
    ScheduleWindowCopyToVram(&data->windows[21]);

    FillWindowPixelBuffer(&data->dynamicWindows[0], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[1], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[2], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[3], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[4], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[5], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[6], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[7], 0);

    windowWidth = GetWindowWidth(&data->dynamicWindows[0]);
    sub_0208C8C8(data, 0, 0x75, 0x77, 0x76, data->unk_254, data->unk_256, 3, (windowWidth << 26) >> 24, 0);

    sub_0208C87C(data, 0x78, data->unk_258, 3, 0);
    sub_0208C778(data, &data->dynamicWindows[1], 0x00010200, 1);

    sub_0208C87C(data, 0x79, data->unk_25A, 3, 0);
    sub_0208C778(data, &data->dynamicWindows[2], 0x00010200, 1);

    sub_0208C87C(data, 0x7A, data->unk_25C, 3, 0);
    sub_0208C778(data, &data->dynamicWindows[3], 0x00010200, 1);

    sub_0208C87C(data, 0x7B, data->unk_25E, 3, 0);
    sub_0208C778(data, &data->dynamicWindows[4], 0x00010200, 1);

    sub_0208C87C(data, 0x7C, data->unk_260, 3, 0);
    sub_0208C778(data, &data->dynamicWindows[5], 0x00010200, 1);

    BufferAbilityName(data->msgFmt, 0, data->unk_262);
    tmpStr = NewString_ReadMsgData(data->msgData, 0x7D);
    StringExpandPlaceholders(data->msgFmt, data->tmpString, tmpStr);
    String_Delete(tmpStr);
    sub_0208C778(data, &data->dynamicWindows[6], 0x00010200, 0);

    natureMsgData = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, NARC_msgdata_msg, NARC_msg_msg_0722_bin, HEAP_ID_19);
    ReadMsgDataIntoString(natureMsgData, data->unk_262, data->tmpString);
    DestroyMsgData(natureMsgData);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[7], 0, data->tmpString, 3, 0, 0xFF, 0x00010200, NULL);

    ScheduleWindowCopyToVram(&data->dynamicWindows[0]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[1]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[2]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[3]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[4]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[5]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[6]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[7]);
}

static void sub_0208D474(PokemonSummaryScreenData *data) {
    if (data->args->unk12 != 2) {
        ScheduleWindowCopyToVram(&data->windows[4]);
    }

    FillWindowPixelBuffer(&data->dynamicWindows[8], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[9], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[10], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[11], 0);

    sub_0208D884(data, 0);
    sub_0208D884(data, 1);
    sub_0208D884(data, 2);
    sub_0208D884(data, 3);

    ScheduleWindowCopyToVram(&data->dynamicWindows[8]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[9]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[10]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[11]);
    ScheduleWindowCopyToVram(&data->windows[23]);
}

static void sub_0208D520(PokemonSummaryScreenData *data) {
    ScheduleWindowCopyToVram(&data->windows[5]);

    if (data->args->isFlag982Set == 0) {
        return;
    }

    FillWindowPixelBuffer(&data->dynamicWindows[3], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[4], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[5], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[6], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[7], 0);

    ReadMsgDataIntoString(data->msgData, 0xBB, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[3], 0, data->tmpString, 4, 0, 0xFF, 0x00010200, NULL);

    ReadMsgDataIntoString(data->msgData, 0xBC, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[4], 0, data->tmpString, 4, 0, 0xFF, 0x00010200, NULL);

    ReadMsgDataIntoString(data->msgData, 0xBD, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[5], 0, data->tmpString, 4, 0, 0xFF, 0x00010200, NULL);

    ReadMsgDataIntoString(data->msgData, 0xBE, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[6], 0, data->tmpString, 4, 0, 0xFF, 0x00010200, NULL);

    ReadMsgDataIntoString(data->msgData, 0xBF, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[7], 0, data->tmpString, 4, 0, 0xFF, 0x00010200, NULL);

    ScheduleWindowCopyToVram(&data->dynamicWindows[3]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[4]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[5]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[6]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[7]);
}

static void sub_0208D6B8(PokemonSummaryScreenData *data) {
    ScheduleWindowCopyToVram(&data->windows[30]);
    ScheduleWindowCopyToVram(&data->windows[29]);
    ScheduleWindowCopyToVram(&data->windows[23]);

    FillWindowPixelBuffer(&data->dynamicWindows[0], 0);

    sub_0208C87C(data, 0xB7, data->unk_7C6, 3, 0);
    sub_0208C778(data, &data->dynamicWindows[0], 0x00010200, 0);

    ScheduleWindowCopyToVram(&data->dynamicWindows[0]);
    BgCommitTilemapBufferToVram(data->bgConfig, 4);
}

static void sub_0208D728(PokemonSummaryScreenData *data) {
    if (data->args->unk12 != 3 && data->args->unk12 != 4) {
        return;
    }

    FillWindowPixelBuffer(&data->dynamicWindows[1], 0);

    ReadMsgDataIntoString(data->msgData, 0xA5, data->tmpString);
    sub_0208C778(data, &data->dynamicWindows[1], 0x000E0F00, 0);

    ReadMsgDataIntoString(data->msgData, data->unk_27B + 0xA6, data->tmpString);
    AddTextPrinterParameterizedWithColor(&data->dynamicWindows[1], 0, data->tmpString, 0, 16, 0xFF, 0x00010200, NULL);

    ScheduleWindowCopyToVram(&data->dynamicWindows[1]);
}

void sub_0208D7C4(PokemonSummaryScreenData *data) {
    FillWindowPixelBuffer(&data->dynamicWindows[1], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[2], 0);

    if (data->unk_7C4 + data->unk_7C5 * 9 < data->unk_7C6) {
        int nameId = GetRibbonAttr(data->unk_7C7, RIBBONDAT_NAMEGMM);
        ReadMsgDataIntoString(data->ribbonMsgData, nameId, data->tmpString);
        sub_0208C778(data, &data->dynamicWindows[1], 0x000E0F00, 0);

        int descId = GetRibbonDescGmm((const u8 *)data->args->ribbons, data->unk_7C7);
        ReadMsgDataIntoString(data->ribbonMsgData, descId, data->tmpString);
        sub_0208C778(data, &data->dynamicWindows[2], 0x00010200, 0);
    }

    ScheduleWindowCopyToVram(&data->dynamicWindows[1]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[2]);
}

static void sub_0208D884(PokemonSummaryScreenData *data, int moveIdx) {
    Window *window;
    u16 move;
    u8 curPP, maxPP;

    window = &data->dynamicWindows[moveIdx + 8];

    if (moveIdx != 4) {
        move = data->moves[moveIdx];
        curPP = data->curPP[moveIdx];
        maxPP = data->maxPP[moveIdx];
    } else {
        move = data->args->moveToLearn;
        curPP = GetMoveMaxPP(move, 0);
        maxPP = curPP;
    }

    ReadMsgDataIntoString(data->moveMsgData, move, data->tmpString);
    AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, 1, 2, 0xFF, 0x00010200, NULL);

    if (move != 0) {
        ReadMsgDataIntoString(data->msgData, 0x87, data->tmpString);
        AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, 16, 16, 0xFF, 0x00010200, NULL);

        sub_0208C8C8(data, moveIdx + 8, 0x75, moveIdx + 0x88, moveIdx + 0x8D, curPP, maxPP, 2, 0x3C, 16);
    } else {
        u32 width;
        ReadMsgDataIntoString(data->msgData, 0x99, data->tmpString);
        width = FontID_String_GetWidth(0, data->tmpString, 0);
        AddTextPrinterParameterizedWithColor(window, 0, data->tmpString, 0x3C - (width >> 1), 16, 0xFF, 0x00010200, NULL);
    }
}

void sub_0208D9A0(PokemonSummaryScreenData *data, u16 move) {
    u32 power, accuracy;
    MsgData *descMsgData;

    ScheduleWindowCopyToVram(&data->windows[24]);
    ScheduleWindowCopyToVram(&data->windows[25]);
    ScheduleWindowCopyToVram(&data->windows[26]);

    FillWindowPixelBuffer(&data->dynamicWindows[13], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[14], 0);
    FillWindowPixelBuffer(&data->dynamicWindows[15], 0);

    power = GetMoveAttr(move, MOVEATTR_POWER);
    if (power <= 1) {
        ReadMsgDataIntoString(data->msgData, 0x9A, data->tmpString);
    } else {
        sub_0208C87C(data, 0x96, power, 3, 0);
    }
    sub_0208C778(data, &data->dynamicWindows[13], 0x00010200, 1);

    accuracy = GetMoveAttr(move, MOVEATTR_ACCURACY);
    if (accuracy == 0) {
        ReadMsgDataIntoString(data->msgData, 0x9A, data->tmpString);
    } else {
        sub_0208C87C(data, 0x97, accuracy, 3, 0);
    }
    sub_0208C778(data, &data->dynamicWindows[14], 0x00010200, 1);

    descMsgData = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, NARC_msgdata_msg, NARC_msg_msg_0749_bin, HEAP_ID_19);
    ReadMsgDataIntoString(descMsgData, move, data->tmpString);
    sub_0208C778(data, &data->dynamicWindows[15], 0x00010200, 0);
    DestroyMsgData(descMsgData);

    ScheduleWindowCopyToVram(&data->dynamicWindows[13]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[14]);
    ScheduleWindowCopyToVram(&data->dynamicWindows[15]);
}

void sub_0208DAD4(PokemonSummaryScreenData *data) {
    ClearWindowTilemapAndCopyToVram(&data->windows[0x18]);
    ClearWindowTilemapAndCopyToVram(&data->windows[0x19]);
    ClearWindowTilemapAndCopyToVram(&data->windows[0x1A]);
    ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xD]);
    ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xE]);
    ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xF]);
}

void sub_0208DB1C(PokemonSummaryScreenData *data) {
    if (data->args->moveToLearn != 0) {
        FillWindowPixelBuffer(&data->dynamicWindows[0xC], 0);
        sub_0208D884(data, 4);
        ScheduleWindowCopyToVram(&data->dynamicWindows[0xC]);
    } else {
        ScheduleWindowCopyToVram(&data->windows[0x17]);
    }
}

void sub_0208DB58(PokemonSummaryScreenData *data) {
    ScheduleBgTilemapBufferTransfer(data->bgConfig, 1);
}

void sub_0208DB64(PokemonSummaryScreenData *data) {
    Window *wins;
    s32 idx;

    wins = data->dynamicWindows;
    idx = data->unk_7BD << 28;
    FillWindowPixelBuffer(&wins[((u32)idx >> 28) + 8], 0);

    wins = data->dynamicWindows;
    idx = data->unk_7BD << 24;
    FillWindowPixelBuffer(&wins[((u32)idx >> 28) + 8], 0);

    sub_0208D884(data, (u32)(data->unk_7BD << 28) >> 28);
    sub_0208D884(data, (u32)(data->unk_7BD << 24) >> 28);

    wins = data->dynamicWindows;
    idx = data->unk_7BD << 28;
    ScheduleWindowCopyToVram(&wins[((u32)idx >> 28) + 8]);

    wins = data->dynamicWindows;
    idx = data->unk_7BD << 24;
    ScheduleWindowCopyToVram(&wins[((u32)idx >> 28) + 8]);
}

void sub_0208DBF0(PokemonSummaryScreenData *data) {
    Window *window;

    if (data->mode == 1) {
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xD]);
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xE]);
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xF]);
        window = &data->dynamicWindows[0xF];
    } else {
        window = &data->dynamicWindows[0x5];
    }

    FillWindowPixelBuffer(window, 0);
    ReadMsgDataIntoString(data->msgData, 0x9C, data->tmpString);
    sub_0208C778(data, window, 0x00010200, 0);
    ScheduleWindowCopyToVram(window);
}

void sub_0208DC68(PokemonSummaryScreenData *data, int a1) {
    if (a1 == 1) {
        FillWindowPixelBuffer(&data->dynamicWindows[16], 0);
        ReadMsgDataIntoString(data->msgData, 0xC1, data->tmpString);
        AddTextPrinterParameterizedWithColor(&data->dynamicWindows[16], 4, data->tmpString, 0, 0, 0xFF, 0x000E0F00, NULL);
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[0xC]);
        ScheduleWindowCopyToVram(&data->dynamicWindows[16]);
    } else {
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[16]);
        if (data->args->moveToLearn != 0) {
            FillWindowPixelBuffer(&data->dynamicWindows[0xC], 0);
            sub_0208D884(data, 4);
            ScheduleWindowCopyToVram(&data->dynamicWindows[0xC]);
        }
    }
}

void sub_0208DD20(PokemonSummaryScreenData *data, int a1) {
    if (a1 == 1) {
        FillWindowPixelBuffer(&data->dynamicWindows[17], 0);
        ReadMsgDataIntoString(data->msgData, 0xC2, data->tmpString);
        AddTextPrinterParameterizedWithColor(&data->dynamicWindows[17], 4, data->tmpString, 0, 0, 0xFF, 0x000E0F00, NULL);
        ScheduleWindowCopyToVram(&data->dynamicWindows[17]);
    } else {
        ClearWindowTilemapAndCopyToVram(&data->dynamicWindows[17]);
    }
}

void sub_0208DDA0(PokemonSummaryScreenData *data, int a1) {
    int msgId;
    Window *window;

    switch (a1) {
    case 0:
        msgId = 0xAC;
        break;
    case 1:
        msgId = 0xAE;
        break;
    case 2:
        msgId = 0xAF;
        break;
    case 3:
        msgId = 0xB0;
        break;
    case 4:
        msgId = 0xAD;
        break;
    case 0xFE:
        msgId = 0xB1;
        break;
    default:
        msgId = 0xB2;
        break;
    }

    window = &data->dynamicWindows[2];
    DrawFrameAndWindow2(window, 1, 0x03E2, 0x0D);
    FillWindowPixelBuffer(window, 0xF);
    ReadMsgDataIntoString(data->msgData, msgId, data->tmpString);
    AddTextPrinterParameterizedWithColor(window, 1, data->tmpString, 0, 0, 0xFF, 0x0001020F, NULL);
    ScheduleWindowCopyToVram(window);
}
