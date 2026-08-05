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
	// CompoundWidget class
	////////////////////////////////////////////////////////////////////////////////////////////
	class CompoundWidget
		: public Widget {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* widget option *//

		Vector2f ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const override;

		void OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const override;

		int32_t OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

		//* pointer option *//

		void SetPointer(WidgetPointer pointer) { pointer_ = std::move(pointer); }

		const WidgetPointer& GetPointer() const { return pointer_; }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		WidgetPointer pointer_ = nullptr;
		Margin padding_ = {};

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
