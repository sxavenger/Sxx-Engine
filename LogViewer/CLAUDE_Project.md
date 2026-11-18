# Project概要
Sxavenger Engineで出力されたLogを閲覧しやすくするApplicationを作成してください。

## Logのフォーマット
### Fileの始め
```
Sxavenger Library File Logger
profile: Debug
main thread id: 31388
timestamp: 2026-11-18 12:55:20
```
|profile|main thread id|timestamp|
|:--:|:--:|:--:|
| 実行時のProfile | メインスレッドの番号 | 実行時の時間 |

### 通常ログ
```
<Stream> [2026-11-18 12:55:20] [Engine/Runtime/Util/Configuration/Configuration.cpp:46] [main thread] [Info] Configuration | successfully loaded config. filepath: Engine/Packages/config/Platform.toml
```
|label|timestamp|filepath:line|thread|level|message|
|:--:|:--:|:--:|:--:|:--:|:--:|
| Loggerの種類 | Logを発行した時間 | Logを発行した場所(ファイルパス) | Logが書き出されたthread | LogLevel | メッセージ

### Exception
```
================================================================================================================================

 [Exception] Sxavenger Stream Logger Exception.

[location]
 filepath: Game/Unit/SandboxUnit.cpp
 function: void __cdecl SandboxUnit::InitSandbox(void)
 line:     145

[thread]
 main thread

[timestamp]
 2026-11-18 13:00:36

[stacktrace]
 0# Sandbox!SandboxUnit::InitSandbox+0x87F | SandboxUnit.cpp:145
 1# Sandbox!`SandboxUnit::Setup'::`2'::<lambda_1>::operator()+0x2F | SandboxUnit.cpp:68
 2# Sandbox!std::invoke<`SandboxUnit::Setup'::`2'::<lambda_1> &>+0x2C | SandboxUnit.cpp:1,792
 3# Sandbox!std::_Func_impl_no_alloc<`SandboxUnit::Setup'::`2'::<lambda_1>,void>::_Do_call+0x33 | SandboxUnit.cpp:1,005
 4# Sandbox!std::_Func_class<void>::operator()+0x57 | SandboxUnit.cpp:1,057
 5# Sandbox!Sxx::Framework::Pipeline::RunPhase+0x1ED | SandboxUnit.cpp:87
 6# Sandbox!Sxx::Framework::Pipeline::Execute+0x128 | SandboxUnit.cpp:28
 7# Sandbox!Sxx::Framework::Context::Run+0xC9 | SandboxUnit.cpp:20
 ... (more 7 frames)

test exception

================================================================================================================================
```
|location|thread|timestamp|stacktrace|message|
|:--:|:--:|:--:|:--:|:--:|
| エラーが発生した箇所 | エラーが発生したthread | エラーが発生した時間 | エラーが発生までのstacktrace(8行まで) | メッセージ |

> SampleLogフォルダに通常版とエラーログ版の二つがあります
> 詳しいコードはSxavenegrEnginePreview内のLib/Loggerを参照してください

## Applicationについて
pythonで作成してください
UI等は[daisyUI components](https://daisyui.com/components/)を参考してください。

## 機能について
- label, thread, level等のフィルター機能
- filepathがクリックされたらapplication内のモナコエディターでreadonlyの該当ファイル行が見れる

## Logの特殊な操作
StackLoggerの場合, 同じログが連続行で出力されている場合見やすさを挙げるためは以下のListにまとめてください
```
<Stack> [first-timestamp ~ last-timestamp] [filepath:line] [thread] [Level] message [x message_count]
```
