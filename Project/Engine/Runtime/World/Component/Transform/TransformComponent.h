#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Transform/Transformation3d.h>
#include <Lib/Math/Transform/Transform3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TransformationMatrix structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TransformationMatrix {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* transfer option *//

		void Transfer(const Transformation3d& transformation);

		//* constant value methods *//

		static TransformationMatrix Identity() noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4f world            = Matrix4x4f::Identity();
		Matrix4x4f inverseTranspose = Matrix4x4f::Identity();

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	TransformComponent(uint64_t handle, RefPtr<EntityBehaviour> entity) noexcept : BaseComponent(handle, entity) {}
	~TransformComponent() override = default;

	//* transform option *//

	//! @brief 行列を更新
	void Update();

	Transform3d& GetTransform() { return transform_; }
	const Transform3d& GetTransform() const { return transform_; }

	const Transformation3d& GetTransformation() const { return transformation_; }

	//* operator [access] *//

	Transform3d& operator*() { return transform_; }
	const Transform3d& operator*() const { return transform_; }

	Transform3d* operator->() { return &transform_; }
	const Transform3d* operator->() const { return &transform_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transform *//

	Transformation3d transformation_ = Transformation3d::Identity(); //!< 変換行列
	Transform3d transform_           = Transform3d::Identity();

};

SXAVENGER_ENGINE_NAMESPACE_END
