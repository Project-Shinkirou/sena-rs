# 接口补齐记录：2026-10-23

本轮继续实现已取得原生证据的接口；未将整个 SoftPAL 引擎标记为完成。

## btn_mode（8:36）

Game.exe 0x410BA0 pop 一个整数，将非零转为 bool 写入 ctx+12956，并调用 PalButtonMode。
现已接通：0 使用按下反应，非零使用松开模式。普通按钮按 PAL 0x1023A740 的 held-hover 捕获和取消逻辑；Select 直接判断 mouse/Return/Space pull。

按下捕获与实际按钮反应分开：捕获不提前解除 ADV 等待，不完成文字揭示。真正触发后的鼠标松开事件在 engine 中从 task 和 VM 的输入里消费。滑块仍按下捕获。

新测试覆盖 pop 栈纪律、默认 push、按住/松开、移出取消、按住移入、模式切换、清理捕获和 Select 键鼠 pull。
原生确认音结束才派发反应的时延未在本轮扩展，也未做真实窗口人工点击验收。
证据：[Game](evidence/btn-mode.c)、[PAL](evidence/button-reaction-pull.c)。

## tips_get_param（2:39）

新增 selectors 12..17：返回 text_init 的原始字号、正文 x/y、姓名 x/y、mode/pitch。
依据 0x441400 写字段与 0x441BD0 读字段直接匹配；不做 1.5× 或窗口投影、不跟随后续共享字体变化、不截断为 u16。
这些 selector 不读取 record，所以 record 参数不影响结果；text_uninit 后返回 0。
测试覆盖多种逻辑分辨率、负坐标、超 u16 字号、字体修改和反初始化。

Tips producer、字形颜色和交互/完成回调链仍未补齐。

## history_get_text（14:20）

新实现见 [历史查询接口](history-query.md)。写出姓名、语音名、正文并返回脚本文字指令位置；索引 0 对应最新记录。
之前 pop 4 后只返回记录数的占位实现已替换。动态源字符串的历史冻结与原生历史镜像导入仍有已记录边界。

## 共用签名与反编译

btn_mode 和 history_get_text 新增共享签名及准确 pop 数，Tips 查询描述同步。反编译回归覆盖保留相邻调用参数与三个动态字符串目的地的顺序。

## 合并验证

`cargo test -p pal-vm --lib --test save_semantics_fixture`：98 passed + 6 passed / 4 ignored。
`cargo test -p pal-script -p pal-decompiler`：12 + 32 passed。
本轮合并验证共 **148 passed**，`git diff --check` 通过。真实窗口人工点击未包含在这些测试内。
