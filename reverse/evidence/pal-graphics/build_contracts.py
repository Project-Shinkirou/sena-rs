"""Build the reviewed graphics inventory from explicit contracts and IDA evidence.

The table below is hand-reviewed. IDA prototypes are retained only as evidence;
this script does not promote inferred prototypes to reviewed declarations.
"""

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
OUT = ROOT / "reverse/contracts/graphics.json"
EVIDENCE = ROOT / "reverse/evidence/pal-graphics"

BUILDS = {
    "totsulover": "744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e",
    "reference": "e676972ddaab2d45fc82210bb89143923f10d1053492a89e50571dbcec54840a",
    "koikake": "2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e",
}

# Each entry is: name | machine arguments in stack order | return | behavior.
# Pointers are Win32 addresses, never portable arena keys. f64 occupies 2 words.
TABLE = r"""
PalEffect|effect_id:u32,duration_ms:i32|i32|Calls PalEffectEx(effect_id,duration_ms,0,1).
PalEffectEx|effect_id:u32,duration_ms:i32,flags:u32,wait:u32|i32|Disabled effects and ID zero return 1 immediately. Otherwise rejects an active transition or an out-of-range unsigned ID, captures the preceding image, selects its worker, sets the active flag and cached start clock, and optionally enters PalWait(-1). Native capture may retry GPU locking ten times.
PalEffectEnable|enabled:u32|unresolved|Stores the complete word in the global effect-enable field. Leaves the engine-context pointer in EAX; source-level return type is not established.
PalEffectEnableIs||u32|Returns the complete stored enable word without boolean normalization.
PalEffectIs||u32|Returns the global scene-effect active field.
PalGetRender||ptr32<Render>|Returns the borrowed active render-list address; this can be null.
PalRenderGetActive||ptr32<Render>|Returns the borrowed active render-list address; this can be null.
PalRenderGetState||i32|Returns active render-list +12, or -1 when no list is active.
PalRenderInit||ptr32<Render>|Allocates and zeroes a 48-byte list header and initializes its embedded critical section. Native initialization does not check allocation before initializing the critical section.
PalRenderInsert|render:ptr32<Render>,object:ptr32<Opaque>,callback:ptr32<Callback>,priority:i32|ptr32<RenderNode>|Allocates a 48-byte node and inserts it in descending signed priority order. Equal priorities are placed after existing equals. Object/callback addresses are borrowed. Null allocation returns null; the render pointer is not null-checked.
PalRenderInsertNext|render:ptr32<Render>,object:ptr32<Opaque>,callback:ptr32<Callback>,priority:i32,before_object:ptr32<Opaque>|ptr32<RenderNode>|Allocates a 48-byte node and inserts immediately before the first node whose object equals before_object. Appends if no object matches. Despite the name this is not an insertion after the matched object.
PalRenderMainLock|state:i32|i32|Writes active list +12 with the full supplied word; returns 1, or 0 with error 6 when no active list exists.
PalRenderLock||i32|Sets active list +12 to 1; returns 1, or 0 with error 6 when no active list exists. This alone does not enter a critical section.
PalRenderUnlock||i32|Sets active list +12 to 0; returns 1, or 0 with error 6 when no active list exists.
PalRenderLockWait||i32|Polls active list +12 with Sleep(1) until zero. Returns 1, or 0/error 6 when initially inactive. There is no recovered timeout.
PalRenderLockEnter||i32|Records the active list, enters its critical section at +16, then writes current active list +12 to 1. Returns 0/error 6 when inactive.
PalRenderLockLeave||i32|Clears current active list +12 and leaves that list's critical section when the remembered enter pointer is nonzero. Does not clear the remembered pointer. Returns 0/error 6 when inactive.
PalRenderLockIsBusy||i32|Returns 1 when engine/list is absent; otherwise checks the critical-section recursion count at list +28 for nonzero. It does not query the list +12 state word.
PalRenderSetActive|render:ptr32<Render>|unresolved|Assigns the borrowed list pointer directly without validation or ownership transfer. Leaves engine context in EAX, not the previous active list.
PalRenderUninit|render:ptr32<Render>|void|Unlinks/frees render nodes and the header, deleting its embedded critical section. Does not release the sprite objects stored in the nodes and does not clear the global active-list pointer.
PalTargetRenderListAdd|object:ptr32<Opaque>|unresolved|Copies one complete pointer word to a new std::list node; permits duplicates and null. IDA's char argument is disproved by the helper's dword copy. Returned allocation is incidental rather than an established public handle.
PalTargetRenderListClear||unresolved|Clears the target-object list; object pointers are borrowed and their underlying sprites are not destroyed.
PalTargetRenderListDraw||i32|Clears a temporary GPU render target, then visits target objects in target-list order and searches the active render list for each. Runs matching callbacks even for hidden sprites because the temporary-render flag bypasses visibility. Removes render nodes whose callbacks return zero. Restores the main target; absent active render list returns 0/error 6.
PalSpriteCreate|render:ptr32<Render>,width:i32,height:i32,priority:i32|ptr32<Sprite>|Creates a hidden sprite, white modulation, unit scale, zero rotation/offset, default render mode 1, full source rectangle and a render node. Returns owned sprite or null; release through PalSpriteRelease.
PalSpriteLoad|render:ptr32<Render>,filename:ptr32<const char>,priority:i32|ptr32<Sprite>|Loads through PAL file/resource lookup into engine scratch storage, dispatches PGD differential forms separately, then calls the memory loader. Successful loads also update image-info cache in the newer build. Returns owned sprite or null.
PalSpriteLoadMemory|render:ptr32<Render>,resource_name:ptr32<const char>,bytes:ptr32<const u8>,byte_length:u32,priority:i32|ptr32<Sprite>|Decodes the borrowed encoded bytes synchronously into an owned texture and sprite. GE headers provide position and logical cell size; other formats use decoded dimensions. Inserts the render node on success. The newer implementation ignores resource_name and leaks the sprite allocation on decoder failure.
PalSpriteInfoCreate|filename:ptr32<const char>,fallback_width:i32,fallback_height:i32,priority:i32|ptr32<Sprite>|Creates an owned metadata-only sprite without a texture/render node. Null filename uses supplied dimensions. Otherwise opens the resource and probes GE/PGD, BMP, TGA and, in newer profiles, JPEG/PNG headers. Caches successful newer metadata probes.
PalSpriteInfoCreateFromCache|filename:ptr32<const char>,unused_width:i32,unused_height:i32,priority:i32|ptr32<Sprite>|Returns a metadata-only owned sprite only on an existing cache hit; no fallback file read. Null name or cache miss returns null. Filename is copied to the sprite's bounded name storage.
PalSpriteIsUseSpriteInfoCache||i32|Constant 1 in this export body.
PalSpriteCreateText|render:ptr32<Render>,text:ptr32<const char>,priority:i32,color:u32|ptr32<Sprite>|Allocates an owned hidden sprite using current PAL font state, text measurement, effect-size padding and optional leading bkcolor=/size= tags. Draws with PalFontBegin/DrawText/End and restores the font size on the successful path. Malformed tags are not bounded by the native parser.
PalSpriteCreateTextEx|sprite:ptr32<Sprite>,text:ptr32<const char>,x:i32,y:i32,color:u32|i32|Draws text into an existing sprite and updates its modulation color. Parses leading tags and temporarily changes font size; it does not create a new sprite despite its name. Null sprite/texture returns 0/error 6.
PalSpriteCopy|render:ptr32<Render>,source:ptr32<Sprite>,source_rect:ptr32<const Rect32>,destination_rect:ptr32<const Rect32>,priority:i32|ptr32<Sprite>|Creates a separate owned texture by copying/resizing the source rectangle to destination rectangle size. Inherits source position, scales logical cell dimensions by the rectangle ratios, initializes other transforms, and inserts a new hidden render node. Rectangle pointers are required and unchecked.
PalSpriteRelease|render:ptr32<Render>,sprite:ptr32<Sprite>|i32|Releases attached movie/animation work, unlinks the render node, releases FX and filter backup allocations, releases the texture and frees the sprite. Returns 1; null sprite returns 0/error 6. The caller's pointer is not cleared.
PalSpriteReleaseArray|render:ptr32<Render>,sprites:ptr32<ptr32<Sprite>>,count:i32|i32|Walks count-1 down to zero, releases nonnull entries, and writes zero into every entry. Returns 1 if any release failed and 0 otherwise; zero/negative count returns 0. Array pointer must be valid when count is positive.
PalSpriteDebugList||void|Empty release-build function; no printing or mutation.
PalSpriteGetInfo|sprite:ptr32<Sprite>,out_info:ptr32<SpriteInfo>|i32|Copies the profile-specific public sprite-info fields. Does not return a native object dump. Newer build additionally rejects 0xCCCCCCCC, 0xDDDDDDDD and 0xEEEEEEEE poison pointers. Null input/output returns 0/error 6.
PalSpriteSetInfo|sprite:ptr32<Sprite>,info:ptr32<const SpriteInfo>|i32|Copies profile-specific info fields directly, without recalculating source rectangle, render ordering or texture allocation. Null input/output returns 0/error 6. Koikake additionally updates texture-wrapper dimensions.
PalSpriteGetWidth|sprite:ptr32<const Sprite>|u32|Reads logical cell width at sprite +92. No null guard; this is not necessarily physical texture width.
PalSpriteGetHeight|sprite:ptr32<const Sprite>|u32|Reads logical cell height at sprite +96. No null guard; this is not necessarily physical texture height.
PalSpriteGetPosX|sprite:ptr32<const Sprite>|unresolved_float_st0|Loads sprite +64 float into x87 ST0; null returns floating zero and sets error 6. Float versus double source declaration is not established by this instruction sequence.
PalSpriteGetPosY|sprite:ptr32<const Sprite>|unresolved_float_st0|Loads sprite +68 float into x87 ST0; null returns floating zero and sets error 6. Float versus double source declaration is not established by this instruction sequence.
PalSpriteSetPos|sprite:ptr32<Sprite>,x:f32,y:f32,z:f32|i32|Stores three IEEE-754 floats then reorders the render node using wrapping priority + CVTTSS2SI(z). Returns 1; null returns 0/error 6.
PalSpriteMovePos|sprite:ptr32<Sprite>,dx:f32,dy:f32,dz:f32|i32|Adds float deltas and reorders using priority + truncated resulting z. Returns 1; null returns 0/error 6.
PalSpriteSetOffsetPos|sprite:ptr32<Sprite>,x:i32,y:i32|i32|Stores integer draw offsets at +76/+80. Does not mutate the primary float position or priority. Null returns 0/error 6.
PalSpriteMoveOffsetPos|sprite:ptr32<Sprite>,dx:i32,dy:i32|i32|Adds wrapping 32-bit integer deltas to draw offsets. Null returns 0/error 6.
PalSpriteSetCenterOffset|sprite:ptr32<Sprite>,x:i32,y:i32|unresolved|Stores wrapping negation of x/y in the center offsets. Null performs no write; EAX is not consistently defined, so no success result is established.
PalSpriteChangePriority|sprite:ptr32<Sprite>,priority:i32|i32|Stores base priority then reorders the render node using that priority alone, unlike SetPos/MovePos which add truncated z. Null returns 0/error 6.
PalSpriteViewCtrl|sprite:ptr32<Sprite>,visible:u32|i32|Stores the whole visibility word at +40 without normalization. Returns 1; null returns 0/error 6.
PalSpriteViewIs|sprite:ptr32<const Sprite>|u32|Returns the complete visibility word; null returns 0/error 6.
PalSpriteSetColor|sprite:ptr32<Sprite>,argb:u32|i32|Stores the complete ARGB modulation word at +60. Returns 1; null returns 0/error 6.
PalSpriteSetScale|sprite:ptr32<Sprite>,scale_x:f32,scale_y:f32|i32|Newer profile stores separate x/y scale floats at +112/+116. Older profiles accept one uniform scale and store only +112. No finite-value/clamp check; null returns 0/error 6.
PalSpriteSetRotate|sprite:ptr32<Sprite>,z_degrees:i32|i32|Converts signed 32-bit integer degrees to float and stores z rotation. No angle normalization; null returns 0/error 6.
PalSpriteSetRotateEx|sprite:ptr32<Sprite>,x_degrees:i32,y_degrees:i32,z_degrees:i32|i32|Converts three signed integer degree values to float and stores xyz rotation. Null returns 0/error 6.
PalSpriteSetRotateFloat|sprite:ptr32<Sprite>,z_degrees:f32|i32|Copies a float angle bit pattern without integer conversion or angle normalization. Null returns 0/error 6.
PalSpriteSetRotateExFloat|sprite:ptr32<Sprite>,x_degrees:f32,y_degrees:f32,z_degrees:f32|i32|Copies three float angle bit patterns. The IDA int declarations are not source-ABI evidence. Null returns 0/error 6.
PalSpriteSetRotateResolution|sprite:ptr32<Sprite>,resolution:i32|i32|Converts signed integer resolution to float and stores +156; returns 1, or 0 for null without SetLastError.
PalSpriteSetLayer|sprite:ptr32<Sprite>,layer:u32|i32|Newer profile stores +160 and returns 1, null returns 0/error 6. Reference profile is a constant-success stub with no argument reads; its argument declaration is unresolved there.
PalSpriteSetRenderMode|sprite:ptr32<Sprite>,mode:u32|i32|Stores the full render-mode word without validating known values. Null returns 0/error 6. Drawing later translates it to backend blend operations.
PalSpriteSetAddressUV|sprite:ptr32<Sprite>,mode:u32|unresolved|Writes UV address mode at +140 when sprite is nonnull. Native mode 1 selects wrap; other values select clamp in the GPU draw path. EAX on null is undefined.
PalSpriteSetRect|sprite:ptr32<Sprite>,rect:ptr32<const Rect32>|i32|Copies all four signed coordinates verbatim. Null rect restores [0,0,texture_width,texture_height], not the logical cell. Null sprite returns 0/error 6.
PalSpriteGetRect|sprite:ptr32<const Sprite>,out_rect:ptr32<Rect32>|i32|Copies all four source coordinates; both pointers required. Null returns 0/error 6.
PalSpriteSetOffsetRect|sprite:ptr32<Sprite>,width:u32,height:u32|i32|Sets logical cell width/height only; does not change source rectangle, texture or pixel allocation. Zero and large values are accepted. Null returns 0/error 6.
PalSpriteRectSetPos|sprite:ptr32<Sprite>,column:i32,row:i32|i32|Selects source cell with full-width 32-bit arithmetic. Newer signed quotient comparisons preserve negative indices and special-case a one-pixel remainder. Older profiles compare wrapping products as unsigned. Zero cell dimensions can fault in native code.
PalSpriteHit|sprite:ptr32<const Sprite>,x:i32,y:i32,test_alpha:u32|i32|Requires visibility, subtracts integer draw offset and truncates float position, then tests an unscaled unrotated half-open source-rectangle-sized box. Optional alpha test reads the selected texture texel. Ignores modulation alpha, center offset, scale and rotation. Failed/out-of-range texture reads use opaque -1 in newer/reference helpers.
PalSpriteGetForeground|x:i32,y:i32,test_alpha:u32|ptr32<Sprite>|Visits the active render list and runs PalSpriteHit; selects lowest base priority, replacing on ties. The compared priority is SpriteInfo base priority, not base plus z. Null list/no hit returns null; result is borrowed.
PalSpriteConvertAspectPosition|sprite:ptr32<Sprite>,mode:i32|unresolved|Only acts when aspect correction is enabled and window mode equals 3. Modes 1/2 shift x toward the monitor center, 1/3 shift y upward, and 4 scales centered coordinates using monitor/client ratios. No stable return contract or unconditional sprite null guard.
PalSpriteLock|sprite:ptr32<Sprite>,out_width:ptr32<u32>,out_height:ptr32<u32>|ptr32<u8>|Marks sprite lock flag at +36 before obtaining pixels. Optional outputs receive physical texture width/height, not byte pitch or cell dimensions. Returned BGRA storage is borrowed until unlock; null sprite returns null/error 6. Failed GPU lock leaves the flag set until unlock.
PalSpriteUnlock|sprite:ptr32<Sprite>|unresolved|GPU path invokes texture UnlockRect, then clears sprite +36; software path only clears the flag. Null is ignored. EAX differs by backend and null path, so a boolean success API is not established.
PalSpriteSetBuffer|sprite:ptr32<Sprite>,pixels:ptr32<const u8>,width:i32,height:i32|i32|Copies width*4 bytes per row into the physical texture with texture-width destination stride and tightly packed source stride. No clipping or buffer-length argument. Returns 1; null sprite returns 0/error 6. IDA's extra EDI/ESI args are saved registers, not export parameters.
PalSpritePaint|sprite:ptr32<Sprite>,rect:ptr32<const Rect32>,argb:u32|i32|Null rectangle overwrites the entire texture with the full ARGB word. Nonnull rectangle uses the bounded fill helper. Returns 1 for nonnull sprite even when GPU lock fails; null returns 0/error 6.
PalSpriteBoxFill|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32,argb:u32|unresolved|For positive dimensions, locks pixels, clamps rectangle endpoints to physical dimensions and overwrites full 32-bit pixels. Empty/null cases may return incidental pointer values; successful path inherits Unlock's backend-dependent EAX.
PalSpriteCircleFill|sprite:ptr32<Sprite>,center_x:i32,center_y:i32,radius:i32,argb:u32|unresolved|Uses midpoint-circle iteration and clipped horizontal spans, overwriting full pixels. Radius <=0 does nothing. The span helper dereferences all 32 color bits despite IDA's char prototype. Return inherits incidental values/Unlock.
PalSpriteMaskAlpha|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32,mask:ptr32<const MaskImage32>,mask_x:i32,mask_y:i32|unresolved|Clips both images, then alpha=(destination_alpha*mask_blue)>>8 using native byte 0. Mask is a borrowed {width,height,pixels} record, not a sprite. RGB is unchanged. Return is incidental/backend-dependent.
PalSpriteMaskAlpha2|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32,mask:ptr32<const MaskImage32>,mask_x:i32,mask_y:i32|unresolved|Same mask-image contract as MaskAlpha, but alpha=(destination_alpha*(255-mask_blue))>>8. RGB remains unchanged.
PalSpriteMaskAlphaEx|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32,mask_sprite:ptr32<Sprite>,mask_x:i32,mask_y:i32,flags:u32|unresolved|Locks two sprites and applies mask blue or its inverse when flags bit 0 is set. Uses low-byte bit test; other bits do not alter the kernel. Empty clipped area returns after unlocking only destination, leaving mask locked in this native build.
PalSpriteApplyAlphaMask|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32,mask_sprite:ptr32<Sprite>,mask_x:i32,mask_y:i32,stretch_x:i32,stretch_y:i32|unresolved|Repeats the mask over the destination: mask coordinates are positive-wrapped fmod(mask_origin+pixel/stretch, mask dimensions), truncated for sampling. Multiplies alpha by mask blue and shifts 8. No destination clipping and no zero-stretch check; valid buffers are caller obligations.
PalSpriteDisplacementMap|destination:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32,map:ptr32<Sprite>,map_x:i32,map_y:i32,map_step_x:i32,map_step_y:i32,source:ptr32<Sprite>,source_x:i32,source_y:i32,strength_x:i32,strength_y:i32|unresolved|Requires positive map steps; tiled integer map coordinates use positive modulo. Blue drives x and green drives y with (257*strength*(channel-128))>>16. Copies full source pixel only when source and destination positions are inside their dimensions; skipped pixels stay unchanged.
PalSpriteStretchBlt|destination:ptr32<Sprite>,dst_x:i32,dst_y:i32,dst_width:i32,dst_height:i32,source:ptr32<Sprite>,src_x:i32,src_y:i32,src_width:i32,src_height:i32|unresolved|Checks both sprites and positive rectangles, locks both, applies integer proportional clipping and nearest-neighbor resampling. Coordinates are clamped before reads. Several edge-clipping divisions use already-shrunk dimensions; do not replace with a generic blit and claim equality.
PalSpriteDeleteNoAlpha|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32|i32|Replaces a full pixel with zero only if alpha is zero. Returns 0 for zero dimensions/null sprite and otherwise 1. Native right/bottom overrun adjustment expands width/height rather than safely clipping; restrict comparison tests to in-bounds rectangles.
PalSpriteDeleteNoAlpha255|sprite:ptr32<Sprite>,x:i32,y:i32,width:i32,height:i32|i32|Zero-alpha pixels become zero; every nonzero alpha becomes 255 while RGB stays unchanged. Shares the unsafe overrun adjustment of DeleteNoAlpha.
PalSpriteBackBafferCopy|sprite:ptr32<Sprite>|i32|Copies the backbuffer into the sprite texture using the backend helper. Returns 1 for a nonnull sprite even on GPU lock failure; null returns 0/error 6. Export spelling is intentionally preserved.
PalSpriteRenderCopy|sprite:ptr32<Sprite>|i32|Copies the temporary render target into the sprite texture only in GPU mode. Software mode is a successful no-op. Null returns 0/error 6.
PalSpriteSaveTextureToFile|sprite:ptr32<Sprite>,filename:ptr32<const char>|i32|Exports JPEG through the image encoder on the GPU path and the software save helper otherwise; booleanizes helper byte result ==1. Filename is a real second stack argument omitted by newer IDA pseudocode. Null sprite returns 0/error 6.
PalSpriteSaveTextureToPngFile|sprite:ptr32<Sprite>,filename:ptr32<const char>|i32|Exports image/png via the GPU texture encoder and returns helper byte ==1. Does not select a separate software path. Filename is the second stack word; null sprite returns 0/error 6.
PalSpriteSaveTextureToPng24File|sprite:ptr32<Sprite>,filename:ptr32<const char>|i32|Uses the dedicated 24-bit PNG encoder helper, discarding alpha in the encoded format. Filename is the second stack word; null sprite returns 0/error 6.
PalSpriteLoadTextureToFile|sprite:ptr32<Sprite>,filename:ptr32<const char>|i32|Despite the name, reads an ordinary host file with CreateFileA, builds a memory stream and replaces/updates the texture through the image decoder. Does not use archive lookup. File size is truncated to its low DWORD; returns 0 on open/read/allocation failure.
PalSpriteSetOption|sprite:ptr32<Sprite>,option:u32|i32|Preserves original pixels in a private backup on the first nonzero option. Zero restores and releases the backup; repeated nonzero filters act on current pixels. Options 1/2 use palette transforms, 3/4 use native kernels, and 0x105..0x805 in steps of 0x100 use half-size blur then upscale. Unknown options still create backup and return success.
PalSpriteCreateTransitionHandle||ptr32<Transition>|Allocates and zeroes exactly 32 bytes. Success sets error 0; allocation failure returns null/error 5. Caller owns the handle.
PalSpriteReleaseTransitionHandle|handle:ptr32<Transition>|i32|Cancels when state is nonzero, frees the handle and sets error 0; null returns 0/error 6.
PalSpriteSetTransition|render:ptr32<Render>,first:ptr32<Sprite>,second:ptr32<Sprite>,type:i32,start_time_ms:u32,duration_ms:u32,handle:ptr32<Transition>|i32|Cancels a nonidle prior handle, writes all eight handle fields and marks participating sprites' transition type. Requires at least one sprite. Newer build positions the worker immediately before an involved sprite; old builds use ordinary priority insertion. Both-null failure still leaves handle state=1 and its fields written. Type 1 worker fades first sprite in and second sprite out using (cached_time-start_time)/duration; other types are not handled by this callback.
PalSpriteCancelTransition|handle:ptr32<Transition>|i32|Requires handle and stored render pointer. Clears first sprite transition field, releases the owned second sprite, unlinks worker, zeroes all 32 bytes, and returns 1/error 0. Invalid input returns 0/error 6.
PalSpriteGetTransitionState|handle:ptr32<const Transition>|i32|Reads handle +4; a null handle returns 3, not 0.
PalSpriteSetFxEffect|sprite:ptr32<Sprite>,type:u32,command:u32,duration_ms:u32,source:ptr32<const FxSource>|i32|Obtains/allocates a 28-byte effect owner. Command 0 clears work while retaining owner; 1 refuses active owner and copies the type-specific source; 2 starts stop interpolation using cached PAL time. Other commands return success. Sprite is required and unchecked.
PalSpriteGetFxEffectState|sprite:ptr32<const Sprite>|i32|Directly dereferences sprite->effect->state. Neither sprite nor effect pointer is guarded; calling before an owner exists faults natively.
PalSetFxEffect|target_word:ptr32<u32>,type_and_scope:u32,command:u32,duration_ms:u32,source:ptr32<const FxSource>|i32|Bit 0x10000000 selects the global render-stage owner; target word then supplies render priority. Otherwise target word holds a sprite address. Global start attaches one worker to the active list, rejects existing attachment, and shares the effect-owner command protocol.
PalReleaseFxEffect|target_word:ptr32<u32>,type_and_scope:u32|i32|Global release frees source/work buffers, unlinks worker and clears the global 32-byte owner record. Sprite release frees the owner and clears sprite +28; returns 0 if there was no sprite/owner. Scope selection is the full 32-bit flag.
PalGetFxEffectState|target_word:ptr32<const u32>,type_and_scope:u32|i32|Global scope returns global state directly. Sprite scope dereferences the supplied sprite-address word and returns owner state or zero when owner is absent; pointer-to-word and sprite are otherwise unchecked.
PalCopySpriteToSpriteRGB|destination:ptr32<Sprite>,dst_x:i32,dst_y:i32,source:ptr32<Sprite>,src_x:i32,src_y:i32,width:i32,height:i32|i32|Copies all four bytes per pixel, including alpha despite the RGB name. Negative source origins move destination origin to -source_origin. Zero extents return 0; null sprites return 0/error 6. Native overrun adjustment is not safe clipping and valid in-bounds input is required.
PalCopySpriteToSpriteRGBStr|destination_address:i32,dst_x:i32,dst_y:i32,unused_dst_width:i32,unused_dst_height:i32,source_address:i32,src_x:i32,src_y:i32,width:i32,height:i32|i32|Decorated name establishes ten 32-bit long arguments. The first/sixth are interpreted as sprite addresses. Destination width/height arguments are unused: this export performs the same full-pixel copy, not stretching.
PalCopySpriteToSpriteRectRGB|destination:ptr32<Sprite>,source:ptr32<Sprite>|i32|Calls RGB copy using truncated source position as destination origin, source-rectangle left/top as source origin and logical cell dimensions as extent. Null source returns 0/error 6.
PalCopySpriteToSpriteChannel|destination:ptr32<Sprite>,dst_x:i32,dst_y:i32,dst_channel:i32,source:ptr32<Sprite>,src_x:i32,src_y:i32,width:i32,height:i32,src_channel:i32|i32|Copies one byte per pixel using channel byte offsets supplied without 0..3 validation. Other bytes are unchanged. Coordinates and unsafe overrun adjustment match RGB copy.
PalCopyFileToSpriteRGB|destination:ptr32<Sprite>,dst_x:i32,dst_y:i32,filename:ptr32<const char>,src_x:i32,src_y:i32,width:i32,height:i32|i32|Loads a PAL resource and accepts GE planar data or 24/32-bit BMP/TGA paths. Decodes source pixels and alpha-blends RGB into destination while retaining destination alpha. JPEG/PNG memory decoder support does not imply support in this separate export.
PalMixSpriteToSprite|destination:ptr32<Sprite>,dst_x:i32,dst_y:i32,width:i32,height:i32,source:ptr32<Sprite>,src_x:i32,src_y:i32|i32|Blends RGB with source alpha, preserving destination alpha. Scalar path copies RGB exactly at alpha 255; otherwise (dst*(255-a)+src*a)>>8. MMX path always shifts weighted sums by 8, including alpha 255, producing one-unit darkening. Zero alpha leaves destination untouched.
PalMixSpriteToSpriteEx|destination:ptr32<Sprite>,dst_x:i32,dst_y:i32,width:i32,height:i32,source:ptr32<Sprite>,src_x:i32,src_y:i32|i32|Computes retained_alpha=(255-source_alpha)*destination_alpha/255, output_alpha=source_alpha+retained_alpha, and each output channel=(retained_alpha*dst+source_alpha*src)/output_alpha with integer truncation. Zero output alpha yields a zero pixel.
PalChannelCopyEx|destination:ptr32<Sprite>,destination_rect:ptr32<const Rect32>,destination_channel:u32,source:ptr32<Sprite>,src_x:i32,src_y:i32,source_channel:u32,operation:i32|i32|Clips both images, checks channels <=3, locks a shared source/destination once, then operation 0 copies one channel; 1 saturating-adds; 2 multiplies using (257*(dst*src+128))>>16. Other operation or empty area returns 0. Other channels remain unchanged.
PalGetPixel|sprite:ptr32<Sprite>,x:i32,y:i32|u32|Bounds-checks against logical cell dimensions, locks physical texture and returns raw BGRA DWORD at the unoffset x/y coordinates. Out of bounds returns 0. Sprite is not null-checked.
PalSetPixel|sprite:ptr32<Sprite>,x:i32,y:i32,argb:u32|unresolved|Bounds-checks logical cell dimensions and writes the full raw DWORD; out-of-bounds is a no-op. Sprite is not null-checked; no stable return value.
StretchImage|destination:ptr32<Sprite>,source:ptr32<Sprite>,mode:i32|i32|Uses ratios of locked physical dimensions. Equal ratios 1 copy directly; mode 0 calls 7-bit fixed-point nearest RGBA scaler; mode 1 calls a byte-plane bilinear helper; mode 2 calls the four-channel interpolation helper. Unknown modes skip writing and still return 1.
PalBoxBlurFilter|destination:ptr32<Sprite>,source:ptr32<Sprite>,radius_x:i32,radius_y:i32,passes:i32|i32|Requires equal positive logical dimensions, copies source into temporary storage, then repeats a clamped-edge box kernel horizontally and vertically using a transposed intermediate. Integer channel division truncates. Nonpositive passes copy unchanged. Allocation failure returns 0; render-thread lock is released around CPU work.
PalGaussianBlurFilter|destination:ptr32<Sprite>,source:ptr32<Sprite>,level:i32|i32|Requires equal positive logical dimensions, copies source, runs the native fixed-kernel blur helper, copies result to destination and frees temporaries. Releases/reenters render lock around CPU work when called on the render thread.
PalAlphaCopyFilter|destination:ptr32<Sprite>,source:ptr32<Sprite>,strength:i32|i32|Requires equal positive logical dimensions. Scales source modulation channels by strength/255, then writes RGB multiplied by source alpha using (x*y+255)>>8; output alpha stays source alpha. A zero scaled modulation alpha or zero strength clears the whole destination. It does not composite over destination.
PalMotionBlurFilter|destination:ptr32<Sprite>,source:ptr32<Sprite>,radius:i32,angle:f64,samples_per_unit:i32|i32|Requires equal positive logical dimensions, takes an angle as eight-byte IEEE-754 double, and samples 2*radius*samples_per_unit points along a line with 16.16 fixed-point steps. Out-of-bounds samples are transparent; RGB is alpha-weighted and alpha is averaged. Temp buffers protect source/destination aliasing.
PalSpriteSwirlBlur|destination:ptr32<Sprite>,source:ptr32<Sprite>,rect:ptr32<const Rect32>,swirl:f32,radial:f32|unresolved|Copies the source, downsamples the specified rectangle by nearest 2x, performs center-weighted swirl and four-sample radial blur, sets blurred alpha to 255, expands nearest 2x into the saved full image and copies it to destination. Rectangle/size preconditions are not validated natively.
PalSpriteWaveformWarp|destination:ptr32<Sprite>,source:ptr32<Sprite>,time:f32,amplitude:f32,wavelength:f32,speed:f32,angle_degrees:f32,waveform:i32|unresolved|Requires equal positive physical dimensions. Zero wavelength becomes 1. Rotated coordinates feed sine (0), triangular wave (1), or deterministic integer noise (2); unknown waveform yields zero displacement. Sampling uses truncation and edge clamping. Angle is converted from degrees; source/destination aliasing is not buffered.
PalSpriteWallSpringEffect|destination_address:i32,source_address:i32,center_x:i32,center_y:i32,power:f32,unused:f32|i32|Decorated name proves six stack arguments, including an unused last float omitted by IDA. Copies source first, requires equal logical dimensions, applies a 1024-entry radial power table within radius width/2, then nearest-samples with +0.5 rounding and edge clamp. Returns 1 on matching sizes, else 0.
"""


