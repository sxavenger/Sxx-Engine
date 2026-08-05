#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "../Manager/AssetStorage.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Scheduler/Common/TaskHandle.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Uuid/Uuid.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetHandle class
////////////////////////////////////////////////////////////////////////////////////////////
template <Asset T>
class AssetHandle final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetHandle() noexcept = default;

	//* handle option *//

	bool HasId() const noexcept { return uuid_.has_value(); }

	std::shared_ptr<T> Get() const;

	void Wait() const;

	std::shared_ptr<T> WaitGet() const;

	void Reload() const;

	//* operator [copy / move] <AssetHandle> *//

	AssetHandle(const AssetHandle&) noexcept            = default;
	AssetHandle& operator=(const AssetHandle&) noexcept = default;

	AssetHandle(AssetHandle&&) noexcept            = default;
	AssetHandle& operator=(AssetHandle&&) noexcept = default;

	//* operator [copy] <Uuid> *//

	AssetHandle(const Uuid& id) noexcept : uuid_(id) {}
	AssetHandle& operator=(const Uuid& id) noexcept { uuid_ = id; return *this; }

	//* operator [comparison] <Uuid> *//

	bool operator==(const Uuid& id) const noexcept { return uuid_.has_value() && uuid_.value() == id; }
	bool operator!=(const Uuid& id) const noexcept { return !uuid_.has_value() || uuid_.value() != id; }

	//* operator [comparison] <std::nullopt_t> *//

	bool operator==(std::nullopt_t) const noexcept { return !uuid_.has_value(); }
	bool operator!=(std::nullopt_t) const noexcept { return uuid_.has_value(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<Uuid> uuid_ = std::nullopt; //!< assetのid

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetHandle class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Asset T>
std::shared_ptr<T> AssetHandle<T>::Get() const {
	StreamLogger::Assert(uuid_.has_value(), "failed to get asset. asset id is empty.");
	return AssetStorage::GetInstance()->Get<T>(uuid_.value());
}

template <Asset T>
void AssetHandle<T>::Wait() const {
	if (!uuid_.has_value()) {
		StreamLogger::Warning("Assets::AssetHandle | failed to wait for asset. asset id is empty.");
		return; //!< idが空の場合は何もしない
	}


	std::shared_ptr<BaseAsset> asset = AssetStorage::GetInstance()->Get<T>(uuid_.value()); //!< assetの取得

	//!< assetのビルドが完了するまで待機
	Scheduler::TaskHandle handle = asset->GetTaskHandle();
	handle.Wait();
}

template <Asset T>
std::shared_ptr<T> AssetHandle<T>::WaitGet() const {
	Wait();
	return Get();
}

template <Asset T>
void AssetHandle<T>::Reload() const {
	if (!uuid_.has_value()) {
		StreamLogger::Warning("Assets::AssetHandle | failed to reload asset. asset id is empty.");
		return; //!< idが空の場合は何もしない
	}
	
	AssetStorage::GetInstance()->Reload<T>(uuid_.value());
}

SXAVENGER_ENGINE_NAMESPACE_END
