#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* internal
#include "SxGuiInternal.h"

//* engine
#include <Runtime/Foundation.hpp>

//* imgui
#include <imgui.h>

//* c++
#include <string>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui {

	//-----------------------------------------------------------------------------------------
	// [DragScalar]
	//-----------------------------------------------------------------------------------------

	template <ScalarConcept T, int32_t Component>
	bool DragScalarN(const std::string_view& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalarConcept T>
	bool DragScalar(const std::string_view& label, T* v, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalarConcept T, int32_t Component>
	bool DragVectorN(const std::string_view& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//-----------------------------------------------------------------------------------------
	// [SliderScalar]
	//-----------------------------------------------------------------------------------------

	template <ScalarConcept T, int32_t Component>
	bool SliderScalarN(const std::string_view& label, T v[Component], const T v_min, const T v_max, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <ScalarConcept T>
	bool SliderScalar(const std::string_view& label, T* v, const T v_min, const T v_max, const char* format = NULL, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//-----------------------------------------------------------------------------------------
	// [InputScalar]
	//-----------------------------------------------------------------------------------------

	template <ScalarConcept T, int32_t Component>
	bool InputScalarN(const std::string_view& label, T v[Component], const T p_step, const std::optional<T>& p_step_fast = std::nullopt, const char* format = NULL, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	template <ScalarConcept T>
	bool InputScalar(const std::string_view& label, T* v, const T p_step, const std::optional<T>& p_step_fast = std::nullopt, const char* format = NULL, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	//-----------------------------------------------------------------------------------------
	// [ColorEdit]
	//-----------------------------------------------------------------------------------------

	bool ColorEdit3(const std::string_view& label, float col[3], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);

	bool ColorEdit4(const std::string_view& label, float col[4], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);

	//-----------------------------------------------------------------------------------------
	// [Button]
	//-----------------------------------------------------------------------------------------

	bool ButtonStretch(const std::string_view& label);

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::DragScalarN(const std::string_view& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	const std::pair<T, T> range = { v_min.value_or(std::numeric_limits<T>::lowest()), v_max.value_or(std::numeric_limits<T>::max()) };
	return SxGui::DragScalarNImpl(label.data(), SxGui::GetImGuiDataType<T>(), v, Component, v_speed, &range.first, &range.second, format, flags);;
}

template <SxGui::ScalarConcept T>
bool SxGui::DragScalar(const std::string_view& label, T* v, float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::DragScalarN<T, 1>(label.data(), v, v_speed, v_min, v_max, format, flags);
}

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::DragVectorN(const std::string_view& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::DragScalarN<T, Component>(label.data(), v, v_speed, v_min, v_max, format, flags | ImGuiSliderFlags_ColorMarkers);
}

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::SliderScalarN(const std::string_view& label, T v[Component], const T v_min, const T v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::SliderScalarNImpl(label.data(), SxGui::GetImGuiDataType<T>(), v, Component, &v_min, &v_max, format, flags);
}

template <SxGui::ScalarConcept T>
bool SxGui::SliderScalar(const std::string_view& label, T* v, const T v_min, const T v_max, const char* format, ImGuiSliderFlags flags) {
	return SxGui::SliderScalarN<T, 1>(label.data(), v, v_min, v_max, format, flags);
}

template <SxGui::ScalarConcept T, int32_t Component>
bool SxGui::InputScalarN(const std::string_view& label, T v[Component], const T p_step, const std::optional<T>& p_step_fast, const char* format, ImGuiInputTextFlags flags) {
	return SxGui::InputScalarNImpl(label.data(), SxGui::GetImGuiDataType<T>(), v, Component, &p_step, p_step_fast ? &p_step_fast.value() : nullptr, format, flags);
}

template <SxGui::ScalarConcept T>
bool SxGui::InputScalar(const std::string_view& label, T* v, const T p_step, const std::optional<T>& p_step_fast, const char* format, ImGuiInputTextFlags flags) {
	return SxGui::InputScalarN<T, 1>(label.data(), v, p_step, p_step_fast, format, flags);
}

SXAVENGER_ENGINE_NAMESPACE_END