def canonical(name):
    return name[1:].split("@@", 1)[0] if name.startswith("?") else name


specs = {}
for line in TABLE.strip().splitlines():
    name, args, result, behavior = line.split("|", 3)
    specs[name] = (args, result, behavior)


def arguments(spec):
    result = []
    offset = 4
    for item in spec.split(",") if spec else []:
        name, value_type = item.split(":", 1)
        size = 8 if value_type == "f64" else 4
        arg = {"name": name, "type": value_type, "stack_offset_from_entry_esp": offset,
               "width_bytes": size}
        if value_type.startswith("ptr32") or name.endswith("_address"):
            arg["ownership"] = "borrowed unless the behavior explicitly transfers or releases ownership"
            arg["nullability"] = "see the export-specific behavior; no implicit null acceptance"
        result.append(arg)
        offset += size
    return result


MAPPINGS = {
    "PalEffect": "crates/pal-vm/src/effect.rs; effect/scene_transition.rs",
    "PalEffectEx": "crates/pal-vm/src/effect.rs; effect/scene_transition.rs",
    "PalSpriteLoad": "crates/pal-vm/src/runtime.rs; runtime/registered_images.rs; image/",
    "PalSpriteCreateText": "crates/pal-vm/src/font.rs; runtime.rs text sprite construction",
    "PalSpriteCreateTextEx": "crates/pal-vm/src/font.rs; runtime.rs text sprite construction",
    "PalSpriteMaskAlpha": "crates/pal-vm/src/sprite.rs::mask_alpha_surface",
    "PalSpriteSetRenderMode": "crates/pal-vm/src/scene/blend.rs; engine.rs::compose_scene; renderer/wgpu_backend.rs; platform/{switch,vita}/source/scene_gpu.c",
    "PalSpriteHit": "crates/pal-vm/src/sprite.rs::SpriteSystem::hit",
    "PalChannelCopyEx": "crates/pal-vm/src/sprite/channel_copy.rs::SpriteSystem::channel_copy_ex",
    "PalTargetRenderListAdd": "crates/pal-vm/src/sprite/target_render.rs; runtime/children.rs registered 3:83",
    "PalTargetRenderListClear": "crates/pal-vm/src/sprite/target_render.rs; runtime/children.rs registered 3:82",
    "PalTargetRenderListDraw": "crates/pal-vm/src/sprite/target_render.rs; runtime/children.rs registered 3:84",
    "PalSpriteRenderCopy": "crates/pal-vm/src/sprite/target_render.rs::SpriteSystem::render_copy",
    "PalSpriteSetScale": "crates/pal-vm/src/sprite.rs::SpriteSystem::set_scale (legacy) / set_scale_xy (totsulover); snapshot v12",
    "PalSpriteSetInfo": "crates/pal-vm/src/sprite.rs::PalSprite::set_info (portable structure)",
    "PalSpriteGetInfo": "crates/pal-vm/src/sprite.rs::PalSprite::info (portable structure)",
    "PalMixSpriteToSprite": "crates/pal-vm/src/sprite.rs::blend_pixel / BlendMode::AlphaRgb",
    "PalMixSpriteToSpriteEx": "crates/pal-vm/src/sprite.rs::SpriteSystem::mix_sprite_to_sprite_ex / BlendMode::SourceOver",
    "PalSpriteSetOption": "crates/pal-vm/src/sprite.rs::set_option; sprite/filters.rs",
}

