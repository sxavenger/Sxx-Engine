# SxxLogViewer

Sxavenger Engine が出力するログ (`Sxavenger Library File Logger`) を見やすく表示するローカル用ビューアです。
Python 標準ライブラリのみで動作します (Python 3.10 以上 / Windows 想定、Linux・macOS でも可)。

## 起動

```
python sxxlogview.py [LOGFILE] [--source-root DIR] [--log-dir DIR] [--port N] [--no-browser]
```

| 引数 | 内容 |
|---|---|
| `LOGFILE` | 起動時に開くログ。親ディレクトリが `log_dir` として保存されます |
| `--source-root DIR` | エンジンのリポジトリルート。ログ内の `Engine/Runtime/...` はここからの相対パス |
| `--log-dir DIR` | ログのあるディレクトリ (既定: この `Application` の親にある `SampleLog`) |
| `--port N` | 待ち受けポート (既定: 空きポートを自動選択) |
| `--no-browser` | ブラウザを自動で開かない |
| `--keep-alive` | タブを閉じても終了しない (既定はタブが全部閉じたら自動終了) |
| `--exit-grace 秒` | 全タブが閉じてから終了するまでの猶予 (既定 5) |

Windows では `start.bat` でも起動できます (`py -3` / `python` を自動検出、Python 3.10 未満や未インストールなら案内を表示)。

- ダブルクリックで起動。別のカレントディレクトリからの実行も可。
- ログファイルを `start.bat` にドラッグ&ドロップすると、そのログを開いて起動します。
- コマンドラインでは引数がそのまま渡されます: `start.bat C:\Logs\a.log --port 8080 --no-browser`
- 異常終了時は `pause` でメッセージを読めます。

`127.0.0.1` のみで待ち受けます (外部からは接続できません)。`Ctrl+C` で終了。ブラウザのタブをすべて閉じると、猶予(既定 5 秒、`--exit-grace 秒` で変更)後に自動で終了します
(F5 での再読み込みや複数タブの片方を閉じただけでは終了しません)。自動終了を止めるには `--keep-alive` を付けます。
ブラウザへのドラッグ&ドロップ、または「ファイルを開く」で任意のログも読めます (上限 100MB)。

## 設定 (`config.json`)

`Application/config.json` に自動保存されます (Git 管理外想定)。画面右上の「設定」からも変更できます。

```json
{ "source_root": "C:\\Dev\\SxavengerEngine", "log_dir": "C:\\Dev\\SxavengerEngine\\Logs", "group_mode": "callsite", "theme": "dark" }
```

- `source_root`: ソース表示の基準ディレクトリ。**この配下のファイルだけ**開けます (`..`・絶対パス・シンボリックリンクで外へ出るものは拒否)。
  `.git` / `node_modules` / `build` / `bin` / `obj` / `x64` などはファイル名検索の対象外です。5MB を超えるファイルは開けません。
- 設定画面の入力欄の右にある「参照…」ボタンで、この PC 上にネイティブのフォルダ選択ダイアログ(tkinter、使えなければ Windows の PowerShell)を開いて `source_root` / `log_dir` を選べます。
  選んだパスは入力欄に入るだけなので、「保存」で確定してください。ダイアログを開けない環境ではテキストで入力します。
- `group_mode`: Stack ログのまとめ方。`callsite` (既定: filepath・line・thread・level が同じ連続行) / `exact` (message も同じ) / `none`
- `theme`: `dark` / `light`

## 使い方

- 左サイドバー: Label / Thread / Level のチェックボックス (件数付き)、`message` と `filepath` の検索 (`.*` で正規表現)、Stack のまとめ方。
- Stack グループ行 (`x120` バッジ) をクリックで子行を展開/折り畳み。
- 行をクリックで詳細。Exception 行(赤)クリックで location / function / thread / stacktrace / message を表示。ナビバーの「Exception」で該当行へジャンプ。
- `File:Line` や stacktrace のフレームをクリックすると、右パネルの Monaco エディタ (読み取り専用) で該当行を中央表示・ハイライトします。
  スタックフレームのようにファイル名だけの場合は `source_root` 以下から検索し、複数あれば選択できます。
- キーボード: `Ctrl+F` 検索欄 / `Esc` モーダル・右パネルを閉じる。

## オフライン化 (vendor)

既定では UI ライブラリ (daisyUI 5 / Tailwind CSS v4 browser / Monaco 0.52.2) を jsDelivr CDN から読み込みます。
ネットワークに出られない環境では、次のコマンドで `Application/vendor/` に取得してください (標準ライブラリのみ、npm 不要)。

```
python tools/setup_vendor.py          # --force で再取得
```

`vendor/` があるとサーバが自動的にそちらを配信します (ファイル単位で CDN と切り替え)。設定画面の下部に現在のモード (CDN / vendor) が出ます。
取得元は `registry.npmjs.org` で、tarball の integrity (sha512) を検証してから `min/vs` 等の必要ファイルだけを展開します。
Monaco のバージョンは `sxxlog/server.py` の `MONACO_VERSION` と `tools/setup_vendor.py` で一致させています。

## テスト

```
python -m unittest discover -s tests
```

## 構成

```
sxxlogview.py        エントリポイント
sxxlog/parser.py     パーサ (UI 非依存) と Stack グループ化
sxxlog/sources.py    ソース解決 (source_root 内に限定)
sxxlog/server.py     http.server ベースのサーバと JSON API
web/index.html       フロントエンド (1 ファイル)
tools/setup_vendor.py  オフライン用ライブラリ取得
tests/               unittest
```

### API (すべて JSON)

`GET/POST /api/config` ・ `GET /api/logs` ・ `GET /api/log?name=&group=` ・ `POST /api/parse?group=` ・ `GET /api/source?path=` ・ `GET /api/find?name=` ・ `POST /api/pick-folder` ・ `GET /api/alive` (SSE。タブの生存確認用)
(`Host` / `Origin` が 127.0.0.1 / localhost 以外のリクエストは拒否します)
