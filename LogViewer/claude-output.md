# 作業ログ — SxxLogViewer

## 2026-11-18 初版作成

### 体制(CLAUDE.md「サブエージェントの立て方」に従う)
- 監督: Claude Opus 5.5(仕様策定・レビュー・配置)
- 実装: サブエージェント(Sonnet)
- テスト: 実装とは別のサブエージェント(Sonnet)。実装者の報告を信用せず独立に検証

### 仕様上の判断
- 構成: Python 標準ライブラリのみのローカル HTTP サーバ(127.0.0.1 限定)+ ブラウザ UI(daisyUI 5 / Tailwind v4 browser / Monaco 0.52.2)。
  UI ライブラリは既定で jsDelivr CDN、`tools/setup_vendor.py` でオフライン化可能。
- Stack のまとめ方: サンプル 2 本では「message まで完全一致の連続」が 0 件(`handle: N` 等が変化するため)。
  呼び出し箇所(filepath:line + thread + level)一致の連続を既定とし、最大 120 行 → 1 行に。`exact` / `none` に切替可能。
  表示は `[first ~ last] ... message [x count]`、クリックで子行を展開。
- ソース表示: ログの filepath はエンジンルートからの相対パスのため、設定で `source_root` を指定する方式。
  スタックフレーム(ファイル名のみ)は `source_root` 以下をファイル名検索。
- パーサの正規表現: 仕様案の貪欲マッチでは、メッセージ内に `[...:N]` を含む行(NGX 行)で誤抽出したため非貪欲に変更。

### 検証(この作業環境 Linux / headless Chrome で実施)
- unittest 59 件 全件成功。
- サンプル 2 本の Label / Thread / Level 件数、Exception 全フィールド(line=145, frames 8, more 7, `1,792`→1792)、
  Stack グループ(12-55-20: 135 グループ最大 x120)を grep による独立集計と照合し一致。
- パストラバーサル(`..`、エンコード、NUL、symlink、UNC、ドライブ指定)を拒否。UNC/ドライブ指定はファイルシステムに触れる前に文字列判定で拒否。
- UI: フィルタ、全解除→0 件、Stack 展開、File:Line クリック→Monaco readOnly で該当行を中央表示・ハイライト、Exception モーダル、スタックフレーム→ソース、テーマ切替、10 万行(仮想スクロール、読み込み約 1.4 秒)。

### テストで見つかり修正した点
1. Windows で UNC パスが拒否判定前に解決されうる(推論) → resolve 前に文字列で拒否
2. Label/Thread/Level 全解除でも Exception 行が残る → Label に `Exception` / `(raw)` を追加
3. まとめ方変更・再読み込みでフィルタがリセット → 同一ログなら保持
4. File:Line 列で行番号が見切れる → ファイル名:行を固定表示、ディレクトリ側を省略
5. ポート使用中でトレースバック → 日本語メッセージで終了
6. config.json の BOM で設定が消える → utf-8-sig で読み込み
7. ファイル名インデックスの除外追加と、0 件時の自動再構築
8. 通常ログ中の `====` 行を Exception と誤判定 → 次の非空行が `[Exception]` の場合のみ
9. Stack グループの時刻を `first ~ last` のフル表示に
10. 狭い画面でコードパネルがテーブルを潰す → テーブル最低 420px を確保

### 未確認
- Windows 実機での動作(Linux でのみ実行。Windows 固有のパス挙動はコードからの推論)
- CDN からの実際の読み込み(作業環境から jsDelivr に到達不可。vendor モードでのみ画面確認)

## 2026-11-18 start.bat 追加
- `Application/start.bat` を作成(実装: サブエージェント Sonnet / レビュー: 監督)。
  - `py -3` → `python` の順に、実際に `-c` を実行できるかで Python を検出(Microsoft Store のスタブを除外)。3.10 未満・未インストールは日本語で案内。
  - `%~dp0` 基準で起動するため、ダブルクリック・別ディレクトリからの実行・ログファイルのドラッグ&ドロップ(`%*` で引数を転送)に対応。
  - 異常終了時は `pause` してメッセージを表示。
  - 日本語版 cmd 向けに CP932 + CRLF で保存。括弧ブロックを使わず goto で構成し、遅延展開の問題を回避。
- README.md の「起動」節に使い方を追記。
- 未確認: Windows 実機の cmd での実行(作業環境は Linux)。

