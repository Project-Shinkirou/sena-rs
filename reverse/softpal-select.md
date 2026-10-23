# SoftPAL Select 状态机与调用约定

更新：2026-10-23。地址仅适用于 [manifest](evidence/manifest.json) 标识的本地 Game.exe。
证据：[完整 IDA 导出](evidence/select-voice.json)，含伪代码、反汇编、xref、派发表。
实现：`crates/pal-vm/src/runtime/select.rs`；签名：`crates/pal-script/src/extsig.rs`。

## 为什么旧实现不对

原实现只记录字符串和选项，主脚本继续执行，既没有生成界面，也没有等待选择及跳到分支。
旧 IDB 的名称还存在错位：0x435C90 实际是提交/轮询，旧名却是 SelectClear。
此文按函数体与表 `0x4BB350` 定义语义，不能只相信 IDA 名称。

## 调用表

参数顺序为连续 pop 的顺序（脚本通常反序 push）。第三个 extcall DWORD 是结果目标，不是普通栈参数。

| cat:index | 地址 | 参数 | 行为 |
|---|---|---|---|
| 6:0 | 0x4369F0 | background, normal_color, hover_color, color3 | 首参为 FILE.DAT 资源；颜色补不透明 alpha |
| 6:1 | 0x436240 | text_visible | 开始声明段、设置 ADV 可见性，native 内部消费随后的 6:2 直到 6:3 |
| 6:2 | 0x4361D0 | text, point, x, y, flags, row | 由 6:1 内部读取；单独调用 native 此函数为错误 |
| 6:3 | 0x435C90 | 无 | 回退 PC 12 字节逐帧轮询；选择后跳 point |
| 6:4 | 0x435C30 | 无 | 释放背景和文字两组 sprite，保留配置和回调 |
| 6:5 | 0x435BA0 | index, offset | 第 index 项的背景基准行设置为 3 × offset |
| 6:6 | 0x435A70 | enter, leave, choose | 注册三个 point；0 表示未注册 |
| 6:7 | 0x435940 | lock | 仅强制背景基准行 0，不禁止选中 |
| 6:8 | 0x435A30 | 无 | 当前 reaction 低字节减 1 |
| 6:9 | 0x4359F0 | 无 | 上一 reaction 低字节减 1 |
| 6:10 | 0x4359A0 | 无 | current & 0x200 时返回选项索引，否则 -1 |

6:12..17 是共享 skip/auto 控制，不能错误解释为 select lock。

## 选项与渲染

- 最多 16 条声明。flags bit 0 控制显示，bit 1 控制可选；隐藏项不占显示索引。
- text 走 TEXT.DAT，background 走 FILE.DAT，不能把背景 id 当提示文字。
- 最后一个声明参数是背景帧行；不是颜色、是否启用或行数。
- native 将脚本 x/y 乘 1.5 放入 1920 参考空间；移植使用项目的逻辑分辨率映射。
- 背景使用资源 cell 尺寸，文字居中并追加脚本 y 偏移 4。不可选项降 alpha。
- native priority 为 4871 加当前 sprite priority cursor；移植按现有排序方向取负值。
- 普通/hover 颜色分别用于背景和标签；hover 帧为基准行 + 1。
- 第三个 init 颜色已存储，但作用尚未由本轮证据确认，不虚构用途。

## 轮询、分支与回调

初次到达 commit 先停一帧，避免打开选择界面的点击立即选中。
无选择时 PC 停在 commit。接受选择后释放所有选择 sprite，PC 改为 Point.dat 对应地址；
point 0 按 native code-base 处理，不能当作顺序执行。

`reaction = current | (previous << 16)`；current 低字节为 index+1，接受选择时再置 `0x200`。
没有 hover 时低字节为 0，所以查询返回 -1。

在 reaction 变化时，native 先压入 enter/choose，再压入 leave；实际执行次序因此是
leave → enter/choose → 原 commit 或分支目标。choose/enter/leave 在 ctx 中的物理顺序
不同于 pop 顺序，`0x435A70` 明确把参数映射到 ctx[165427]/[165428]/[165426]。

原生确认键根据 ctx+12956 选择 push 或 pull，掩码为 0x800006。当前移植使用 push；
该字段来源已定位为 [btn_mode / 0x410BA0](evidence/btn-mode.c)，category 8 index 36，
pop 一个整数写成 bool，并调用 PalButtonMode；完整 pull 路径仍待接线。上下键在 native 中移动系统鼠标；
移植使用内部焦点并在鼠标实际移动时取消，因此系统光标的位置不完全等价。
重叠项按声明顺序逐个 hit-test：无点击时全部命中项都高亮，最后命中项写入 reaction；
确认时立即接受第一个命中项。实现已据 `0x435C90` 的循环修正，独立测试覆盖两者差异。
键盘导航使用 ctx+661680 独立游标，begin 初始化为 -1；第一次 Up/Down 都到第 0 项，
上下边界夹紧，鼠标 hover 不修改游标。移植内部焦点也按目标 cell 中心重新 hit-test 重叠项。

## 验证

- `cargo test -p pal-vm --lib runtime::select`：7 项构造脚本测试通过，1 项本地游戏测试默认忽略。
- `cargo test -p pal-vm --lib select_koikake_first_choice -- --ignored --nocapture`：本地只读资源验证通过。
- Koikake 从 `0x7C1EC` 开始，停在 commit `0x7C278`；两个选项共 4 个 sprite。
  背景 cell 785×49，逻辑区域 `[210,205,995,254]` 和 `[225,260,1010,309]`。
  点击第二项跳到 point 3641，选择 sprite 全部释放。
- 覆盖到达帧点击屏蔽、隐藏/禁用项、键盘确认、回调栈顺序、lock 只改视觉。

## 尚未覆盖

原生全部 cursor 操作、选择中保存/恢复的完整状态、第三颜色用途、
特殊/空选项脚本的 native 失败路径均未完成逐项对比。载入其他场景会清理选择 sprite；
这不等于可恢复任意选择中间态。清理仅丢弃选项、临时输入和 sprite，保留 select_init
背景/颜色及回调注册，避免加载后下一次选择失去启动时初始化。text_init 重建 UI 时同样清理。
