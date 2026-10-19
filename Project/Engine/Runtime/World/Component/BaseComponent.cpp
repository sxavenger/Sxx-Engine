#include "BaseComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../Entity/EntityBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool BaseComponent::IsActive() const {
	RefPtr<EntityBehaviour> owner = owner_.Get();
	return owner->IsActive();
}
