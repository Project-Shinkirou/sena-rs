# SoftPAL 运行时逆向资料：语音、保存、Tips、宿主消息框

更新：2026-10-23。本文记录本轮实际检查到的原生行为、Rust 实现和待补部分；它不是“整个引擎已逆向完成”的声明。目标游戏目录只读，测试写入临时目录。实现应保持 PAL 通用语义，不依赖 Koikake 私有变量和台词。

## 证据与阅读规则

- 原始证据：本仓库 `reverse/Game.sqlite` 的 `decompilations` 表、`reverse/PAL.sqlite`，以及本机 IDA 9 对 Game 数据库的重新反编译。
- 本文引用的 Game 地址对应这个数据库代表的 EXE。不同版本可能地址不同；必须用参数弹栈、表项和调用链识别函数，不能只靠旧 IDB 函数名。旧名已有错位实例。
- 可独立阅读的 C 伪代码副本位于 [`evidence/runtime-audit/`](evidence/runtime-audit/)。它们保留反编译器的类型猜测和伪变量；数组索引和指针类型不等于已确认的 C 结构声明。
- `target/reverse-audit/select-voice.json`、`tips.json` 是本轮更完整的反汇编/伪代码/交叉引用输出。`target/` 可清理，核心函数已复制到上述 `reverse/` 目录。
- 所有本文“参数顺序”均指 VM **弹栈顺序**；构造脚本时通常要逆序压栈。
- `pal-script/src/extsig.rs` 的 `Blocked` 是签名验证状态，不代表运行时没有处理器。反之，处理器已存在也不代表整个子系统完整。

## 1. 语音自动声像 Autopan

### 1.1 注册与播放入口

| Game 地址 / 分派 | 已确认合同 |
| --- | --- |
| `0x443B60`，category 13/index 14；文本目录 index 62 | 弹一个 `limit`，保存 `100 * limit` 到 `ctx+661488`。初始化幅度，不关闭 enable latch。此前零参数并关闭开关的近似实现错误。 |
| category 13/index 15；文本目录 index 63 | 弹 enable 值；非零启用。 |
| `0x4438D0`，category 13/index 16；文本目录 index 64 | 弹 `(slot, target, name, mode)`。注册表 16 槽，每项 40 字节，表基址 `ctx+658964`。`target==-1 && mode==0` 清除该项。名称最多 32 编码字节。 |
| category 13/index 17；文本目录 index 65 | 返回 enable latch。 |
| `0x442550` | 从注册槽 0 到 15 依序匹配名字前缀，忽略 ASCII 大小写；第一个有效匹配生效，而非最长前缀。 |
| `0x442C10` | 开声音时复制命中的目标到播放槽；flags `0x80` 跳过自动声像。 |
| `0x4457F0` | ADV 任务每帧刷新活动声音的声像，包含脚本停在文字等待中的帧。 |

Rust 位置：`crates/pal-vm/src/runtime.rs` 的 `voice_autopan_match`、`voice_autopan_value`、`update_voice_autopan`、`load_named_audio`；状态保存在 `TextSubsystemState` 的注册表和播放表中。开新声道时清除旧声道的跟踪项，逐帧根据 `game_audio` 验证仍对应同一活动句柄。

### 1.2 几何与 PAL 音量单位

`0x442430` 的固定位置模式 `mode!=0` 将 target 的 0..20 对应到 1920 宽参考空间：`x=target*96`。左侧越界饱和为 `-limit`，右侧越界饱和为 `+limit`。范围内为：

```text
pan = trunc((x / 1920.0) * (2 * limit) - limit)
```

`mode==0` 从精灵包装表中查找目标，调用 `PalSpriteGetInfo`，使用 `trunc(x)+(width>>1)` 作为中心。没有有效精灵时不写新 pan。原生只在 pan 改变时调用 `PalSoundSetPanChannel`。

