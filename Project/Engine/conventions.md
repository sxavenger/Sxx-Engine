# Sxavenger Engine コーディング規約

Sxavenger Engine (Sxx Engine) v4.0.0 / DirectX12 / C++20(C++23) / Windows

本書はエンジン層 (`Lib/`, `Runtime/`) における **命名規則とコードの書き方** を定めるもの.

---

## 1. 基本フォーマット

| 項目 | 規約 |
|---|---|
| インデント | **タブ** (スペース禁止) |
| ブレース | K&R. 開き括弧は宣言と同一行 |
| 文字コード | UTF-8 (`/utf-8`) |
| 改行コード | CRLF |
| コメント言語 | **日本語** |
| 識別子 / ログ文言 | **英語** |
| 言語規格 | C++20 |

### 1.1 縦揃え

連続する代入・初期化・メンバ宣言は `=` の位置を縦に揃える.

```cpp
desc.Type           = DescriptorAllocator::GetDescriptorHeapType(category);
desc.NumDescriptors = capacity;
desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
```

```cpp
UINT index                                     = NULL;
D3D12_CPU_DESCRIPTOR_HANDLE cpu                = D3D12_CPU_DESCRIPTOR_HANDLE{};
std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> gpu = std::nullopt;
```

```cpp
Descriptor(const Descriptor&)            = delete;
Descriptor& operator=(const Descriptor&) = delete;
```

### 1.2 空行

- アクセス指定子 (`public:` / `private:` / `protected:`) の直後は1行空ける
- セクションバナーの前後は1行空ける
- 小見出し `//* ... *//` の前後は1行空ける
- クラスの閉じ括弧 `};` の直前は1行空ける

### 1.3 引数の折り返し

引数が多い場合は改行して整形する.

```cpp
void Init(
	const Device& device,
	DescriptorCategory category, UINT capacity
);
```

### 1.4 switch

`case` は1段インデント. 単純な写像は `return` を縦に揃えて1行で書く.

```cpp
constexpr DXGI_FORMAT ConvertToSRGBFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case DXGI_FORMAT_BC1_UNORM:      return DXGI_FORMAT_BC1_UNORM_SRGB;
		default:                         return format;
	}
}
```

処理を伴う場合は `case` ごとに改行し, 網羅漏れは例外で落とす.

```cpp
switch (category) {
	case DescriptorCategory::RTV:
	case DescriptorCategory::DSV:
		return false;

	case DescriptorCategory::SRV_CBV_UAV:
		return true;

	default:
		StreamLogger::Exception("category is not a valid value.");
}
```

---

## 2. ファイル

### 2.1 拡張子

| 拡張子 | 用途 |
|---|---|
| `.h` | クラス・構造体宣言 (テンプレート実装を含む) |
| `.cpp` | 実装 |
| `.hpp` | マクロ定義のみ / 実装を持たないヘッダ (`Foundation.hpp`, `CXXAttribute.hpp`) |
| `.hlsli` | HLSL 共通ヘッダ |
| `.hlsl` | HLSL 実体 (`<Name>.<stage>.hlsl` 形式) |

- インクルードガードは **`#pragma once`**
- ファイル名は主となる型名と一致させる (`DescriptorAllocator.h` ⇔ `class DescriptorAllocator`)

### 2.2 ファイル名の接頭 / 接尾

| 形式 | 用途 | 例 |
|---|---|---|
| `Base*` | 継承前提の基底クラス | `BaseAsset`, `BaseComponent`, `BaseScene` |
| `I*` | 純粋インターフェイス | `IUnit` |
| `*Util.h` | enum・定数・自由関数の集約 | `GraphicsUtil.h`, `AssetUtil.h`, `TomlUtil.h` |
| `*Traits.h` | 型特性・静的ディスパッチ | `AssetTraits.h`, `TextureTraits.h` |
| `*Metadata.h` | メタデータ構造体 | `BaseAssetMetadata.h` |
| `*Handle.h` | ハンドル型 | `AssetHandle.h`, `ComponentHandle.h` |
| `*Storage.h` | 実体の一元管理 (シングルトン) | `AssetStorage.h`, `EntityStorage.h` |

### 2.3 配置

```
Lib/        エンジン非依存の汎用ライブラリ. Runtime/ に依存してはならない
Runtime/    エンジン層. namespace Sxx::<Category>
Externals/  外部ライブラリ. 改変しない
Packages/   config(.toml), shader(.hlsl), fonts, textures
```

