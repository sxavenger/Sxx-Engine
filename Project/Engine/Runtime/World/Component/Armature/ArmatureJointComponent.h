#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureJointComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ArmatureJointComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ArmatureJointComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~ArmatureJointComponent() override = default;

	//* joint option *//

	void SetName(const std::string_view& name) { name_ = name; }

	std::string_view GetName() const { return name_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::string name_; //!< joint名

	//=========================================================================================
	// private variables
	//=========================================================================================

};

SXAVENGER_ENGINE_NAMESPACE_END
