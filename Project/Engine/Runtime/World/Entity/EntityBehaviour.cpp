#include "EntityBehaviour.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityBehaviour::~EntityBehaviour() {
	for (auto& [type, handle] : components_) {
		ComponentStorage::GetInstance()->Unregister(type, handle); //!< componentの登録解除
	}

	// TODO: Entityの子が存在する場合, 子のEntityを削除する.
}

RefPtr<EntityBehaviour> EntityBehaviour::GetParent() const {
	return EntityStorage::GetInstance()->GetEntity(parent_);
}