PAL 的 `PalSoundSetPanChannel` 最终使用 DirectSound 的声像接口；单位为百分之一 dB，范围 -10000..10000。它降低远侧声道而不放大近侧声道。Kira 的等功率 panning 不能直接接收这个整数，因此 `audio.rs::pal_pan_settings` 同时换算 pan 和整体增益：

```text
q = 10 ^ (-abs(pan)/1000)          # 远侧功率比例
kira_pan = sign(pan)*(1-q)/(1+q)
gain_db = 10*log10((1+q)/2)
```

测试把单位左右声道经 Kira 的 `Frame::panned` 实际处理，验证近侧幅度 1、远侧幅度 `10^(-abs(pan)/2000)`，覆盖正负、中心和范围边界。

### 1.3 未验证边界

- 此轮没有真实立绘移动配合双声道听音的验收。仓库精灵系统存在原生 1920 空间与逻辑投影两类路径；当前实时 `PalSprite` 和无后端时包装表回退的几何一致性，还需原机比较。不要把合成固定坐标测试当作所有缩放/投影布局都已对齐。
- 名称前缀采用 Rust 字符串上的 ASCII 大小写折叠；非 ASCII 资源名的原生字节大小写规则未验证。
- 注册表越界输入不复刻原生越界内存访问；播放只扫描 0..15。
- enable 在声音播放后切换的所有边界、存档后恢复仍在播放的 voice，没有完整原机对照。

## 2. MIX：独立语音槽，不是先求和再播

### 2.1 原生加载流程

`0x442C10` 首先尝试 `<voice>.mix`。找到列表时释放 voice 槽 0..7（每槽 100 字节），不使用调用方请求的槽号作为 MIX 目的槽；独立槽 8 保留。最多读八项，每项各自创建 PAL 音频句柄，全部加载后进入播放循环。

| flags | 已确认作用 |
| --- | --- |
| `0x01` | 循环播放。 |
| `0x02` | 将对应槽的位写入原生等待/声音状态掩码；这一掩码体系还未完整移植。 |
| `0x04` | 用 `PalSoundPlayFade(handle, looping, fade_ms)`；普通播放用 `PalSoundPlay`。Rust 已传递第四参数到普通语音和 MIX 各 lane 的 Kira fade-in（见 `RUST_VOICE_FADE_FIX.md`）；原生曲线尚未逐采样核对。 |
| `0x40` | 跳过 `0x4425F0` 的按名字角色音量处理。 |
| `0x80` | 跳过前缀匹配及自动声像。 |
| `0x100` | 原生影响 BGM/voice 关联音量路径；不可简单视为“无效位”，尚未完整移植。 |

原生文件读取每次 64 字节，随后把各 16 位字符格的低字节复制到窄字符串。当前 `parse_mix_playlist` 同时接受旧有 32 字节零填充记录和 64 字节 UTF-16LE 记录；ASCII 名称与观察到的低字节转换一致。**非 ASCII 宽字符名称未完成验证**：现实现还要求首字符是 ASCII graphic，且其 UTF-16 解码并不等价于原生低字节提取，不能声称支持任意 Unicode MIX 名。

### 2.2 Rust 语音路径

`runtime.rs::load_voice_audio` 专门处理 category 13：探测 MIX、清除八槽、逐成员解析角色音量及 mute、逐成员注册 autopan、先加载再启动。`load_named_audio` 继续服务单声音/BGM 等普通路径。

`audio.rs::decode_game_audio` 的旧 PCM 求和能力仍保留给不经过 category 13 的调用者。这是兼容保留，不是现在 ADV MIX 的播放方式。

`0x43C2C0` 对新 ADV voice 清除旧的八个 ADV 槽。因此上一句为多人 MIX、下一句为单人 voice 时，上一句其余成员不能继续残留。`release_adv_voice_slots` 同时清除这八槽的 autopan 跟踪。

`voice_play` 第四参数是 fade 时间，不能作为 channel 音量；通常为 0，误当音量会把声音关掉。角色音量应通过 `voice_unit_slot_for_name` 查角色表，再通过 channel volume 与 group volume 叠加。

### 2.3 独立审查发现并修正的等待范围