UNMAPPED = {
    "PalSpriteApplyAlphaMask", "PalSpriteDisplacementMap", "PalSpriteBoxFill",
    "PalSpriteCircleFill", "PalSpriteDeleteNoAlpha", "PalSpriteDeleteNoAlpha255",
    "PalSpriteMaskAlpha2", "PalSpriteMaskAlphaEx", "PalBoxBlurFilter",
    "PalGaussianBlurFilter", "PalAlphaCopyFilter", "PalMotionBlurFilter", "PalSpriteSwirlBlur",
    "PalSpriteWaveformWarp", "PalSpriteWallSpringEffect", "PalSpriteSetAddressUV",
    "PalSpriteSetRotateResolution", "StretchImage", "PalCopySpriteToSpriteChannel",
    "PalCopySpriteToSpriteRGBStr", "PalSpriteInfoCreateFromCache", "PalSpriteIsUseSpriteInfoCache",
}

MISMATCHES = {
    "PalSpriteSetScale": ["Separate newer X/Y scales and profile-scoped ABIs are implemented. Negative/nonfinite values remain stored, but portable geometry rejection/clamping can differ from native transformed drawing."],
    "PalSpriteMaskAlpha": ["Native mask input is MaskImage32, not a sprite handle.", "Portable RGBA blue-channel selection was corrected during this audit; native ABI mask-image adaptation remains separate."],
    "PalSpriteGetInfo": ["Portable PalSpriteInfo is not the native build-specific layout."],
    "PalSpriteSetInfo": ["Portable PalSpriteInfo is not the native build-specific layout."],
    "PalMixSpriteToSprite": ["Portable alpha-255 behavior matches MMX, not the native scalar special case."],
    "PalMixSpriteToSpriteEx": ["Integer retained-alpha/channel truncation and zero-output clearing are implemented. Portable overlapping surfaces use an immutable source snapshot rather than native in-place aliasing; unsafe nonpositive extents are rejected."],
    "PalSpriteHit": ["Unscaled selected-rectangle testing, any-nonzero alpha-test word, wrapping coordinates and fail-open missing pixel behavior are implemented; GPU lock failures are not simulated."],
    "PalChannelCopyEx": ["BGRA channel selection, clipping, copy/add/rounded-product and in-place alias feedback are implemented. Unsafe overflowing coordinates are rejected and native GPU lock failures/last-error residue are not simulated."],
    "PalTargetRenderListDraw": ["Ordered sprite-only callbacks, hidden sprites, transparent target capture and state resets are integrated through registered VM wrappers 3:82..84. Arbitrary foreign render callbacks are outside the typed arena model; locked sprites are skipped instead of sleeping in a blocked native thread."],
    "PalSpriteRenderCopy": ["All RGBA channels are copied; larger destinations center the logical capture with transparent padding. Physical D3D pitch/padding and driver-lock failure residue are not reproduced."],
    "PalSpriteViewCtrl": ["Portable visibility is bool; native API retains a 32-bit word and ViewIs returns that exact word."],
    "PalSpriteSetRenderMode": ["Legacy modes 1..4 and newer modes 1..11 use reviewed D3D RGB equations across CPU/wgpu/deko3d/GXM. Mode-11 blend-disabled/texture-RGB state persists across ordinary engine walks and remains separate from the mode word. Target-list state reset and transparent RGBA capture are integrated through registered VM wrappers 3:82..84. Native DIB kernels and offscreen alpha behavior differ from the opaque portable presentation model; console hardware remains untested."],
    "PalSpriteSetLayer": ["Reference export is a no-op while the newer export stores a layer word."],
    "PalSpriteReleaseArray": ["Native result is 0 on success, unlike ordinary PAL success-returning functions."],
    "PalSpriteSetTransition": ["Newer worker insertion uses before-object ordering, unlike older equal-priority insertion.", "Portable transition drawing still submits two ordinary textured draws instead of reproducing the native intermediate render-target combination and newer shared transform placement."],
}