---

## 3. 命名規則

### 3.1 一覧

| 対象 | 規則 | 例 |
|---|---|---|
| class / struct | PascalCase | `DescriptorAllocator`, `PaintArguments` |
| enum class | PascalCase | `DescriptorCategory`, `ShaderVisibility` |
| enum 列挙子 | PascalCase (略語は大文字可) | `Direct`, `TriangleStrip`, `RTV`, `SRV_CBV_UAV` |
| メソッド / 関数 | PascalCase | `Allocate()`, `GetCPUHandle()` |
| ローカル変数 / 引数 | camelCase | `descriptorHeap`, `entryPoint` |
| private / protected メンバ変数 | camelCase + **`_`** | `handleOffset_`, `visibility_`, `mutex_` |
| public メンバ変数 (POD 構造体) | camelCase (`_` なし) | `index`, `cpu`, `deltaTime`, `enable` |
| 定数 (`constexpr` / `static const`) | **`k`** + PascalCase | `kFrameCount`, `kConfigPath`, `kPi`, `kRequireShaderModel` |
| concept | PascalCase | `Unit`, `Component`, `Asset`, `Concept::Integral` |
| マクロ | SCREAMING_SNAKE_CASE | `SXAVENGER_ENGINE_NAMESPACE_BEGIN`, `NODISCARD` |
| namespace | PascalCase | `Sxx`, `Graphics`, `Slate`, `Concept` |
| テンプレート引数 | `T`, `U` | `template <typename T>` |
| テンプレート引数 (内部実装) | `_` + PascalCase | `_Bit`, `_Mask`, `_Enum` |
| 型エイリアス | PascalCase | `RefPtr`, `WidgetPointer`, `ArrangedChildren` |

### 3.2 数値型エイリアスの接尾

`Lib/Math` の型は要素型を接尾で表す.

| 接尾 | 型 | 例 |
|---|---|---|
| `f` | `float` | `Vector3f`, `Color4f`, `Matrix4x4f` |
| `d` | `double` | `Vector3d`, `Quaterniond` |
| `i` | `std::int32_t` | `Vector2i`, `Color3i` |
| `ui` | `std::uint32_t` | `Vector3ui`, `Color4ui` |
| `sz` | `std::size_t` | `Vector2sz` |

### 3.3 メソッド名の動詞

| 接頭 | 用途 |
|---|---|
| `Init` / `Term` | 明示的な初期化 / 終了. コンストラクタで重い初期化をしない |
| `Get` / `Set` | アクセサ |
| `Is` / `Has` / `Contains` | bool を返す状態問い合わせ |
| `Check` | static な判定ヘルパ |
| `Create` | static な生成ヘルパ |
| `ConvertTo` / `Parse` / `Cast` | 変換 |
| `Reset` / `Free` / `Clear` | 解放・初期状態化 |
| `Begin` / `End` | 範囲処理 (`BeginFrame`) |
| `On*` | 仮想フック (`OnPaint`, `OnArrangeChildren`) |

### 3.4 その他

- コンストラクタ引数がメンバ名と衝突する場合のみ先頭 `_` を許可する

```cpp
constexpr Vector3(T _x, T _y, T _z) noexcept : x(_x), y(_y), z(_z) {}
```

- 略語は慣用に従い大文字のまま使う (`GetCPUHandle`, `GetGPUHandle`, `Uuid`, `RTV`)
- 未実装は `// TODO: serializationの実装を追加する` と記す

---

## 4. namespace

- ルートは `Sxx`. `Runtime/` は機能単位でカテゴリ名前空間を切る
  (`Graphics` / `Assets` / `World` / `Framework` / `Editor` / `Scheduler` / `Platform`)
- `namespace` を直接書かず, `Runtime/Foundation.hpp` のマクロを使う

**ヘッダ**

```cpp
////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)   //!< カテゴリなしは SXAVENGER_ENGINE_NAMESPACE_BEGIN

...

SXAVENGER_ENGINE_NAMESPACE_END
```

**ソース** — 自ヘッダの直後に `using` を書く.

```cpp
#include "DescriptorAllocator.h"
SXAVENGER_ENGINE_USING_(Graphics)
```

