#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Matrix4x4.h>

//* c++
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class CameraComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Projection enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Projection : bool {
		Perspective,
		Orthographic
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Perspective structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Perspective {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Matrix4x4f GetProjection() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f sensor = {};   //!< センサーサイズ
		float focal     = 0.0f; //!< 焦点距離
		float nearClip  = 0.0f; //!< ニアクリップ距離
		float farClip   = 0.0f; //!< ファークリップ距離

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Orthographic structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Orthographic {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Matrix4x4f GetProjection() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f size   = {}; //!< サイズ
		float nearClip  = 0.0f; //!< ニアクリップ距離
		float farClip   = 0.0f; //!< ファークリップ距離

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CameraComponent(uint64_t handle, RefPtr<EntityBehaviour> entity) noexcept : BaseComponent(handle, entity) {}
	~CameraComponent() override = default;

	//* projection option *//

	//! @brief 投影タイプを取得する
	Projection GetProjectionType() const noexcept;

	//! @brief 投影(prespective)の設定
	void SetProjection(const Perspective& perspective) { projection_ = perspective; }

	//! @brief 投影(orthographic)の設定
	void SetProjection(const Orthographic& orthographic) { projection_ = orthographic; }

	//! @brief 投影(prespective)の取得
	//! @throw 投影タイプがOrthographicの場合に発生する
	const Perspective& GetPerspective() const;

	//! @brief 投影(orthographic)の取得
	//! @throw 投影タイプがPerspectiveの場合に発生する
	const Orthographic& GetOrthographic() const;

	//! @brief 投影行列を取得する
	Matrix4x4f GetProjectionMatrix() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::variant<Perspective, Orthographic> projection_;

};

SXAVENGER_ENGINE_NAMESPACE_END