HELPERS = {
    "PalSpriteLock": ["0x100b41e0"],
    "PalSpriteSetPos": ["0x100af4f0"],
    "PalSpriteMovePos": ["0x100af4f0"],
    "PalSpriteChangePriority": ["0x100af4f0"],
    "PalSpriteMaskAlpha": ["0x100b5810"],
    "PalSpriteMaskAlpha2": ["0x100b5810"],
    "PalSpriteMaskAlphaEx": ["0x100b5810"],
    "PalSpriteCircleFill": ["0x100b6750"],
    "PalBoxBlurFilter": ["0x100ba320"],
    "PalGaussianBlurFilter": ["0x100c45f0"],
    "PalMotionBlurFilter": ["0x100bac20"],
    "StretchImage": ["0x100c51f0", "0x100c5370", "0x100c4de0"],
    "PalTargetRenderListAdd": ["0x100b00f0"],
    "PalSpriteSetFxEffect": ["0x100a9540", "0x100a95a0"],
    "PalSetFxEffect": ["0x100a9540", "0x100a95a0", "0x100a96f0"],
    "PalSpriteSetOption": ["0x100c45a0", "0x100c45f0", "0x100c4b60", "0x100c7830"],
}


def build_rows():
    rows = []
    for build, digest in BUILDS.items():
        path = EVIDENCE / f"{build}-exports.json"
        evidence = json.loads(path.read_text())
        evidence.pop("input", None)
        evidence.update({"binary": "Pal.dll", "build": build, "sha256": digest})
        path.write_text(json.dumps(evidence, indent=2) + "\n")
        for function in evidence["functions"]:
            name = function["name"]
            key = canonical(name)
            spec, result, behavior = specs[key]
            issues = []
            if build != "totsulover" and key == "PalSpriteSetScale":
                spec = "sprite:ptr32<Sprite>,uniform_scale:f32"
            if build == "reference" and key == "PalSpriteSetLayer":
                spec = ""
                issues.append("No argument reads in the constant-success stub; arity is not established for this build.")
            if result.startswith("unresolved"):
                issues.append("Source-level return type remains unresolved; do not emit a guessed callable declaration.")
            if key == "PalSpriteDebugList":
                issues.append("Empty body proves no side effects but does not independently establish argument arity.")
            if key == "PalSpriteMaskAlpha" and build != "totsulover":
                behavior = "Uses borrowed MaskImage32 {width,height,pixels}; applies alpha=(destination_alpha*mask_blue)>>8 over the caller's exact extent, without clipping. Invalid rectangles can write out of bounds. Return inherits Unlock's backend-dependent EAX."
                if build == "koikake":
                    spec = spec.replace("width:i32,height:i32", "width:u32,height:u32")
            if key == "PalSpriteSetOption" and build == "reference":
                behavior += " Reference GPU path additionally recognizes 0x905 as direct full-size blur; reference software and newer/Koikake paths do not."
            if key == "PalSpriteGetInfo" or key == "PalSpriteSetInfo":
                behavior += " Layout profile: " + {
                    "koikake": "76 bytes, no filename or advanced fields.",
                    "reference": "340-byte storage footprint; filename starts +80 and strncpy writes at most 259 bytes.",
                    "totsulover": "372-byte storage footprint; filename starts +84, physical dimensions +344/+348, advanced fields +352..368.",
                }[build]
            if key == "PalSpriteSetRenderMode":
                behavior += " " + (
                    "Newer D3D recognizes modes 1..11. Mode 11 disables alpha blending and selects texture RGB; ordinary draws restore only blend operation/factors, leaving those two states latched across nodes and frames. TargetRenderListDraw restores default state before each callback and after the walk."
                    if build == "totsulover" else
                    "Legacy D3D recognizes modes 1..4 only. Non-default draws restore SRCALPHA/INVSRCALPHA; unsupported mode words therefore draw with initialized default factors."
                )
            if key == "PalEffectEx":
                behavior += " Accepted IDs are below " + {"koikake": "30", "reference": "50", "totsulover": "100"}[build] + "."
            if key == "PalRenderInit" and build == "totsulover":
                behavior += " Also initializes the global sprite/render critical section."
            if key == "PalRenderUninit" and build == "totsulover":
                behavior += " Also deletes the global sprite/render critical section, even when the supplied list is null."
            if key == "PalSpriteRelease" and build == "totsulover":
                behavior += " Entire teardown is protected by the global sprite/render critical section."
            portable = MAPPINGS.get(key)
            if key in UNMAPPED:
                portable = "No direct reviewed export adapter identified; do not infer implementation from a similarly named VM extcall."
            elif portable is None:
                portable = "crates/pal-vm/src/sprite.rs / renderer/ / runtime.rs (portable equivalent, not a native ABI adapter)"
            row = {
                "name": name,
                "canonical_name": key,
                "build": build,
                "binary_sha256": digest,
                "address": function["ea"],
                "evidence": [str(path.relative_to(ROOT))],
                "abi_status": "unresolved" if issues else "reviewed_machine_contract",
                "behavior_status": "reviewed_export_boundary",
                "calling_convention": "cdecl",
                "arguments": arguments(spec),
                "return_type": result,
                "behavior": behavior,
                "portable_mapping": portable,
                "portable_differences": MISMATCHES.get(key, []).copy(),
                "unresolved": issues,
                "review_basis": ["export body disassembly", "stack argument loads", "memory reads/writes and explicit return paths"],
            }
            if build == "totsulover" and key in HELPERS:
                row["reviewed_helper_addresses"] = HELPERS[key]
                row["evidence"].append("reverse/evidence/pal-graphics/totsulover-helpers.json")
                row["review_basis"].append("named helper body and call-site register/stack adaptation")
            if key == "PalSpriteSetTransition" and build == "totsulover":
                row["evidence"].append("reverse/evidence/pal-graphics/totsulover-draw.json")
                row["reviewed_helper_addresses"] = ["0x100b45e0", "0x100b4270", "0x100b4aa0"]
            if key == "PalSpriteSetRenderMode":
                if build == "totsulover":
                    suffixes = ["draw", "draw-helpers", "blend-reset", "render-walk", "render-end"]
                    addresses = ["0x100b4b80", "0x100cd710", "0x100cc860", "0x100caca0", "0x100af870", "0x100ca650"]
                else:
                    suffixes = ["mode-callback", "mode-draw", "blend-modes"]
                    addresses = {
                        "reference": ["0x100b1e00", "0x100c6420", "0x100c5530"],
                        "koikake": ["0x100c48a0", "0x100ab470", "0x100aa650"],
                    }[build]
                row["evidence"].extend(f"reverse/evidence/pal-graphics/{build}-{suffix}.json" for suffix in suffixes)
                row["reviewed_helper_addresses"] = addresses
                row["review_basis"].append("sprite callback through GPU draw, D3D blend-state switch, post-draw restoration and surrounding draw-walk boundaries")
            if key == "PalSpriteHit":
                row["behavior"] += " Any nonzero alpha-test word enables pixel alpha lookup. Missing texture, out-of-range source coordinates, or failed lock returns an opaque -1 sentinel and accepts a geometrically valid hit."
                if build == "koikake":
                    row["evidence"].append("reverse/evidence/pal-graphics/koikake-hit-pixel.json")
                    row["reviewed_helper_addresses"] = ["0x100aa530", "0x100a52c0"]
            if key == "PalSpriteRenderCopy" and build == "totsulover":
                row["behavior"] += " Temp-target capture helper preserves all four BGRA bytes, unlike thumbnail capture; requires destination at least logical size and copies a centered destination-sized region."
                row["evidence"].append("reverse/evidence/pal-graphics/totsulover-target-copy.json")
                row["reviewed_helper_addresses"] = ["0x100cb700"]
            if key == "PalTargetRenderListDraw" and build == "totsulover":
                row["behavior"] += " Calls default-state reset before every matching callback and after the list, restoring alpha blending and texture/diffuse modulation changed by mode 11."
                row["evidence"].append("reverse/evidence/pal-graphics/totsulover-blend-reset.json")
                row["reviewed_helper_addresses"] = ["0x100caca0"]
            if name.startswith("?"):
                row["review_basis"].append("MSVC decorated export signature, where present")
            rows.append(row)
    return rows