- `Lib/` は namespace を付けない (`Concept` のような小さな機能名前空間は例外)
- サブ名前空間 (`Sxx::Editor::Slate`) は `namespace Slate { ... }` をネストし, 中身を1段インデントする

---

## 5. include

ファイル先頭に include バナーを置き, **グループ順を固定** する.

```cpp
//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "Device.h"

//* graphics [core]
#include "Core/Device.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Logger/StreamLogger.h>

//* DirectX12
#include <d3d12.h>

//* c++
#include <queue>
```

| 順 | グループタグ | 記法 |
|---|---|---|
| 1 | `//* <自モジュール名>` | `"..."` 相対パス |
| 2 | `//* engine` | `<Runtime/...>` |
| 3 | `//* lib` | `<Lib/...>` |
| 4 | `//* DirectX12` / 外部ライブラリ名 | `<...>` |
| 5 | `//* c++` | 標準ライブラリ. 必ず最後 |

- 同一モジュール内は相対パス, 他モジュールは **プロジェクトルート起点の `<>`**
- サブディレクトリを示すときは `//* graphics [core]` のように `[ ]` を付ける
- `.cpp` は自ヘッダを最初にインクルードし, その下に改めて include バナーを置く
- 循環参照は前方宣言で解消する

```cpp
//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DescriptorAllocator;
```

---

## 6. コメント / バナー

### 6.1 バナーの種類

| バナー | 文字数 | 用途 |
|---|---|---|
| `////...` | `/` × 92 | 型宣言 / namespace / ファイル区分 |
| `//---...` | `//` + `-` × 89 | include, forward, using, define などのブロック |
| `//===...` | `//` + `=` × 89 | クラス内セクション (インデント付き) |

### 6.2 型宣言バナー

```cpp
////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorAllocator class
////////////////////////////////////////////////////////////////////////////////////////////
class DescriptorAllocator final {
```

種別に応じて末尾を書き分ける.

- `// Xxx class`
- `// Xxx structure`
- `// Xxx enum class`
- `// Xxx namespace`
- `// Xxx class methods` (`.cpp` 側)
- `// [Outer] Inner structure methods` (`.cpp` 側のネスト型)
- `// Xxx class template methods` (ヘッダ末尾のテンプレート実装)

### 6.3 セクションバナー

```cpp
	//=========================================================================================
	// public methods
	//=========================================================================================
```

使用する見出しは以下に固定する.

- `public methods` / `public variables`
- `protected methods` / `protected variables`
- `private methods` / `private variables`

### 6.4 小見出し `//* ... *//`

メンバを役割ごとにグルーピングする. 既存の語彙に合わせること.

```
//* constructor / destructor *//     //* singleton *//
//* constructor *//                  //* settings *//
//* <name> option *//                //* parameter *//
//* <name> helper methods *//        //* runtime state *//
//* DirectX12 *//                    //* thread *//
//* operator [copy] <T> *//          //* operator [move] <T> *//
//* operator [comparison] <T> *//    //* operator [access] *//
//* operator [binary] <T> *//        //* operator [cast] <T> *//
```

### 6.5 ドキュメントコメント

宣言に対する説明は Doxygen 風の `//!` を使う.

```cpp
//! @brief デスクリプタのインデックスを取得する.
//! @throw デスクリプタが無効な場合.
UINT GetIndex() const;

//! @brief 子の配置（トップダウン）. 子を持つウィジェットのみ override.
//! @note renderer は文字幅の計測に使う.
virtual void OnArrangeChildren(const Geometry& geometry, ArrangedChildren& children) const {}
```

行内・行末の補足は `//!<` を使う.

```cpp
SIZE_T handleOffset_ = 0; //!< デスクリプタヒープ内の割り当てるハンドルのオフセット.

allocator_.Capacity(capacity); //!< allocatorの初期化
```

- 文末は句点 `.` で締める
- 通常の `//` はコード内の一時的な補足のみに使う

---

## 7. クラスの記述順序

```cpp
class Foo final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
		...
	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Storage = std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Foo() noexcept = default;
	~Foo();

	//* foo option *//

	void Init(const Configuration& config);

	NODISCARD Descriptor Allocate();

	uint32_t GetCapacity() const { return allocator_.GetCapacity(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	//* parameter *//

	DescriptorCategory category_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initialize helper methods *//

	static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(RefPtr<ID3D12Device> device, UINT capacity);

};
```

**順序ルール**

