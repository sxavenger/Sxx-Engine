#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"
#include "BaseAssetMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Scheduler/Common/TaskState.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>

//* c++
#include <string>
#include <memory>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseAsset(const BaseAssetMetadata& metadata) noexcept;
	virtual ~BaseAsset() noexcept = default;

	//* type option *//

	AssetType GetType() const noexcept { return type_; }

	//* uuid option *//

	const Uuid& GetUuid() const noexcept { return uuid_; }

	//* filepath option *//

	std::string GetName() const { return filepath_.stem().string(); }

	std::filesystem::path GetDirectory() const { return filepath_.parent_path(); }

	const std::filesystem::path& GetFilepath() const noexcept { return filepath_; }

	//* metadata option *//

	BaseAssetMetadata GetMetadata() const;

	//* task state option *//

	Scheduler::TaskHandle GetTaskHandle() const noexcept { return state_; }

	Scheduler::TaskState::Pointer GetStatePointer() const { return state_.pointer; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	const AssetType type_;
	const Uuid uuid_;
	std::filesystem::path filepath_;

	//* task state *//

	Scheduler::TaskState state_;

};

//-----------------------------------------------------------------------------------------
// Asset concept
//-----------------------------------------------------------------------------------------
template <typename T>
concept Asset = std::derived_from<T, BaseAsset>;

SXAVENGER_ENGINE_NAMESPACE_END
