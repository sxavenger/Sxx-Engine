# 指示
## コミットメッセージの指示
### Commit Message
コミットメッセージは以下の形式で生成すること

```text
<type>: <変更内容を簡潔に記述>
```

`<type>` の部分は山括弧を付けず、以下のいずれかの単語に置き換えること（例: `refactor:`）

```text
build: ビルドシステム・依存関係の変更
chore: その他の変更
docs: ドキュメントの追加・修正
feature: 新機能の追加
fix: バグ修正
refactor: コードのリファクタリング
test: テストの追加・修正
```

### 出力順のルール
- 複数行を出力する場合、行は **`<type>` ごとにまとめて並べる**。同じ `<type>` の行の間に、別の `<type>` の行を挟まない
- `<type>` の並び順は、上の一覧の順（アルファベット順）に固定する
  - `build` → `chore` → `docs` → `feature` → `fix` → `refactor` → `test`
- 同じ `<type>` の中では、差分に現れた順（元の記述順）を維持する
- 出力に含まれない `<type>` は飛ばしてよい

### 最重要ルール: 1行に1つの変更
- 1行のコミットメッセージには、変更を **1つだけ** 書く
- ここでいう「変更の対象」は、**クラス・構造体・名前空間直下の関数・ファイル** の単位とする
- 変更の対象が異なる場合は、必ず別の行に分ける
- 変更の種類（引数順変更、constexpr化、名前変更、追加、削除、バグ修正など）が異なる場合も、必ず別の行に分ける
- 同じ種類の変更であっても、対象（クラス・構造体・ファイル）が複数ある場合は対象ごとに別の行に分ける
- 「関連型」「関連クラス」「周辺」「など」のように、複数のクラスやファイルをまとめてぼかす表現は使わず、対象の名前を1つずつ書く

### 分割しすぎないためのルール
- 同じクラス・構造体の中で、**同じ種類の変更** が複数のメンバー（コンストラクタ、演算子、メンバー関数など）にまたがる場合は、メンバーごとに分けず **クラス名だけを使って1行にまとめる**
  - 例: ReferencePointerのコンストラクタ、代入演算子、Resetなどをすべてconstexpr化した → `refactor: ReferencePointerをconstexpr化`
- 同じクラスの中でも、変更の種類が異なる場合は別の行に分ける
  - 例: Matrix4x4のMakeAffineの引数順変更と、MakeTranslateのバグ修正 → 2行に分ける
- 1つのメンバーだけが変更された場合は、そのメンバー名を書いてよい
  - 例: `refactor: Matrix4x4のMakeAffine引数順変更`
- 「各コンストラクタ」「各演算子」のようにメンバーの種類ごとに行を分けない

### 生成手順
コミットメッセージは、以下の手順で生成すること

1. 差分（diff）を読み、変更点を洗い出す
2. 洗い出した変更点を「対象のクラス・構造体・ファイル」と「変更の種類」の組み合わせでグループ化する。同じグループに属するメンバー単位の変更は1つにまとめる
3. グループ1つにつき、1行のコミットメッセージを作る
4. 作った各行を見直し、以下の語や記号が **異なるクラス・ファイル、または異なる種類の変更** 同士をつないでいないか確認する。つないでいる場合は、その行を分割する
   - 「と」「および」「及び」「ならびに」「並びに」「や」「かつ」「、」「・」「＆」「&」「/」「+」「and」
5. 同じクラス名で始まり、同じ種類の変更を表す行が複数できていないか確認する。できている場合は、クラス名だけを使って1行にまとめる
6. 「出力順のルール」に従い、行を `<type>` の順（`build` → `chore` → `docs` → `feature` → `fix` → `refactor` → `test`）に並べ替える。同じ `<type>` の中では元の順を保つ
7. グループが1つしかない場合だけ、1行で出力する

### Format
必ず以下の形式で出力すること

```text
<type>: <変更内容>
```

