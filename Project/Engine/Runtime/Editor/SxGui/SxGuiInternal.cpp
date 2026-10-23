#include "SxGuiInternal.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* externals
#include <imgui_internal.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	//=========================================================================================
	// constant variables
	//=========================================================================================

	static const float DRAGDROP_HOLD_TO_OPEN_TIMER = 0.70f; // Time for drag-hold to activate items accepting the ImGuiButtonFlags_PressedOnDragDropHold button behavior.
	static const float DRAG_MOUSE_THRESHOLD_FACTOR = 0.50f; // Multiplier for the default value of io.MouseDragThreshold to make DragFloat/DragInt react faster to mouse drags.

	static const ImU32 GDefaultRgbaColorMarkers[4] = {
		IM_COL32(203, 38, 0, 255), IM_COL32(103, 169, 0, 255), IM_COL32(44, 126, 237, 255), IM_COL32(140, 140, 140, 255)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ImVec math operators
	////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IMGUI_DEFINE_MATH_OPERATORS
	// ImVec2 operators
	static inline ImVec2  operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
	static inline ImVec2  operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
	static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
	static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
	static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
	static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
	static inline ImVec2  operator-(const ImVec2& lhs) { return ImVec2(-lhs.x, -lhs.y); }
	static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
	static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
	static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
	static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
	static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
	static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
	static inline bool    operator==(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
	static inline bool    operator!=(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }
	// ImVec4 operators
	static inline ImVec4  operator*(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }
	static inline ImVec4  operator/(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs); }
	static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
	static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
	static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
	static inline ImVec4  operator/(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }
	static inline ImVec4  operator-(const ImVec4& lhs) { return ImVec4(-lhs.x, -lhs.y, -lhs.z, -lhs.w); }
	static inline bool    operator==(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
	static inline bool    operator!=(const ImVec4& lhs, const ImVec4& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w; }
#endif

	////////////////////////////////////////////////////////////////////////////////////////////
	// helper methods
	////////////////////////////////////////////////////////////////////////////////////////////

	static bool CheckLabelHidden(const char* label) {
		return label == ImGui::FindRenderedTextEnd(label); //!< ラベルが非表示かどうかをチェックする（"##"で終わるか、空であるかを確認する）
	}

	static void HideLabelWidth() {
		//!< 呼び出し側が SetNextItemWidth 済みの場合はそちらを優先
		if (GImGui->NextItemData.HasFlags & ImGuiNextItemDataFlags_HasWidth) {
			return;
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui [internal] namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxGui::DragScalarNImpl(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::DragScalarN(label, data_type, p_data, components, v_speed, p_min, p_max, format, flags);
}

bool SxGui::SliderScalarNImpl(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::SliderScalarN(label, data_type, v, components, v_min, v_max, format, flags);
}

bool SxGui::InputScalarNImpl(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, flags);
}

bool SxGui::ColorEdit3Impl(const char* label, float col[3], ImGuiColorEditFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::ColorEdit3(label, col, flags);
}

bool SxGui::ColorEdit4Impl(const char* label, float col[4], ImGuiColorEditFlags flags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::ColorEdit4(label, col, flags);
}

bool SxGui::InputTextImpl(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::InputText(label, buf, buf_size, flags, callback, user_data);
}