for path in EVIDENCE.glob("totsulover-*.json"):
    data = json.loads(path.read_text())
    data.pop("input", None)
    data.update({"binary": "Pal.dll", "build": "totsulover", "sha256": BUILDS["totsulover"]})
    path.write_text(json.dumps(data, indent=2) + "\n")

for build in ("koikake", "reference"):
    for path in EVIDENCE.glob(f"{build}-*.json"):
        data = json.loads(path.read_text())
        data.pop("input", None)
        data.update({"binary": "Pal.dll", "build": build, "sha256": BUILDS[build]})
        path.write_text(json.dumps(data, indent=2) + "\n")

COMMON_INFO = [
    (0, "visible", "u32"), (4, "argb", "u32"),
    (8, "position_x", "f32"), (12, "position_y", "f32"), (16, "position_z", "f32"),
    (20, "offset_x", "i32"), (24, "offset_y", "i32"),
    (28, "texture_width", "u32"), (32, "texture_height", "u32"),
    (36, "cell_width", "u32"), (40, "cell_height", "u32"),
    (44, "center_offset_x", "i32"), (48, "center_offset_y", "i32"),
    (52, "base_priority", "i32"), (56, "scale_x", "f32"),
]
OLD_INFO = COMMON_INFO + [
    (60, "rotation_x", "f32"), (64, "rotation_y", "f32"),
    (68, "rotation_z", "f32"), (72, "render_mode", "u32"),
]
NEW_INFO = COMMON_INFO + [
    (60, "scale_y", "f32"), (64, "rotation_x", "f32"), (68, "rotation_y", "f32"),
    (72, "rotation_z", "f32"), (76, "render_mode", "u32"), (80, "layer", "u32"),
    (84, "resource_name", "char[260]"),
    (344, "physical_width", "u32"), (348, "physical_height", "u32"),
    (352, "address_uv", "u32"), (356, "skew_x", "f32"), (360, "skew_y", "f32"),
    (364, "perspective_enabled", "u32"), (368, "rotation_resolution", "f32"),
]


