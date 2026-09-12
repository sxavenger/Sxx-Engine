#include "CameraComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// [CameraComponent] Perspective structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4f CameraComponent::Perspective::GetProjection() const {
	float aspect = sensor.x / sensor.y;
	float fov = 2.0f * std::atan(sensor.y / (2.0f * focal));
	return Matrix4x4f::MakePerspective(fov, aspect, nearClip, farClip);
}

////////////////////////////////////////////////////////////////////////////////////////////
// [CameraComponent] Orthographic structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4f CameraComponent::Orthographic::GetProjection() const {
	return Matrix4x4f::MakeOrthographic(-size.x / 2.0f, size.x / 2.0f, -size.y / 2.0f, size.y / 2.0f, nearClip, farClip);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

CameraComponent::Projection CameraComponent::GetProjectionType() const noexcept {
	return static_cast<Projection>(projection_.index());
}

const CameraComponent::Perspective& CameraComponent::GetPerspective() const {
	STREAM_ASSERT(GetProjectionType() == Projection::Perspective, "projection is not perspective");
	return std::get<Perspective>(projection_);
}

const CameraComponent::Orthographic& CameraComponent::GetOrthographic() const {
	STREAM_ASSERT(GetProjectionType() == Projection::Orthographic, "projection is not orthographic");
	return std::get<Orthographic>(projection_);
}

Matrix4x4f CameraComponent::GetProjectionMatrix() const {
	return std::visit([this](const auto& projection) { return projection.GetProjection(); }, projection_);
}
