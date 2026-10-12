#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Spacer class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 何も描かず領域だけを占める葉のwidget.
	//! @note dockが空(全panelを失った)main windowでもstatusBarをclientの下端に固定するために使う.
	class Spacer final
		: public Widget {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Vector2f ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const override {
			return Vector2f{ 0.0f, 0.0f };
		}

		int32_t OnPaint(const PaintArguments& /*arguments*/, const Geometry& /*geometry*/, ImGuiRenderer* /*renderer*/, int32_t layer) const override {
			return layer;
		}

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
