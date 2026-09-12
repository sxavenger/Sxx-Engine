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

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	TransformComponent(uint64_t handle, RefPtr<EntityBehaviour> entity) noexcept : BaseComponent(handle, entity) {}
	~TransformComponent() override = default;

	//* transform option *//

	//! @brief 行列を更新
	void Update();

	void SetPosition(const Vector3f& position) { transform_.translation = position; }

	const Vector3f& GetPosition() const { return transform_.translation; }

	void SetRotation(const Quaternionf& rotation) { transform_.rotation = rotation; }

	const Quaternionf& GetRotation() const { return transform_.rotation; }

	void SetScale(const Vector3f& scale) { transform_.scale = scale; }

	const Vector3f& GetScale() const { return transform_.scale; }

	void SetTransform(const Transform3d& transform) { transform_ = transform; }

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
