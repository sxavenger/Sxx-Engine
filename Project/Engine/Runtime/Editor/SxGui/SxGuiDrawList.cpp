#include "SxGuiDrawList.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// DrawList structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxGui::DrawList::Line(const Vector2f& p1, const Vector2f& p2, const Color4f& color, float thickness) {
	if (drawList) {
		drawList->AddLine({ p1.x, p1.y }, { p2.x, p2.y }, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }), thickness);
	}
}

void SxGui::DrawList::Rect(const Vector2f& min, const Vector2f& max, const Color4f& color, float rounding, float thickness, ImDrawFlags flags) {
	if (drawList) {
		drawList->AddRect({ min.x, min.y }, { max.x, max.y }, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }), rounding, thickness, flags);
	}
}

void SxGui::DrawList::RectFilled(const Vector2f& min, const Vector2f& max, const Color4f& color, float rounding, ImDrawFlags flags) {
	if (drawList) {
		drawList->AddRectFilled({ min.x, min.y }, { max.x, max.y }, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }), rounding, flags);
	}
}

void SxGui::DrawList::Circle(const Vector2f& center, float radius, const Color4f& color, int segments, float thickness) {
	if (drawList) {
		drawList->AddCircle({ center.x, center.y }, radius, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }), segments, thickness);
	}
}

void SxGui::DrawList::CircleFilled(const Vector2f& center, float radius, const Color4f& color, int segments) {
	if (drawList) {
		drawList->AddCircleFilled({ center.x, center.y }, radius, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }), segments);
	}
}

void SxGui::DrawList::Triangle(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3, const Color4f& color, float thickness) {
	if (drawList) {
		drawList->AddTriangle({ p1.x, p1.y }, { p2.x, p2.y }, { p3.x, p3.y }, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }), thickness);
	}
}

void SxGui::DrawList::TriangleFilled(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3, const Color4f& color) {
	if (drawList) {
		drawList->AddTriangleFilled({ p1.x, p1.y }, { p2.x, p2.y }, { p3.x, p3.y }, ImGui::ColorConvertFloat4ToU32({ color.r, color.g, color.b, color.a }));
	}
}

SxGui::DrawList SxGui::DrawList::GetCurrentDrawList() {
	return ImGui::GetWindowDrawList();
}

SxGui::DrawList SxGui::DrawList::GetBackgroundDrawList() {
	return ImGui::GetBackgroundDrawList();
}

SxGui::DrawList SxGui::DrawList::GetForegroundDrawList() {
	return ImGui::GetForegroundDrawList();
}
