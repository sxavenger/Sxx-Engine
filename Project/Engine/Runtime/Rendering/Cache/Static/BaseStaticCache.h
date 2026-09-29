#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <concepts>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseStaticCache class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseStaticCache {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseStaticCache() noexcept          = default;
	virtual ~BaseStaticCache() noexcept = default;

	//! @brief chaceが有効かどうかを取得する.
	bool HasCache() const { return address_.has_value(); }

	//! @brief キャッシュされたアドレスを取得する.
	uintptr_t GetAddress() const { return address_.value_or(NULL); }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::optional<uintptr_t> address_ = std::nullopt;

};

//-----------------------------------------------------------------------------------------
// concepts
//-----------------------------------------------------------------------------------------
template <typename T>
concept StaticCache = std::derived_from<T, BaseStaticCache>;

SXAVENGER_ENGINE_NAMESPACE_END