`0x443800` 的 `voice_wait(-1)` 写的是 **0xFF**，只包含 0..7；等待指定 8 则写 `1<<8`。审查发现 Rust 先前用 `slot<0` 匹配全部 category-13 句柄，将独立槽 8 也包含在“全部 ADV voice”等待中。如果槽 8 循环，等待可能永不结束。

本轮将筛选改为：`slot==-1` 仅选 0..7，其余选择精确槽。已有等待逻辑会在首次弹参数后保存 slot、PC 回退 12 字节并等待一帧；重入时不重复弹参数。此处修正由明确原生掩码支持；没有复制原生对其它异常负数移位的未定义/平台行为。

### 2.4 测试与缺口

合成 WAV + 临时目录测试覆盖九成员输入只取前八、slot 8 保留、不同成员音量/mute/pan、`0x40/0x80` bypass、下一句替换整组和 stop-all。音频设备关闭的测试验证配置与句柄生命周期，**不验证可听同步、真实播放时长或硬件输出**。fade、`0x02/0x100` 状态路径还需补齐。

## 3. 保存槽与“最近保存”

| 地址 | 行为 |
| --- | --- |
| `0x432620` `is_save` | 只有参数 `-1` 扫描编号 0..999 的 `save%03d.dat`。其它值是精确编号文件。扫描不包含 `continue.dat`。 |
| `0x44D320` 初始化 | task-data `+24` 初始为 `-1`。 |
| `0x4312F0` `get_new_savefile` | 原样返回 task-data `+24`；不是按文件时间找最新存档，也不将 -1 钳到 0。 |
| `0x433D10` save | savepoint 已武装、槽在 0..999 且第二参数非零时，记录“最近保存”槽号。 |
| `0x431130` delete | 返回 `DeleteFileA` 结果；只要删的是记忆槽，就把记忆槽置 -1，即便文件已不存在而删除返回失败。 |

文件名约定：只有 `slot==-1` 对应 `continue.dat`；例如 -2 按原生格式命名为 `save-02.dat`。普通保存的文件格式在 `save_format.rs`；运行时查询和 remembered-slot 在 `runtime.rs`。

Koikake 标题脚本在 `0x57CE4` 调 `is_save(998)`，在 `0x57D74` 调 `is_save(-1)`。后一项控制 DATA LOAD，普通编号存档足够，不能把这个条件解释为只支持 continue。本轮开始时通配查询已存在；旧 AGENTS 续接笔记说“缺失”已过时。本轮改的是异常负数范围、初始/返回 sentinel、失败删除时清除记忆槽，不是新发明通配查询。

`tests/save_semantics_fixture.rs` 用真实构造的 Sv20 指令经公开 runtime 执行，六项测试覆盖标题两查询、扫描上下界、排除 continue、旧 portable `.sav` 兼容、负编号、记忆槽生命周期。写文件只在临时目录。

### 3.1 存档格式当前范围

- 原始前缀可写标题、缩略图/mosaic、lock、恢复文字命令位置（0x20C），后面附加 `SENARSAV` 可移植快照。
- 当前快照版本 7：此前 v4 加按钮格尺寸/源矩形/颜色，v5 加 BGM 列表，v6 加 sprite 原生投影和 center-scale，v7 加 ADV 窗口 alpha。恢复仍需向后兼容旧版本。
- 原生无 trailer 存档可读取文字值（0x12A3C）和恢复命令位置，重新建立文字及点击等待；这不等于全镜像还原。
- **未完成**：原生镜像声音 wrapper、完整 sprite/history/window 记录映射、写出原生引擎可端到端接受的镜像/镜像重复块。导入原生存档的场景仍靠后续脚本渐进重建，BGM 没有 portable track list 时不会立即恢复。
- 标题查询测试不等于本轮做了完整游戏 UI、菜单保存、重启读档的原机回归。

## 4. Tips：已接命令层，未接完整布局/交互