1. ネストした型 / `using` を先頭の `public:` ブロックに置き, 改めて `public:` を書いて本体を続ける
2. **public は methods → variables**
3. **private / protected は variables → methods**
4. アクセス指定子はインデントしない. 中身は1段インデント

---

## 8. 宣言の書き方

### 8.1 特殊メンバ

- 継承しないクラスには **`final`** を付ける
- 既定の特殊メンバは `= default`, 可能なら `noexcept` を付ける

```cpp
DescriptorAllocator() noexcept = default;
virtual ~IUnit() noexcept      = default;
```

- コピー禁止型はコピーを明示的に `= delete` し, 小見出しで意図を示す

```cpp
	//* operator [copy] <Descriptor> (delete) *//

	Descriptor(const Descriptor&)            = delete;
	Descriptor& operator=(const Descriptor&) = delete;

	//* operator [move] <Descriptor> *//

	Descriptor(Descriptor&& other) noexcept;
	Descriptor& operator=(Descriptor&& other) noexcept;
```

### 8.2 属性マクロ

素の `[[...]]` を直接書かず, `Lib/CXXAttribute.hpp` のマクロを使う.

```cpp
NODISCARD Descriptor Allocate();
NORETURN static void Exception(const std::string_view& message);
DEPRECATED("Use GPU-side resource management instead")
static Graphics::Resource UploadResourceData(...);
```

利用可能: `NODISCARD` / `NORETURN` / `MAYBE_UNUSED` / `DEPRECATED(msg)` / `LIKELY` / `UNLIKELY`

### 8.3 インライン定義

- 1行で済むアクセサ・演算子はヘッダ内で定義してよい

```cpp
uint32_t GetCapacity() const { return allocator_.GetCapacity(); }
```

- 処理を伴うものは `.cpp` に置く
- テンプレート実装はヘッダ末尾の `// Xxx class template methods` バナー以下にまとめ, `inline` を付ける

### 8.4 static メンバ

- 静的クラスのメンバ変数は `static inline` で宣言する

```cpp
	static inline Device device_;
	static inline std::mutex mutex_ = {};
```

- クラス内から static メンバを呼ぶ場合も **クラス名で修飾する**

```cpp
descriptorHeap_ = DescriptorAllocator::CreateDescriptorHeap(device.GetDevice(), category, capacity);
```

- シングルトンは `//* singleton *//` 節に `static GetInstance()` を置く

---

## 9. 型の選択

### 9.1 ポインタ

| 用途 | 型 |
|---|---|
| 所有 (共有) | `std::shared_ptr<T>` |
| 所有 (単独) | `std::unique_ptr<T>` |
| **非所有参照** | **`RefPtr<T>`** (`Lib/Pointer/ReferencePointer.h`) |
| COM オブジェクト | `ComPtr<T>` (`Runtime/Core/Utility/ComPtr.h`) |

生ポインタをメンバに保持しない. `RefPtr<T>` は null 参照時に assert が働く.

### 9.2 enum

`enum` は禁止. **`enum class` + 基底型明示**.

```cpp
////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorCategory enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class DescriptorCategory : uint8_t {
	RTV,         //!< RenderTargetView
	DSV,         //!< DepthStencilView
	SRV_CBV_UAV, //!< ShaderResourceView, ConstantBufferView, UnorderedAccessView
};
```

- DirectX の定数はエンジン側 enum にラップし, 短い名前を与える

```cpp
enum class SampleMode : uint32_t {
	Wrap   = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	Clamp  = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
};
```

- 名前 / 要素数 / 整数変換は `EnumUtil<T>` を使う (`GetName()`, `GetCount()`, `Cast()`)
- ビットフラグは `FlagMask` / `FlagEnum` (`Lib/Flag/Flag.h`) を使う

### 9.3 テンプレート制約

テンプレート引数は必ず concept で制約する.

```cpp
//-----------------------------------------------------------------------------------------
// Asset concept
//-----------------------------------------------------------------------------------------
template <typename T>
concept Asset = std::derived_from<T, BaseAsset>;
```

```cpp
template <Asset T>
Uuid Import(const std::filesystem::path& filepath);

template <std::integral T>
class IndexAllocator { ... };

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
static void Info(std::format_string<Args...> format, Args&&... args);
```

基底クラス定義の直後に, 対応する concept を `//---` バナー付きで定義する.

---

## 10. ログ / アサーション