def fields(items):
    return [{"offset": offset, "name": name, "type": value_type,
             "width_bytes": 260 if value_type == "char[260]" else 4}
            for offset, name, value_type in items]


LAYOUTS = [
    {"name": "PalSpriteInfoKoikake32", "build": "koikake", "accessed_bytes": 76,
     "alignment": 4, "fields": fields(OLD_INFO),
     "notes": "SetInfo also writes texture wrapper dimensions. Source rectangle and node order are not included."},
    {"name": "PalSpriteInfoReference32", "build": "reference", "storage_footprint": 340,
     "alignment": 4, "fields": fields(OLD_INFO + [(76, "auxiliary_word", "u32"), (80, "resource_name", "char[260]")]),
     "notes": "Only 259 filename bytes are copied by strncpy; the final byte is not initialized by GetInfo. The semantic name of auxiliary_word is unresolved."},
    {"name": "PalSpriteInfoTotsulover32", "build": "totsulover", "storage_footprint": 372,
     "alignment": 4, "fields": fields(NEW_INFO),
     "notes": "GetInfo leaves physical dimensions unchanged when the texture is absent. SetInfo does not copy the physical-width/height outputs. SetInfo terminates the sprite's internal filename at byte 259; GetInfo only copies 259 bytes."},
    {"name": "PalSpriteTransition32", "build": "all", "size": 32, "alignment": 4,
     "fields": fields([(0, "type", "i32"), (4, "state", "i32"), (8, "render", "ptr32<Render>"),
                       (12, "first", "ptr32<Sprite>"), (16, "second", "ptr32<Sprite>"),
                       (20, "last_frame", "i32"), (24, "duration_ms", "u32"), (28, "start_time_ms", "u32")]),
     "notes": "States: allocation/cancel=0, start=1, elapsed>=duration=2, null query=3. Cancel owns/releases second; first is retained. No flags word exists in this layout."},
    {"name": "PalMaskImage32", "build": "all", "accessed_bytes": 12, "alignment": 4,
     "fields": fields([(0, "width", "u32"), (4, "height", "u32"), (8, "bgra_pixels", "ptr32<u8>")]),
     "notes": "MaskAlpha borrows this image record, not PalSprite. Koikake helpers confirm data +8 and width +0; newer clipping additionally reads height +4."},
    {"name": "PalRenderNode32", "build": "all", "size": 48, "alignment": 4,
     "fields": fields([(0, "object", "ptr32<Opaque>"), (4, "callback", "ptr32<Callback>"),
                       (8, "priority", "i32"), (12, "state", "i32"),
                       (40, "next", "ptr32<RenderNode>"), (44, "previous", "ptr32<RenderNode>")]),
     "notes": "Header bytes +16..39 hold an x86 CRITICAL_SECTION. Ordinary allocated nodes do not initialize this range. Callback receives (core_pointer, object_pointer), both 32-bit cdecl words, and a zero result removes the node."},
]

