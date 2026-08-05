#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// EntityHandle class
////////////////////////////////////////////////////////////////////////////////////////////
class EntityHandle final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Type = uint64_t; //!< Handleの整数型.

	////////////////////////////////////////////////////////////////////////////////////////////
	// Hash structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Hash {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* operator () *//

		size_t operator()(const EntityHandle& handle) const noexcept {
			return std::hash<size_t>()(handle.GetHashCode());
		}

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	EntityHandle() noexcept = default;
	EntityHandle(uint64_t handle) noexcept : handle_(handle) {}
	EntityHandle(std::nullopt_t) noexcept : handle_(std::nullopt) {}

	//* operator [copy / move] <EntityHandle> *//

	EntityHandle(const EntityHandle&) noexcept            = default;
	EntityHandle& operator=(const EntityHandle&) noexcept = default;

	EntityHandle(EntityHandle&&) noexcept            = default;
	EntityHandle& operator=(EntityHandle&&) noexcept = default;

	//* operator [comparison] <EntityHandle> *//

	bool operator==(const EntityHandle& other) const noexcept { return handle_ == other.handle_; }
	bool operator!=(const EntityHandle& other) const noexcept { return handle_ != other.handle_; }

	//* handle option *//

	bool HasHandle() const noexcept { return handle_.has_value(); }

	Type GetHandle() const;

	size_t GetHashCode() const noexcept { return handle_.value_or(0); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<Type> handle_ = std::nullopt;

};

SXAVENGER_ENGINE_NAMESPACE_END