category 总表 `0x49C360`，category 2 表 `0x4BB490`。32 项记录，每项 0x182C 字节：11 个 dword、一个换行标记、三个 2047 字节字符串及尾部对齐。整个状态初始化清零大小为 0x3059C。

| index / 地址 | 弹栈 | 语义 |
| --- | --- | --- |
| 36 / `0x441950` | 两个回调点 | 已补注册处理器：保存 point id、清 32 个记录，保留 count/mode/flag/colors；回调触发链仍待接入。 |
| 37 / `0x441A30` | 0 | 返回 `max(count-1,0)`，是末个完成索引，不是数量。 |
| 38 / `0x441A70` | `(record,dst,selector)` | 三字符串中选一写动态字符串 dst，返回 record ID。 |
| 39 / `0x441BD0` | `(record,selector)` | 查询参数，见下表。 |
| 40 / `0x441DC0` | 0 | 清当前槽和槽 0，count=0、mode=1；其余槽、pause、颜色保留。 |
| 41 / `0x441E50` | `(dynamic_string)` | strcmp 精确匹配第一个字符串，返回最后一个匹配项；无匹配 -1。 |
| 42 / `0x441F80` | `(color0,color1)` | color0=-1 只关闭自定义色；否则保存两色并启用。关闭时保留已存颜色。 |
| 43 / `0x441E20` | 0 | mode=256；不是 init，也不改 pause。 |
| 44 / `0x442030` | 0 | 返回 flag 字节。 |
| 45 / `0x442070` | 0 | 完整清空 Tips 状态。 |
| 46 / `0x4420B0` | 0 | 返回旧 flag，然后 flag XOR 1。 |

`get_param` 的 selector 0..8 对应记录前九 dword；9 是换行标记；10/11 为 width/height，加当前字体效果 padding（effect 0:0；1/2:2；3:4）。12..17 已通过 `text_init`（`0x441400`）写入位置确认：依次返回初始化字号、正文 x/y、姓名 x/y、最后的行距/模式参数（上下文 dword 4/6/7/10/11/5），现已接入 Rust。它们保留有符号 32 位脚本原值，不应用 1.5 倍或窗口投影缩放，也不随后续共享字体字号/当前文本模式改变。`text_uninit` 清零这些值；视觉小说模式初始化使用独立的 dword 13..16。原始函数证据保存在 `reverse/evidence/runtime-audit/sub_441400.c` 等。大于 17 原生返回 0。读取记录的 selector 对非法 record 使用边界检查，不复制原生越界读取；12..17 不访问记录，因此和原生一样忽略 record 索引，包括负数及越界值。

此前 get_str/get_param/set_color/get_flag 的近似弹栈数 1/1/1/1 错误；现在分别为 **3/2/2/0**。源码 `crates/pal-vm/src/runtime/tips.rs` 的六项测试验证精确栈平衡、动态字符串写入、重复检索、暂停返回旧值、reset/init/stop 差异、颜色保留、padding，以及全部 context selector 在不同初始化值、窗口尺寸和共享字体状态下的原始返回单位。

### 4.1 已找到但未接的数据生产者

`0x43C7C0` 在文字布局过程中处理 `<dNUMBER>...</d>`。开始标签记录 ID、起点 x/y，可能改变文本颜色；结束标签记录原始正文、宽高、结束坐标、跨行拆分内容，推进计数并将 mode 置 257。原生开始路径在 count 达 31 时绕回 0。

`0x439480` 从交互路径触发一个回调，`0x43DB40` 在文字完成路径触发另一个。不能只看到标签就把两个回调同时执行。布局坐标、跨行、点选区域与回调执行顺序仍需连通。

**现状**：producer 尚未接入，所以正常运行的记录查询仍看到初始化的空记录。custom colors 只存储，未应用到 glyph；Tips 状态未序列化；没有可用的完整 Tips UI。不能以命令测试通过宣称 Tips 完成。

本地完整 Koikake 指令遍历共 67,196 extcalls，报告中未出现 Tips 名称，category 2/index 38..46 无调用。结论仅针对这个解码脚本，并不能推广到所有 PAL 游戏或所有语言发行版。

