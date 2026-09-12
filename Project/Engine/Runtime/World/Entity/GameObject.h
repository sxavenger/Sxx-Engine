#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityHandle.h"
#include "EntityBehaviour.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class
////////////////////////////////////////////////////////////////////////////////////////////
class GameObject final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Instance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Instance {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Instance();
		Instance(const std::string_view& name);

		~Instance();

		//* entity option *//

		RefPtr<EntityBehaviour> GetEntity() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		EntityHandle handle_ = {};

	};

public:


	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	GameObject();
	~GameObject();

	//* entity option *//

	RefPtr<EntityBehaviour> GetEntity() const;

	//* operator [access] *//

	EntityBehaviour* operator->() { return GetEntity(); }
	const EntityBehaviour* operator->() const { return GetEntity(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<Instance> instance_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
