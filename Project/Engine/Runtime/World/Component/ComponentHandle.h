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
// ComponentHandle class
////////////////////////////////////////////////////////////////////////////////////////////
class ComponentHandle {
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

		size_t operator()(const ComponentHandle& handle) const noexcept {
			return std::hash<size_t>()(handle.GetHashCode());
		}

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ComponentHandle() noexcept = default;
	ComponentHandle(uint64_t handle) noexcept : handle_(handle) {}

	//* operator [copy / move] <ComponentHandle> *//

	ComponentHandle(const ComponentHandle&) noexcept            = default;
	ComponentHandle& operator=(const ComponentHandle&) noexcept = default;

	ComponentHandle(ComponentHandle&&) noexcept            = default;
	ComponentHandle& operator=(ComponentHandle&&) noexcept = default;

	//* operator [comparison] <ComponentHandle> *//

	bool operator==(const ComponentHandle& other) const noexcept { return handle_ == other.handle_; }
	bool operator!=(const ComponentHandle& other) const noexcept { return handle_ != other.handle_; }

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
