# 第二轮限时 Rust 修复（2026-10-23）

本轮仅修改有原生证据或媒体回归支持的实现，不等同于全部引擎逆向完成。

- Select：按 0x435C90 修正重叠项全部 hover、最后命中返回 reaction、点击时第一个命中取得分支；键盘游标独立，初值 -1，鼠标不改变它。load/text_init 清理选择中间态但保留初始化资源/颜色/回调/lock。
- Tips：补 0x441950 的两个参数注册，清记录但保留 count/mode/flag/colors；补齐 36..46 的共享签名及精确 pop。反编译回归实际构造连续调用，证明零参数查询和三参数 get_str 不会吃掉后续 autopan 参数。布局 producer/实际 callback 调度仍未完成。
- 电影：单帧流不能用无前帧间隔的 1ms 默认值立刻结束，按 codec cadence/container duration 保留画面；独立 MPEG/WMV 单帧样本回归。
- 语音：flags 4 的 fade 请求传入音频 backend，单声道和 MIX 成员独立淡入；Kira 淡入 envelope 与 gain/pan 分离，避免后续自动声像更新覆盖淡入。原 PAL fade 曲线尚未逐样本复原。

具体代码与测试结果以本轮最终检查为准。原生证据参见本目录专题和 evidence/。

## 最终验证

合并后的 `pal-vm --lib` 为 88 passed / 4 ignored；audio_fixture 3、movie_timing 7、mpeg2_movie 3、save_semantics_fixture 6 全通过。
`pal-script` 12 项、`pal-decompiler` 30 项通过，本轮合计 **149 passed**。`git diff --check` 通过。
真实 Koikake 只读 Select fixture 也单独运行通过。

本轮另定位 btn_mode（8:36、0x410BA0）写 ctx+12956、选择 push/pull，证据 `evidence/btn-mode.c` 已保存；完整按钮和 Select pull 模式接线尚未完成，未纳入完成声明。
