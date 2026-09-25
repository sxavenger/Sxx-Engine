# Cache Management
## Cacheについて
cacheは, `Assets Cache`と`Component Cache`の2種類に分けられる。

### Assets Cache
Assetsからのキャッシュデータ
Assetsに付与してある`Uuid`をキーとしてキャッシュする。

#### Cacheの更新方法
1. uuidをキーとしてキャッシュを取得する
2. キャッシュ側のaddressとAssets側のaddressを比較する
3. 一致していなければ, キャッシュを更新する
4. 一致していれば, キャッシュをそのまま利用する

### Component Cache
Componentからのキャッシュデータ
Componentの`address`をキーとしてキャッシュする。

## Cache構造体
```c++

struct CacheHash {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : bool {
		Pointer, //!< Component Cache想定
		Uuid //!< Assets Cache想定
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	Hash() noexcept = default;

	//* option *//

	std::string Serialize() const {
		switch (static_cast<Type>(id.index())) {
			case Type::Pointer:
				return std::format("{:x}", std::get<uintptr_t>(id));

			case Type::Uuid:
				return std::get<Uuid>(id).Serialize();

			default:
				return "Unknown";
		}
	}

	//* operator [assign] <Uuid> *//

	Hash(const Uuid& uuid) noexcept : id(uuid) {}
	Hash& operator=(const Uuid& uuid) noexcept { id = uuid; return *this; }

	//* operator [assign] <uintptr_t> *//

	Hash(const uintptr_t& ptr) noexcept : id(ptr) {}
	Hash& operator=(const uintptr_t& ptr) noexcept { id = ptr; return *this; }

	//* operator [hash] *//

	std::size_t operator()(const Hash& hash) const noexcept {
		return std::visit([](const auto& value) { return std::hash<std::decay_t<decltype(value)>>{}	(value); }, hash.id); //!< std::visitを使ってstd::variantの中身に応じてハッシュ値を計算
	}

	//* operator [comparison] *//

	friend bool operator==(const Hash& lhs, const Hash& rhs) noexcept {
		return lhs.id == rhs.id;
	}

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::variant<uintptr_t, Uuid> id;

};

std::unordered_map<CacheHash, std::shared_ptr<CacheData>, CacheHash> caches;

```
