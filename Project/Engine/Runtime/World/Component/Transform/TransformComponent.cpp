#include "TransformComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* world
#include "../../Entity/EntityBehaviour.h"
#include "../../Entity/EntityStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// [TransformComponent] TransformationMatrix structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::TransformationMatrix::Transfer(const Transformation3d& transformation) {
	world            = Transformation3d::ConvertMatrix(transformation);
	inverseTranspose = Matrix4x4f::Transpose(Transformation3d::ConvertMatrix(Transformation3d::Inverse(transformation)));
	//!< (mat^-1)tと同様. (Normalの計算に使用する)
}

TransformComponent::TransformationMatrix TransformComponent::TransformationMatrix::Identity() noexcept {
	TransformationMatrix matrix = {};
	matrix.world            = Matrix4x4f::Identity();
	matrix.inverseTranspose = Matrix4x4f::Identity();

	return matrix;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::Update() {

	//!< transformからtransformationに変換
	transformation_ = Transform3d::ConvertTransformation(transform_);

	//!< Entityの親が存在する場合, 親のTransformComponentのtransformationを掛ける.
	if (BaseComponent::GetEntity()->HasParent()) {
		RefPtr<EntityBehaviour> parent = BaseComponent::GetEntity()->GetParent();

		if (parent->HasComponent<TransformComponent>()) {
			RefPtr<TransformComponent> component = parent->GetComponent<TransformComponent>();
			transformation_ *= component->GetTransformation();
		}
	}
}
