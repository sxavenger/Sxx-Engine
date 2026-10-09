#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* math
#include "../Vector2.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Transform2d structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Transform2d {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Transform2d() noexcept = default;
	constexpr Transform2d(const Vector2f& _scale, float _rotation, const Vector2f& _translation) noexcept
		: scale(_scale), rotation(_rotation), translation(_translation) {
	};

	//* operator [copy / move] <Transform2d> *//

	constexpr Transform2d(const Transform2d&) noexcept            = default;
	constexpr Transform2d& operator=(const Transform2d&) noexcept = default;

	constexpr Transform2d(Transform2d&&) noexcept            = default;
	constexpr Transform2d& operator=(Transform2d&&) noexcept = default;

	//* constant value methods *//

	constexpr static Transform2d Identity() noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector2f scale       = Vector2f::Unit();
	float rotation       = 0.0f;
	Vector2f translation = Vector2f::Origin();
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Transform2d structure inline methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Transform2d Transform2d::Identity() noexcept {
	return { Vector2f::Unit(), 0.0f, Vector2f::Origin() };
}