rows = build_rows()
payload = {
    "schema_version": 1,
    "scope": "All PalSprite/PalRender/PalEffect exports and explicitly assigned blit, filter, pixel, target-render and FX siblings in the three release PAL DLL inventories. Movie/MSprite exports are excluded.",
    "review_date": "2026-10-23",
    "builds": BUILDS,
    "coverage": {build: sum(row["build"] == build for row in rows) for build in BUILDS},
    "status_definition": {
        "reviewed_machine_contract": "Stack slots, widths and return path have been reviewed; this does not certify a portable adapter or every downstream OS operation.",
        "reviewed_export_boundary": "Named export's control flow, state writes, major helper path and edge conditions are recorded. See narrative for downstream shader/kernel limitations.",
        "unresolved": "A listed ABI fact remains ambiguous and must not be turned into a guessed C/Rust declaration.",
    },
    "functions": rows,
    "shared_layouts": LAYOUTS,
    "boundary_examples": [
        {"function": "PalSpriteViewCtrl/PalSpriteViewIs", "input": "visible=0x80000002", "expected": "ViewIs returns the same 0x80000002 word, not 1."},
        {"function": "PalSpriteSetPos/ChangePriority/MovePos", "input": "base=100; SetPos z=9.75; ChangePriority(101); MovePos dz=0", "expected": "Node priorities are 109, then 101, then 110. The setter sequence matters."},
        {"function": "PalSpriteSetTransition", "input": "start=1000,duration=250,current=1250,type=1", "expected": "state=2; first alpha reaches its stored alpha, second fades to zero. The worker remains until cancellation/release."},
        {"function": "PalSpriteSetTransition", "input": "start=0xFFFFFFFC,current=1,duration=10", "expected": "Wrapping elapsed is 5; interpolation fraction is 0.5."},
        {"function": "PalSpriteMaskAlpha", "input": "destination alpha=255, native mask BGRA=(255,0,0,255)", "expected": "Alpha becomes 254. A red-only mask BGRA=(0,0,255,255) yields alpha 0."},
        {"function": "PalMixSpriteToSprite", "input": "source channel=255,source alpha=255", "expected": "Scalar output channel=255; MMX output channel=254; destination alpha is retained."},
        {"function": "PalSpriteReleaseArray", "input": "count=0", "expected": "Returns 0 without dereferencing the array."},
    ],
}
OUT.parent.mkdir(parents=True, exist_ok=True)
OUT.write_text(json.dumps(payload, indent=2) + "\n")
print(json.dumps(payload["coverage"]))