## 2026-11-18 ローカル(オフライン)動作化
- PC 上で `python tools/setup_vendor.py` を実行し、`Application/vendor/` に daisyUI 5.8.1 / Tailwind browser 4.3.5 / Monaco 0.52.2 を配置(107 ファイル・約 16MB)。
- 作業環境で同じ vendor を作り、headless Chrome で 127.0.0.1 以外への通信をすべて遮断した状態で起動 → 外部リクエスト 0 件、コンソールエラー 0 件、ログ表示と Monaco のソース表示(該当行ハイライト)を確認。
- PC 側の vendor と検証に使った vendor は全ファイルの SHA-256 が一致。
- vendor/ が存在するとサーバは自動で vendor を配信する(コード変更なし)。

## 2026-11-18 UI 修正とフォルダ選択ダイアログ
- Level 列のずれ: Debug(badge-ghost)と Info(badge-info)で箱の形・幅が違っていた。Level/Label バッジを固定幅 76px・中央揃えにし、Debug も枠と背景が見える表示に変更。Chrome で Debug/Info の left/width 一致を数値確認。
- source_root / log_dir: テキスト入力に加え「参照…」ボタンを追加。`POST /api/pick-folder` がサーバ側(=PC 上)で別プロセスの tkinter `askdirectory` を開く(使えない場合は PowerShell の FolderBrowserDialog)。Host/Origin 検証あり・同時に1つまで。
- Exception 行: 全列をまたぐ赤い行のまま、左右端を通常行と一致させた。原因はセルの padding 4px と角丸で、alert が行より内側に寄っていたこと。
  (一度は列に分解する実装をしたが、ユーザー指示「x 幅を揃えるだけでよい」により元の見た目へ戻した)
- unittest 75 件成功。
- 未確認: Windows 実機でのフォルダ選択ダイアログの表示(作業環境は Linux。ダイアログがブラウザの裏に出る可能性あり)。

## 2026-11-18 チェックボックスのデザイン修正
- 依頼「RadioButton のデザイン修正」: サイドバーの checkbox(facet と正規表現トグル)がテーマの --radius-selector で丸く描画され、ラジオボタンに見えていたのが原因と判断。
- checkbox を角 4px の四角に統一(ON は primary 塗り+白チェック)。facet は行全体クリックで切替・ホバーで行ハイライト。
- 正規表現 `.*` は検索欄と連結したトグルボタンに変更(ON で primary 表示)。挙動は従来どおり。
- 変更は web/index.html のみ。unittest 全件成功、Chrome で ON/OFF・全選択/全解除・正規表現を確認。

## 2026-11-18 facet を radio 風の表示に変更
- ユーザー指示: 丸の中をチェックマークではなく一回り小さい丸で ON/OFF を表現(daisyUI radio)。直前の四角いチェックボックス化は取り消し。
- facet の input は type="checkbox" のまま(複数選択を維持)、class を `radio radio-primary radio-sm` に。daisyUI の `.radio-sm` のサイズ指定は [type=radio] にしか効かないため、checkbox 用に 20px を指定する CSS を 1 行追加。
- 本物の daisyUI radio と computed style(サイズ・枠・内丸)を dark/light で比較し差分ゼロを確認。正規表現 `.*` トグルボタンは変更なし。

## 2026-11-18 タブを閉じたら自動終了
- 依頼: LogViewer を閉じたら start.bat も閉じる。start.bat は終了コード 0 なら自然に閉じるため、サーバ側で自動終了を実装(start.bat は変更なし)。
- 方式: ページが `GET /api/alive`(SSE)を常時接続。接続数が 0 の状態が猶予(既定 5 秒)続いたら正常終了(exit 0)。
  定期 heartbeat は Chrome の非表示タブのタイマー間引き(最大 1 分)で誤検知するため不採用。
  一度も接続が無い間は終了しない。F5・ログ切替・複数タブでは終了しない。
- オプション: `--keep-alive`(自動終了しない)、`--exit-grace 秒`。
- サーバ終了後のページには「サーバーが終了しました」の alert を表示。
- 確認: unittest 85 件成功。実プロセス+headless Chrome で、最後のタブ/ブラウザを閉じると約 2.5 秒(猶予 2 秒設定)で exit 0、F5 や 2 タブ中 1 つ閉じでは継続、--keep-alive では継続。
- 未確認: Windows 実機での動作。

## 2026-11-18 SxavengerEnginePreview/LogViewer へ移動
- `SxxLogView/Application` の中身(vendor・config.json・start.bat 含む)を `SxavengerEnginePreview/LogViewer` 直下へコピーし、全ファイル一致を確認後、移動元の Application を削除。
- 移動先で unittest 成功。ただしパーサのサンプルログテスト 10 件は `../SampleLog` を参照するため移動先ではスキップされる。
- config.json の log_dir は従来どおり `SxxLogView/SampleLog` を指している(source_root は変更なし)。
