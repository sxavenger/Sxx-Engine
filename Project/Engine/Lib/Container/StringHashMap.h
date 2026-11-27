#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <utility>
#include <stdexcept>
#include <cstddef>

////////////////////////////////////////////////////////////////////////////////////////////
// StringHashMap structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct StringHashMap final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Hash structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Hash final {
	public:

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		//!< @brief std::unordered_mapのfind()で、std::string_viewを使えるようにするための設定.
		using is_transparent = void;

		//=========================================================================================
		// public methods
		//=========================================================================================

		size_t operator()(std::string_view str) const noexcept {
			return std::hash<std::string_view>{}(str);
		}

	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	//!< @brief is_transparentを有効にするには, key_equalも透過的(std::equal_to<>)である必要がある.
	using Container      = std::unordered_map<std::string, T, Hash, std::equal_to<>>;
	using iterator       = Container::iterator;
	using const_iterator = Container::const_iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* region methods *//

	[[nodiscard]] bool Empty() const noexcept { return map.empty(); }

	std::size_t Size() const noexcept { return map.size(); }

	//* iterator methods *//

	iterator begin() noexcept { return map.begin(); }
	const_iterator begin() const noexcept { return map.begin(); }

	iterator end() noexcept { return map.end(); }
	const_iterator end() const noexcept { return map.end(); }

	const_iterator cbegin() const noexcept { return map.cbegin(); }

	const_iterator cend() const noexcept { return map.cend(); }

	//* container modifier methods *//

	template <typename... Args>
	std::pair<iterator, bool> Emplace(Args&&... args) {
		return map.emplace(std::forward<Args>(args)...);
	}

	template <typename... Args>
	iterator EmplaceHint(const_iterator hint, Args&&... args) {
		return map.emplace_hint(hint, std::forward<Args>(args)...);
	}

	iterator Erase(iterator position) { return map.erase(position); }

	iterator Erase(const_iterator position) { return map.erase(position); }

	//! @brief std::unordered_map::erase(key)は透過的検索に非対応のため, find()経由で実装する.
	std::size_t Erase(std::string_view key) {
		if (auto it = map.find(key); it != map.end()) {
			map.erase(it);
			return 1;
		}

		return 0;
	}

	void Clear() noexcept { map.clear(); }

	//* lookup methods *//

	iterator Find(std::string_view key) { return map.find(key); }
	const_iterator Find(std::string_view key) const { return map.find(key); }

	std::size_t Count(std::string_view key) const { return map.count(key); }

	bool Contains(std::string_view key) const { return map.contains(key); }

	//! @brief std::unordered_map::operator[]は透過的検索に非対応のため, find()経由で実装する.
	//!        既存keyの場合はstd::stringを生成しない.
	T& operator[](std::string_view key) {
		if (auto it = map.find(key); it != map.end()) {
			return it->second;
		}

		return map.try_emplace(std::string(key)).first->second;
	}

	//! @brief std::unordered_map::at()は透過的検索に非対応のため, find()経由で実装する.
	T& At(std::string_view key) {
		if (auto it = map.find(key); it != map.end()) {
			return it->second;
		}

		throw std::out_of_range("StringHashMap::At: key not found.");
	}

	const T& At(std::string_view key) const {
		if (auto it = map.find(key); it != map.end()) {
			return it->second;
		}

		throw std::out_of_range("StringHashMap::At: key not found.");
	}

	//=========================================================================================
	// public variables
	//=========================================================================================

	Container map;

};