### Guidelines
- 変更内容は「何を変更したか」を簡潔に記述する
- 文末に句読点を付けない
- 可能な限り具体的なクラス名、関数名、ファイル名、機能名を使用する
- 複数の変更がある場合は、コミットメッセージを複数行に分けて出力する
- 複数行で出力する場合、各行を独立したコミットメッセージとして扱う
- 各行には必ず `<type>:` を付ける
- 同じ `<type>` のコミットメッセージであっても、変更内容をまとめない
- 行の並びは「出力順のルール」に従い、`<type>` ごとにまとめる。元の記述順は同じ `<type>` の中でのみ維持する
- Markdownの箇条書き記号（`-`、`*`、`1.` など）は使用しない

### Examples
#### 例1
変更内容が以下の場合

```text
Transform構造体を実装
StaticMeshのキャッシュノードを実装
Premake5.luaのフィルタを整理
```

以下のように出力すること

```text
feature: Transform構造体を実装
feature: StaticMeshのキャッシュノードを実装
refactor: Premake5.luaのフィルタ整理
```

#### 例2
差分に「Matrix4x4のアフィン変換関数の引数順の変更」と「ReferencePointerのconstexpr化」が含まれる場合

以下のように出力すること

```text
refactor: Matrix4x4のアフィン引数順変更
refactor: ReferencePointerのconstexpr化
```

以下のように1行にまとめる出力は禁止

```text
refactor: Matrix4x4のアフィン引数順変更と関連型のconstexpr化
```

#### 例3
差分に「ReferencePointerのconstexpr化」と「WeakPointerのconstexpr化」が含まれる場合

以下のように出力すること

```text
refactor: ReferencePointerのconstexpr化
refactor: WeakPointerのconstexpr化
```

以下のように同じ種類の変更を1行にまとめる出力も禁止

```text
refactor: ReferencePointerとWeakPointerのconstexpr化
```

#### 例4
差分に以下の変更が含まれる場合

```text
copilot-instructions.mdのコミットメッセージ規則を整理
InstanceRegistryを追加
Matrix4x4のMakeAffineの引数順を変更
Matrix4x4のMakeTranslateの行列要素の配置を修正
ReferencePointerのコンストラクタ、代入演算子、比較演算子、キャスト演算子、Reset、GetAddressをconstexpr化
```

以下のように出力すること

```text
docs: copilot-instructions.mdのコミットメッセージ規則整理
feature: InstanceRegistryを追加
fix: Matrix4x4のMakeTranslate行列要素配置修正
refactor: Matrix4x4のMakeAffine引数順変更
refactor: ReferencePointerをconstexpr化
```

以下のように `<type>` の順がそろっていない出力は禁止（`refactor` の行の間に `fix` の行が挟まっている）

```text
docs: copilot-instructions.mdのコミットメッセージ規則整理
feature: InstanceRegistryを追加
refactor: Matrix4x4のMakeAffine引数順変更
fix: Matrix4x4のMakeTranslate行列要素配置修正
refactor: ReferencePointerをconstexpr化
```

以下のように同じクラス内の同じ種類の変更をメンバーごとに分ける出力も禁止

```text
refactor: ReferencePointerの各コンストラクタをconstexpr化
refactor: ReferencePointerの代入演算子をconstexpr化
refactor: ReferencePointerの比較演算子をconstexpr化
refactor: ReferencePointerのキャスト演算子をconstexpr化
refactor: ReferencePointerのResetをconstexpr化
refactor: ReferencePointerのGetAddressをconstexpr化
```

#### 禁止例
箇条書き記号を付ける出力は禁止

```text
- feature: Transform構造体を実装
- feature: StaticMeshのキャッシュノードを実装
- refactor: Premake5.luaのフィルタ整理
```

複数の変更を1行にまとめる出力は禁止

```text
feature: Transform構造体とStaticMeshのキャッシュノードを実装
```

`<type>` を山括弧ごと出力することは禁止

```text
<refactor>: Matrix4x4のアフィン引数順変更
```

## コード補間の指示
- 言語がC++の場合、C++23の機能をメインに使用する
- コメントアウト等は日本語で記述する

## GitHub Copilot Chatの指示
- Chat等の回答は日本語で行う
- コードや設計等に関する指示について、特に指定がない場合はコードの編集や追加を行わない
