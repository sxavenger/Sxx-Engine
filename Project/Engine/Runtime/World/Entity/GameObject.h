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

	GameObject() : instance_(std::make_unique<Instance>()) {}
	GameObject(const std::string_view& name) : instance_(std::make_unique<Instance>(name)) {}
	~GameObject();

	//* entity option *//

	RefPtr<EntityBehaviour> GetEntity() const;

	//* operator [access] *//

	EntityBehaviour* operator->() { return GetEntity(); }
	const EntityBehaviour* operator->() const { return GetEntity(); }

	//* operator [copy] <delete> *//

	GameObject(const GameObject&)            = delete;
	GameObject& operator=(const GameObject&) = delete;

	//* operator [move] *//

	GameObject(GameObject&& other) noexcept;
	GameObject& operator=(GameObject&& other) noexcept;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<Instance> instance_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