すべて `StreamLogger` (`Lib/Logger/StreamLogger.h`) 経由. `printf` / `std::cout` / `assert` は使わない.

| API | 用途 |
|---|---|
| `Debug` | 詳細トレース |
| `Info` | 初期化・終了などの通常イベント |
| `Warning` | 継続可能な異常 |
| `Error` / `Critical` | 機能不全 |
| `Exception` | 復帰不能 (`NORETURN`) |
| `Assert(expr, msg)` | 事前条件・不変条件の検証 |

### 10.1 メッセージ書式

```
"<Namespace>::<Class> | <小文字英語の事象>. <key>: {}, <key>: {}"
```

```cpp
StreamLogger::Info("Graphics::DescriptorAllocator<{}> | initialization complete. capacity: {}", category, capacity);
StreamLogger::Warning("Assets::AssetStorage | asset already imported. filepath: {}, uuid: {}", filepath.generic_string(), metadata.uuid.Serialize());
StreamLogger::Assert(current_ < capacity_, std::format("index allocator capacity over. current: {}, capacity: {}.", current_, capacity_));
```

- 可変長引数はフォーマット版オーバーロードを使う (`std::format` 互換)
- COM の `HRESULT` は `ComPtrUtil::Assert(hr, L"descriptor heap create failed.")`
- D3D12 オブジェクトには `SetName()` でデバッグ名を付ける
- `Init` / `Term` (デストラクタ) には必ずログを残す

---

## 11. 警告の抑制

抑制はピンポイントで行い, 公式ドキュメントの URL をコメントする.

```cpp
union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
	struct {
		T x, y, z;
	};

	std::array<T, 3> data;
#pragma warning(pop)
};
```

---

## 12. 設定クラス (Configuration)

設定を持つクラスは内部に `Settings` 構造体を定義する定型に従う.

```cpp
	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Parse(const toml::table& config);

		static Settings ParseFromConfig(const Configuration& config);

		static void Log(const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.DebugLayer" }; //!< 設定のパス.

		bool enable = false; //!< デバッグレイヤーを有効にするか.

	};
```

- 値の取得は `TomlReader<T>::Find(config, "key", member)`
- `Init()` の冒頭で設定を読み, ログを出す

```cpp
settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
Settings::Log(settings_); //!< 設定のログ.
```

- 設定が存在しない場合は `Warning` を出して既定値で継続する
- toml は `Packages/config/*.toml`. `[Category.Class]` を基本設定, `[Category.Class.Debug]` を構成別上書きとし, Debug ブロックは1段インデントする

---

## 13. HLSL

- 共通宣言は `.hlsli` (`#pragma once` 必須), 実体は `<Name>.<stage>.hlsl` (`Basic.vs.hlsl`, `Basic.ps.hlsl`)
- エントリポイント名は `main`
- 入出力構造体は `PSInput` / `PSOutput` のように「ステージ + In/Out」
- 定数は C++ と同じく `k` プレフィックス

```hlsl
static const float4 kTriangle[3] = {
	float4(0.0f, 0.5f, 0.0f, 1.0f),
	float4(0.5f, -0.5f, 0.0f, 1.0f),
	float4(-0.5f, -0.5f, 0.0f, 1.0f)
};

PSInput main(uint vertexId : SV_VertexID) {
	PSInput output;
	output.position = kTriangle[vertexId];
	output.color    = kColors[vertexId];
	return output;
}
```

- インデントはタブ, 代入の縦揃えも C++ と同じ

---

## 14. テンプレート (雛形)

### 14.1 ヘッダ (`Xxx.h`)

```cpp
#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Foo;

////////////////////////////////////////////////////////////////////////////////////////////
// Bar class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief barの説明.
class Bar final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Bar() noexcept = default;
	~Bar();

	//* bar option *//

	void Init(const Configuration& config);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	RefPtr<Foo> foo_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* bar helper methods *//

	static bool CheckState();

};

SXAVENGER_ENGINE_NAMESPACE_END
```

### 14.2 ソース (`Xxx.cpp`)

```cpp
#include "Bar.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Bar class methods
////////////////////////////////////////////////////////////////////////////////////////////

Bar::~Bar() {
	StreamLogger::Info("Graphics::Bar | terminate.");
}

void Bar::Init(const Configuration& config) {

	//!< 初期化処理

	StreamLogger::Info("Graphics::Bar | initialization complete.");
}
```
