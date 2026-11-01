#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Color4.h>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (DrawList) namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui {

	////////////////////////////////////////////////////////////////////////////////////////////
	// DrawList structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DrawList {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* Line *//

		void Line(const Vector2f& p1, const Vector2f& p2, const Color4f& color, float thickness = 1.0f);

		//* Rect *//

		void Rect(const Vector2f& min, const Vector2f& max, const Color4f& color, float rounding = 0.0f, float thickness = 1.0f, ImDrawFlags flags = ImDrawFlags_None);

		void RectFilled(const Vector2f& min, const Vector2f& max, const Color4f& color, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);

		//* Circle *//

		void Circle(const Vector2f& center, float radius, const Color4f& color, int segments = 0, float thickness = 1.0f);

		void CircleFilled(const Vector2f& center, float radius, const Color4f& color, int segments = 0);

		//* Triangle *//

		void Triangle(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3, const Color4f& color, float thickness = 1.0f);

		void TriangleFilled(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3, const Color4f& color);

		//* operator [assignment] <ImDrawList*> *//

		DrawList(ImDrawList* draw_list) noexcept : drawList(draw_list) {};
		DrawList& operator=(ImDrawList* draw_list) noexcept { drawList = draw_list; return *this; }

		//* static methods *//

		static DrawList GetCurrentDrawList();

		static DrawList GetBackgroundDrawList();

		static DrawList GetForegroundDrawList();

		//=========================================================================================
		// public variables
		//=========================================================================================

		ImDrawList* drawList = nullptr; //!< ImGuiのDrawList

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
