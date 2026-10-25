# PAL_d.dll Exports Reference

2026-10-23 audit correction: the PE export table has 288 debug entries; IDA's
289-entry listing also includes `DllEntryPoint`. The reference release has 290
exports, while the current `testcase/dll/Pal.dll` has 262 and is a different
binary. See [vm-pal-shader-audit.md](vm-pal-shader-audit.md) and the generated
[PE inventory](evidence/native-interface-inventory.json). The historical table
below describes the debug build only and does not prove ABI/behavior identity.

## Binary Info
- File: `reverse/PAL_d.dll`
- IDB: `reverse/PAL_d.dll.i64`
- Base: 0x10000000
- Total PE exports: 288 (excluding IDA's synthetic DLL entry point)
- Total functions: 13,678 (2,541 named via debug symbols)

## Cross-Binary Relationship

The reference Game.exe imports from PAL.dll (release) at runtime. PAL_d.dll
provides symbols and matching names for those imports, but whole-ABI and behavior
identity must be verified per function. All addresses below are PAL_d.dll addresses.

Game.exe extcall handlers → PAL_d.dll:
- Category 3 (sprite) → `PalSprite*`, `PalAnimation*`
- Category 4 (BGM) → `PalSound*` (BGM channel)
- Category 5 (SE) → `PalSound*` (SE channel)
- Category 7 (wait) → `PalWait`, `PalWaitTime`, `PalWaitTimeEx`
- Category 8 (btn) → `PalButton*`
- Category 11 (movie) → `PalMSprite*`, `PalVideoPlay`
- Category 13 (voice) → `PalSound*` (voice channel)
- Category 20 (random) → `PalRandom`, `PalRandomEx`
- Category 21 (thread) → `PalTask*`

## Full Export List

### Animation System
| Ordinal | Address | Name |
|---------|---------|------|
| 8 | 0x101204A3 | PalAnimation |
| 9 | 0x1011A585 | PalAnimationEx |
| 10 | 0x1011A125 | PalAnimationGetAnimationPoint |
| 11 | 0x1011E310 | PalAnimationGetTime |
| 12 | 0x1011A04E | PalAnimationLoopIs |
| 13 | 0x1012043F | PalAnimationRelease |
| 14 | 0x1011CC0E | PalAnimationReset |
| 15 | 0x101184F6 | PalAnimationResetEx |
| 16 | 0x10120110 | PalAnimationSetTime |
| 17 | 0x1011CC72 | PalAnimationStart |
| 18 | 0x1011EE8C | PalAnimationStartEx |
| 19 | 0x10119BA3 | PalAnimationStop |
| 20 | 0x1011AEEF | PalAnimationStopEx |

### Button/UI System
| Ordinal | Address | Name |
|---------|---------|------|
| 24 | 0x1011B881 | PalButtonCreate |
| 25 | 0x1011AE4F | PalButtonCreateEx |
| 26 | 0x1011AE09 | PalButtonCtrl |
| 27 | 0x1011ACF1 | PalButtonDelete |
| 28 | 0x10120246 | PalButtonEntry |
| 29 | 0x1011C0B0 | PalButtonEntryEx |
| 30 | 0x1011E1D0 | PalButtonGetReaction |
| 31 | 0x1012152E | PalButtonGetReactionEx |
| 32 | 0x101191D5 | PalButtonMode |
| 33 | 0x1011D72B | PalButtonRelease |
| 34 | 0x1011E59F | PalButtonReset |
| 35 | 0x1011CC6D | PalButtonSetPos |
| 36 | 0x101186DB | PalButtonSetReaction |

### Debug / Logging
| Ordinal | Address | Name |
|---------|---------|------|
| 47 | 0x101214F2 | PalDebugPrintf |
| 48 | 0x10120142 | PalDebugSetData |
| 62 | 0x1011AF67 | PalErrorLogPrintf |
| 63 | 0x1011E8E7 | PalErrorLogPrintfBasic |
| 64 | 0x1011A80A | PalErrorLogPuts |

### Effect System
| Ordinal | Address | Name |
|---------|---------|------|
| 55 | 0x1011CA8D | PalEffect |
| 56 | 0x1011C119 | PalEffectEnable |
| 57 | 0x101195A9 | PalEffectEnableIs |
| 58 | 0x1011A1E8 | PalEffectEx |
| 59 | 0x1011E7D9 | PalEffectIs |
| 90 | 0x1011E040 | PalGetFxEffectState |
| 159 | 0x10121560 | PalReleaseFxEffect |
| 171 | 0x1011D98D | PalSetFxEffect |

### File System
| Ordinal | Address | Name |
|---------|---------|------|
| 67 | 0x1011D20D | PalFileCreate |
| 68 | 0x1011945F | PalFileCreateEx |
| 69 | 0x1011E3E2 | PalFileGetFullPath |
| 70 | 0x1011AE36 | PalFileSetFilePointer |

### Font/Text
| Ordinal | Address | Name |
|---------|---------|------|
| 65 | 0x10119D79 | PalExFontLoad |
| 66 | 0x1011E333 | PalExFontUnload |
| 71 | 0x101212C2 | PalFontBegin |
| 72 | 0x1011BCEB | PalFontDrawText |
| 73 | 0x1011ADD7 | PalFontEnd |
| 74 | 0x10119432 | PalFontGetColor |
| 75 | 0x1011909A | PalFontGetEffect |
| 76 | 0x1011C59C | PalFontGetFontSize |
| 77 | 0x10118BCD | PalFontGetSize |
| 78 | 0x1011CA2E | PalFontGetType |
| 79 | 0x10118898 | PalFontLoad |
| 80 | 0x101214F7 | PalFontSetColor |
| 81 | 0x1011A7A1 | PalFontSetEffect |
| 82 | 0x1011E734 | PalFontSetFontSize |
| 83 | 0x1011C4A2 | PalFontSetType |
| 84 | 0x1011C330 | PalFontUnload |

### Input
| Ordinal | Address | Name |
|---------|---------|------|
| 98 | 0x10119473 | PalInputClear |
| 99 | 0x10118195 | PalInputGetJoyOn |
| 100 | 0x1011991E | PalInputGetJoyOnEx |
| 101 | 0x1011BBC9 | PalInputGetJoyPull |
| 102 | 0x1011A0E9 | PalInputGetJoyPush |
| 103 | 0x10118609 | PalInputGetKeyEx |
| 104 | 0x101208FE | PalInputGetKeyOn |
| 105 | 0x1011D54B | PalInputGetKeyOnEx |
| 106 | 0x1011C42A | PalInputGetKeyPull |
| 107 | 0x1011FBD9 | PalInputGetKeyPush |
| 108 | 0x10118CA3 | PalInputGetMouseMoveX |
| 109 | 0x1012124F | PalInputGetMouseMoveY |
| 110 | 0x101180C8 | PalInputGetMouseX |
| 111 | 0x1011FC3D | PalInputGetMouseY |
| 112 | 0x1011912B | PalInputGetWheel |
| 113 | 0x1011F56C | PalInputKeyCancel |
| 114 | 0x1011E19E | PalInputKeyRepeatClear |
| 115 | 0x101200BB | PalInputSetKeyOn |
| 116 | 0x1011BA84 | PalInputSetKeyPull |
| 117 | 0x10120C37 | PalInputSetKeyPush |

### Lifecycle
| Ordinal | Address | Name |
|---------|---------|------|
| 97 | 0x1011CB32 | PalInit |
| 168 | 0x1011FEB3 | PalRun |
| 60 | 0x1011DC58 | PalEnd |
| 275 | 0x1011AABE | PalUninit |

### List/Data Structures
| Ordinal | Address | Name |
|---------|---------|------|
| 120 | 0x1011B80E | PalListCreate |
| 121 | 0x101192CF | PalListDelete |
| 122 | 0x1011ED5B | PalListFind |
| 123 | 0x1011EB3A | PalListFindNext |
| 124 | 0x1011D465 | PalListGetData |
| 125 | 0x1011D49C | PalListGetDataCount |
| 126 | 0x10118DAC | PalListPop |
| 127 | 0x101211B4 | PalListPopFirst |
| 128 | 0x10120D9F | PalListPush |
| 129 | 0x1012097B | PalListPushLast |
| 130 | 0x1011A558 | PalListRelease |

### Memory
| Ordinal | Address | Name |
|---------|---------|------|
| 143 | 0x1011BC73 | PalMemoryAlloc |
| 144 | 0x10120AB6 | PalMemoryFree |

### Movie Sprite (MSprite)
| Ordinal | Address | Name |
|---------|---------|------|
| 131 | 0x1011F9A9 | PalMSpriteCheck |
| 132 | 0x10120BD3 | PalMSpriteGetState |
| 133 | 0x10119045 | PalMSpriteInfoCreate |
| 134 | 0x101205BB | PalMSpriteIsLoop |
| 135 | 0x1011B39A | PalMSpriteLoad |
| 136 | 0x1011A86E | PalMSpriteLock |
| 137 | 0x1011D56E | PalMSpritePause |
| 138 | 0x1011F62F | PalMSpritePlay |
| 139 | 0x1011E923 | PalMSpriteSetLoop |
| 140 | 0x1011B61A | PalMSpriteSetLoopPoint |
| 141 | 0x10119C7F | PalMSpriteStop |
| 142 | 0x10120CD2 | PalMSpriteUnlock |

### Random Numbers
| Ordinal | Address | Name |
|---------|---------|------|
| 153 | 0x1011E1FD | PalRandom |
| 154 | 0x10120B3D | PalRandomEx |
| 155 | 0x101192E8 | PalRandomGetSeed |
| 156 | 0x10118262 | PalRandomReset |
| 157 | 0x1011A058 | PalRandomSetSeed |

### Sound
| Ordinal | Address | Name |
|---------|---------|------|
| 175 | 0x1011C317 | PalSoundCopy |
| 176 | 0x101199F5 | PalSoundGetChannelCount |
| 177 | 0x1011D627 | PalSoundGetNowChannel |
| 178 | 0x1011A157 | PalSoundGetParam |
| 179 | 0x1011E743 | PalSoundGetPrimaryVolume |
| 180 | 0x10118019 | PalSoundGetState |
| 181 | 0x1011A224 | PalSoundGetStatus |
| 182 | 0x101184B0 | PalSoundGetVolume |
| 183 | 0x10120084 | PalSoundGetVolumeChannel |
| 184 | 0x1011F675 | PalSoundLoad |
| 185 | 0x1011C592 | PalSoundLoadVerEpeg |
| 186 | 0x1011CD0D | PalSoundPause |
| 187 | 0x1011BBCE | PalSoundPlay |
| 188 | 0x10118B6D | PalSoundPlayFade |
| 189 | 0x1011B32C | PalSoundPlayIs |
| 190 | 0x1011D1EA | PalSoundRelease |
| 191 | 0x1011BC4B | PalSoundReleaseEx |
| 192 | 0x10120FF2 | PalSoundSetFraquencyChannel |
| 193 | 0x101211E1 | PalSoundSetPanChannel |
| 194 | 0x1011F977 | PalSoundSetPrimaryVolume |
| 195 | 0x1011A5D5 | PalSoundSetStartEnd |
| 196 | 0x10121218 | PalSoundSetVolume |
| 197 | 0x1011FBED | PalSoundSetVolumeChannel |
| 198 | 0x1011D5EB | PalSoundStop |
| 199 | 0x10119EAA | PalSoundStopFade |

### Sprite
| Ordinal | Address | Name |
|---------|---------|------|
| 200 | 0x1011E3D8 | PalSpriteBackBafferCopy |
| 201 | 0x1011B309 | PalSpriteCancelTransition |
| 202 | 0x1011CCB8 | PalSpriteChangePriority |
| 203 | 0x101215A1 | PalSpriteConvertAspectPosition |
| 204 | 0x1011EE50 | PalSpriteCopy |
| 205 | 0x10120327 | PalSpriteCreate |
| 206 | 0x1011A24C | PalSpriteCreateText |
| 207 | 0x1011F265 | PalSpriteCreateTextEx |
| 208 | 0x1011A9D6 | PalSpriteCreateTransitionHandle |
| 209 | 0x101210C4 | PalSpriteGetForeground |
| 210 | 0x10118E2E | PalSpriteGetFxEffectState |
| 211 | 0x1011E46E | PalSpriteGetHeight |
| 212 | 0x10118EDD | PalSpriteGetInfo |
| 213 | 0x1011E36A | PalSpriteGetRect |
| 214 | 0x1011D6B3 | PalSpriteGetTransitionState |
| 215 | 0x1011EB2B | PalSpriteGetWidth |
| 216 | 0x1011DE1F | PalSpriteHit |
| 217 | 0x1011954F | PalSpriteInfoCreate |
| 218 | 0x1011E9B4 | PalSpriteLoad |
| 219 | 0x1011FD5A | PalSpriteLoadMemory |
| 220 | 0x1011ED65 | PalSpriteLoadTextureToFile |
| 221 | 0x1011B192 | PalSpriteLock |
| 222 | 0x1011B084 | PalSpriteMaskAlpha |
| 223 | 0x1011E022 | PalSpriteMoveOffsetPos |
| 224 | 0x1011D8AC | PalSpriteMovePos |
| 225 | 0x1011BAD9 | PalSpritePaint |
| 226 | 0x1011E865 | PalSpriteRectSetPos |
| 227 | 0x10120D45 | PalSpriteRelease |
| 228 | 0x1011F5F3 | PalSpriteReleaseArray |
| 229 | 0x1011C650 | PalSpriteReleaseTransitionHandle |
| 230 | 0x1011920C | PalSpriteSaveTextureToFile |
| 231 | 0x1011C709 | PalSpriteSaveTextureToPngFile |
| 232 | 0x1011E932 | PalSpriteSetBuffer |
| 233 | 0x10119D1F | PalSpriteSetCenterOffset |
| 234 | 0x10119103 | PalSpriteSetColor |
| 235 | 0x1011FF85 | PalSpriteSetFxEffect |
| 236 | 0x101206BA | PalSpriteSetInfo |
| 237 | 0x10118474 | PalSpriteSetLayer |
| 238 | 0x1011C65A | PalSpriteSetOffsetPos |
| 239 | 0x1011FE7C | PalSpriteSetOffsetRect |
| 240 | 0x1011E829 | PalSpriteSetOption |
| 241 | 0x1011EB35 | PalSpriteSetPos |
| 242 | 0x1011D79E | PalSpriteSetRect |
| 243 | 0x1011B4F8 | PalSpriteSetRenderMode |
| 244 | 0x1011DAAF | PalSpriteSetRotate |
| 245 | 0x101208DB | PalSpriteSetRotateEx |
| 246 | 0x1011ADBE | PalSpriteSetRotateExFloat |
| 247 | 0x1011B890 | PalSpriteSetRotateFloat |
| 248 | 0x1011FC5B | PalSpriteSetScale |
| 249 | 0x1011E851 | PalSpriteSetTransition |
| 250 | 0x101201AB | PalSpriteUnlock |
| 251 | 0x1011B859 | PalSpriteViewCtrl |
| 252 | 0x1011BE2B | PalSpriteViewIs |

### Task System
| Ordinal | Address | Name |
|---------|---------|------|
| 257 | 0x1011D5F0 | PalTaskChangeNext |
| 258 | 0x1011BBC4 | PalTaskChangeNextImm |
| 259 | 0x1011BDFE | PalTaskChangeState |
| 260 | 0x1011F45E | PalTaskCreate |
| 261 | 0x1011806E | PalTaskFree |
| 262 | 0x10120D4F | PalTaskGetMessage |
| 263 | 0x1011FE4A | PalTaskGetState |
| 264 | 0x1011D9A1 | PalTaskGetTaskData |
| 265 | 0x10120665 | PalTaskReProcess |
| 266 | 0x1011BE76 | PalTaskReset |
| 267 | 0x1011B22D | PalTaskSetMessage |
| 268 | 0x1011F58A | PalTaskSetTaskData |

### Thumbnail
| Ordinal | Address | Name |
|---------|---------|------|
| 269 | 0x10121281 | PalThumbnailCreate |
| 270 | 0x1011EDAB | PalThumbnailCreateEx |
| 271 | 0x10118582 | PalThumbnailCreateMosaic |
| 272 | 0x1011DF6E | PalThumbnailCreateMosaicEx |

### Timing/Wait
| Ordinal | Address | Name |
|---------|---------|------|
| 276 | 0x1011A120 | PalUpdateTime |
| 278 | 0x1011CF1C | PalWait |
| 279 | 0x1011C1A5 | PalWaitTime |
| 280 | 0x1011C051 | PalWaitTimeEx |
| 287 | 0x1011F5AD | PaltimeGetTime |

### Video
| Ordinal | Address | Name |
|---------|---------|------|
| 277 | 0x1011CB64 | PalVideoPlay |

### Window/Display
| Ordinal | Address | Name |
|---------|---------|------|
| 37 | 0x1011E257 | PalChangeAspectMode |
| 38 | 0x1011D41F | PalChangeWindowSize |
| 39 | 0x1011FCF1 | PalCheckAspectMode |
| 40 | 0x10118F46 | PalCheckWindowMode |
| 85 | 0x10119374 | PalFrameRateSet |
| 96 | 0x10118B4F | PalGetWindowSize |
| 281 | 0x101205AC | PalWindowChangeEnable |
| 282 | 0x10119E69 | PalWindowChangeEnableIs |
| 283 | 0x101194A5 | PalWindowChangeMode |
| 284 | 0x1011F7C4 | PalWindowGetPos |
| 285 | 0x1011D64F | PalWindowGetPosCenter |
| 286 | 0x1011DC6C | PalWindowSetPos |

### Misc
| Ordinal | Address | Name |
|---------|---------|------|
| 1 | 0x10120B1A | ?PalDeleteCursor@@YAXXZ |
| 2 | 0x1011F535 | GetMemoryPool |
| 3 | 0x1011E432 | GetMinWindowSize |
| 4 | 0x1011D5F5 | IsActive |
| 5 | 0x1012040D | IsMovie |
| 6 | 0x1011FF6C | IsSystemEnd |
| 7 | 0x10120011 | Pal3DIs |
| 21 | 0x10120E53 | PalAttachProcess |
| 22 | 0x1011CCA9 | PalAttachWorkProcess |
| 23 | 0x1011B4B7 | PalBeginUpdate |
| 44 | 0x10120F66 | PalCursorSetAutoHideTime |
| 45 | 0x1011B25F | PalCursorSetPos |
| 46 | 0x10120188 | PalDebugIs |
| 54 | 0x10120A70 | PalDefaultCreate |
| 61 | 0x1011E0E5 | PalEndUpdate |
| 86 | 0x1011F369 | PalGestureCheck |
| 87 | 0x10120890 | PalGetCmdLine |
| 88 | 0x1011D550 | PalGetCore |
| 89 | 0x1011BB6A | PalGetFilePath |
| 91 | 0x1011FC60 | PalGetGesture |
| 92 | 0x1011A2DD | PalGetInstanceHandle |
| 93 | 0x10119D06 | PalGetLanguage |
| 94 | 0x1011D2D5 | PalGetSystemParameters |
| 95 | 0x10119EBE | PalGetWindowHandle |
| 118 | 0x10119DF1 | PalIsTouch |
| 119 | 0x1011F2EC | PalIsTouchMode |
| 145 | 0x10120D54 | PalMixSpriteToSprite |
| 146 | 0x1011A16B | PalNumberAlphaType |
| 147 | 0x10118FB9 | PalNumberCreate |
| 148 | 0x1011C8BC | PalNumberDraw |
| 149 | 0x1011ABC0 | PalNumberDrawCtrl |
| 150 | 0x1011B8A4 | PalNumberGetPlace |
| 151 | 0x1011BD72 | PalNumberSet |
| 152 | 0x101210A1 | PalNumberSetEx |
| 158 | 0x1011DE06 | PalReleaseCursor |
| 160 | 0x1012048A | PalRenderGetActive |
| 161 | 0x1011EC57 | PalRenderInit |
| 162 | 0x1011BC50 | PalRenderInsert |
| 163 | 0x10118DD9 | PalRenderLock |
| 164 | 0x1011D866 | PalRenderLockWait |
| 165 | 0x1011810E | PalRenderSetActive |
| 166 | 0x1011C8B7 | PalRenderUninit |
| 167 | 0x1011C10A | PalRenderUnlock |
| 169 | 0x1011FB52 | PalSameNumberDraw |
| 170 | 0x1011B232 | PalSetCursor |
| 172 | 0x1011D43D | PalSetGesture |
| 173 | 0x1011DE5B | PalSetLanguage |
| 174 | 0x10119590 | PalSetPow |
| 253 | 0x1011A1A2 | PalSystemCNPathSet |
| 254 | 0x1011FAB2 | PalSystemPathClear |
| 255 | 0x101196EE | PalSystemPathSet |
| 256 | 0x10120DBD | PalSystemTCPathSet |
| 273 | 0x1011C44D | PalTouchEnable |
| 274 | 0x1011AD9B | PalTouchMode |
| 288 | 0x1011DAE1 | StretchImage |