## 5. Native messagebox 与宿主 ABI

没有证据表明 `0x415C00` 是通用 messagebox opcode。它实际为 `player_name_set_begin`，弹七参数，只在名字过长的错误路径调用 MessageBoxA。已见原生错误消息框调用点包括 `0x4053D0` 初始化、`0x430760` 写存档、`0x44C2F0/0x44C560/0x44C7C0` 归档错误、`0x44D320` 文字/文件表容量警告。

### 5.1 FFI 当前状态机

`crates/pal-vm/src/ffi.rs` 已把 Android/iOS/pump 三组 callback 和 submit-result ABI 连接到 fatal-frame-error：

```text
正常 step
  ├─ update Err / 捕获 update panic / RuntimeStatus::Faulted
  │    ├─ 无 callback → last_status=2，立即返回 2
  │    └─ 有 callback → 分配 request_id，发 kind=0 / OK
  │                         ↓
  │                    后续 step 返回 0，VM 不再运行
  │                         ↓ submit(handle,id,0)
  │                    清 pending，返回 2；以后保持 2
  └─ 正常结束 → status=1
```

回调签名传 `(user_data, request_id:u64, kind:i32, title*, message*)`。kind 0 的 OK 结果为 0。title/message 是回调期间有效的 C 字符串；宿主必须同步复制后再交给 UI。错误文本中的嵌入 NUL 替换为空格。

UI 线程结果经 `Mutex<HashMap<usize,(u64,bool)>>` 邮箱提交，不解引用 host 指针。request ID 来自进程级 AtomicU64；这避免 host 地址被新分配复用时，旧窗口确认误确认新请求。step 消费匹配确认；过期 ID、非 0 结果忽略。销毁/unregister 删除 mailbox 项。回调期间不保留 host 可变借用，允许 Android 缺 Activity 时同步回送 fallback 结果。

### 5.2 明确边界

- 这是 fatal frame 错误通道，不是“所有原生 MessageBoxA 调用点全部移植”。保存/资源/玩家姓名各站点尚未统一连接。
- 创建引擎失败发生在 callback 注册之前，仍返回 null；不能依赖该回调显示 create 错误。
- 确认错误不让 VM 继续，以免把已损坏状态当成功执行。
- 仅 submit-result 邮箱设计为跨线程访问；其余 host 生命周期/step/set_callback 仍要求宿主按原有线程和生命周期合同调用，不能把它们视为任意并发安全接口。
- 实际 UIKit/Android/AppKit 弹窗尚需设备/宿主验证。

四项 `native_messagebox_tests` 覆盖跨线程确认、同步 callback 回送、空 handle、无 callback、过期 ID、重复 step、unregister、destroy 后晚到结果。

## 6. 验证索引与后续顺序

本轮已有日志 `target/reverse-audit/ffi-tests.log`（4 passed）、`tips-tests.log`（4 passed），以及合并时 `all-lib.log`（83 passed、4 ignored）。后者只代表该次快照，不代替此后 decoder/Select 等并行改动的最终集成测试。

建议复现：

```sh
cargo test -p pal-vm --lib voice_
cargo test -p pal-vm --lib directsound_pan
cargo test -p pal-vm --lib mix_playlist
cargo test -p pal-vm --lib tips::tests
cargo test -p pal-vm --lib native_messagebox_tests
cargo test -p pal-vm --test save_semantics_fixture
```

全 workspace/all-targets 有已知缺少 `testcase/SYSTEM.INI` 的夹具失败；WMV 真游戏 fixture 缺文件时有 ignored 测试，不能把未执行的测试报为通过。

下一轮最有价值的验证：原机 voice_wait 的 slot 8 对照及 flags 掩码；sprite 投影/缩放下的 autopan 听音与坐标追踪；Tips 布局 producer + 双回调；原生存档结构映射；宿主实际错误弹窗验收。每项应先建立可重复输入与原生证据，再替换当前近似。
