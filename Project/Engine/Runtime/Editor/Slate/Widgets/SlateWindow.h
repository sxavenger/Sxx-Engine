#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Window class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Window
		: public Widget {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================


		//* widget methods *//

		Vector2f ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const override;

		void OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const override;

		int32_t OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

		//* pointer option *//

		void SetPointer(WidgetPointer pointer) { pointer_ = std::move(pointer); }

		const WidgetPointer& GetPointer() const { return pointer_; }

		//* client option *//

		//! @brief クライアント領域の大きさを設定する.
		//! @note ネイティブウィンドウのリサイズに追従させるため、毎フレーム設定してよい.
		void SetClientSize(const Vector2f& size) { clientSize_ = size; }

		const Vector2f& GetClientSize() const { return clientSize_; }

		void SetDpiScale(float scale) { dpiScale_ = scale; }

		float GetDpiScale() const { return dpiScale_; }

	private:

		//=========================================================================================
		// private members
		//=========================================================================================

		WidgetPointer pointer_ = nullptr;

		Vector2f clientSize_ = { 1280.0f, 720.0f };
		float dpiScale_      = 1.0f;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
