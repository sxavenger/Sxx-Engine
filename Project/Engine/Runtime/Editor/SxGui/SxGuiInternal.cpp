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
// SxGui [Internal] namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxGui::Internal::DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::DragScalar(label, data_type, p_data, v_speed, p_min, p_max, format, flags);
}

bool SxGui::Internal::DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::DragScalarN(label, data_type, p_data, components, v_speed, p_min, p_max, format, flags);
}

bool SxGui::Internal::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::SliderScalar(label, data_type, p_data, p_min, p_max, format, flags);
}

bool SxGui::Internal::SliderScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::SliderScalarN(label, data_type, p_data, components, p_min, p_max, format, flags);
}

bool SxGui::Internal::InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::InputScalar(label, data_type, p_data, p_step, p_step_fast, format, flags);
}

bool SxGui::Internal::InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::InputScalarN(label, data_type, p_data, components, p_step, p_step_fast, format, flags);
}

bool SxGui::Internal::ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::ColorEdit3(label, col, flags);
}

bool SxGui::Internal::ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	if (CheckLabelHidden(label)) {
		HideLabelWidth();
	}

	return ImGui::ColorEdit4(label, col, flags);
}
