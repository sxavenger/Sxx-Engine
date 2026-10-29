#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGuiTraits.h"
#include "SxGuiInternal.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Descriptor/Descriptor.h>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Color3.h>
#include <Lib/Math/Color4.h>

//* c++
#include <string_view>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (Basic) namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui {

	////////////////////////////////////////////////////////////////////////////////////////////
	// layout methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------------------------
	// [ContentRegion]
	//-----------------------------------------------------------------------------------------

	Vector2f GetContentRegionAvail();

	//-----------------------------------------------------------------------------------------
	// [CursorPos]
	//-----------------------------------------------------------------------------------------

	Vector2f GetCursorPos();

	void SetCursorPos(const Vector2f& pos);

	//-----------------------------------------------------------------------------------------
	// [TextCalc]
	//-----------------------------------------------------------------------------------------

	Vector2f CalcTextSize(const std::string_view& text, bool hide_text_after_double_hash = true, float wrap_width = -1.0f);

	////////////////////////////////////////////////////////////////////////////////////////////
	// widget methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------------------------
	// [DragScalar]
	//-----------------------------------------------------------------------------------------

	template <Traits::Scalar T>
	bool DragScalar(const std::string_view& label, T* v, float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const std::string_view& format = {}, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <Traits::Scalar T, int32_t Component>
	bool DragScalarN(const std::string_view& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const std::string_view& format = {}, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <Traits::Scalar T, int32_t Component>
	bool DragVectorN(const std::string_view& label, T v[Component], float v_speed = 1.0f, const std::optional<T>& v_min = std::nullopt, const std::optional<T>& v_max = std::nullopt, const std::string_view& format = {}, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//-----------------------------------------------------------------------------------------
	// [SliderScalar]
	//-----------------------------------------------------------------------------------------

	template <Traits::Scalar T>
	bool SliderScalar(const std::string_view& label, T* v, const T v_min, const T v_max, const std::string_view& format = {}, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	template <Traits::Scalar T, int32_t Component>
	bool SliderScalarN(const std::string_view& label, T v[Component], const T v_min, const T v_max, const std::string_view& format = {}, ImGuiSliderFlags flags = ImGuiSliderFlags_None);

	//-----------------------------------------------------------------------------------------
	// [InputScalar]
	//-----------------------------------------------------------------------------------------

	template <Traits::Scalar T>
	bool InputScalar(const std::string_view& label, T* v, const T p_step, const std::optional<T>& p_step_fast = std::nullopt, const std::string_view& format = {}, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	template <Traits::Scalar T, int32_t Component>
	bool InputScalarN(const std::string_view& label, T v[Component], const T p_step, const std::optional<T>& p_step_fast = std::nullopt, const std::string_view& format = {}, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

	//-----------------------------------------------------------------------------------------
	// [ColorEdit]
	//-----------------------------------------------------------------------------------------

	bool ColorEdit3(const std::string_view& label, float col[3], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);
	bool ColorEdit3(const std::string_view& label, Color3f& col, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);

	bool ColorEdit4(const std::string_view& label, float col[4], ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);
	bool ColorEdit4(const std::string_view& label, Color4f& col, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);

	//-----------------------------------------------------------------------------------------
	// [Button]
	//-----------------------------------------------------------------------------------------

	bool Button(const std::string_view& label, const Vector2f& size = { 0, 0 });

	//-----------------------------------------------------------------------------------------
	// [Image]
	//-----------------------------------------------------------------------------------------

	void Image(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& uv0 = { 0, 0 }, const Vector2f& uv1 = { 1, 1 });

	void Image(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Color4f& bg_color, const Color4f& tint_color = { 0xFFFFFFFF }, const Vector2f& uv0 = { 0, 0 }, const Vector2f& uv1 = { 1, 1 });

	//-----------------------------------------------------------------------------------------
	// [Text]
	//-----------------------------------------------------------------------------------------

	void Text(const std::string_view& text);

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (Basic) namespace inline methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxGui::Traits::Scalar T>
bool SxGui::DragScalar(const std::string_view& label, T* v, float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const std::string_view& format, ImGuiSliderFlags flags) {
	const T min = v_min.value_or(std::numeric_limits<T>::lowest());
	const T max = v_max.value_or(std::numeric_limits<T>::max());
	return Internal::DragScalar(label.data(), Traits::GetImGuiDataType<T>(), v, v_speed, &min, &max, format.data(), flags);
}

template <SxGui::Traits::Scalar T, int32_t Component>
bool SxGui::DragScalarN(const std::string_view& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const std::string_view& format, ImGuiSliderFlags flags) {
	const T min = v_min.value_or(std::numeric_limits<T>::lowest());
	const T max = v_max.value_or(std::numeric_limits<T>::max());
	return Internal::DragScalarN(label.data(), Traits::GetImGuiDataType<T>(), v, Component, v_speed, &min, &max, format.data(), flags);
}

template <SxGui::Traits::Scalar T, int32_t Component>
bool SxGui::DragVectorN(const std::string_view& label, T v[Component], float v_speed, const std::optional<T>& v_min, const std::optional<T>& v_max, const std::string_view& format, ImGuiSliderFlags flags) {
	return DragScalarN<T, Component>(label, v, v_speed, v_min, v_max, format, flags | ImGuiSliderFlags_ColorMarkers); //!< カラー表示を有効にする
}

template <SxGui::Traits::Scalar T>
bool SxGui::SliderScalar(const std::string_view& label, T* v, const T v_min, const T v_max, const std::string_view& format, ImGuiSliderFlags flags) {
	return Internal::SliderScalar(label.data(), Traits::GetImGuiDataType<T>(), v, &v_min, &v_max, format.data(), flags);
}

template <SxGui::Traits::Scalar T, int32_t Component>
bool SxGui::SliderScalarN(const std::string_view& label, T v[Component], const T v_min, const T v_max, const std::string_view& format, ImGuiSliderFlags flags) {
	return Internal::SliderScalarN(label.data(), Traits::GetImGuiDataType<T>(), v, Component, &v_min, &v_max, format.data(), flags);
}

template <SxGui::Traits::Scalar T>
bool SxGui::InputScalar(const std::string_view& label, T* v, const T p_step, const std::optional<T>& p_step_fast, const std::string_view& format, ImGuiInputTextFlags flags) {
	const T* step_fast = p_step_fast.has_value() ? &p_step_fast.value() : nullptr;
	return Internal::InputScalar(label.data(), Traits::GetImGuiDataType<T>(), v, &p_step, step_fast, format.data(), flags);
}

template <SxGui::Traits::Scalar T, int32_t Component>
bool SxGui::InputScalarN(const std::string_view& label, T v[Component], const T p_step, const std::optional<T>& p_step_fast, const std::string_view& format, ImGuiInputTextFlags flags) {
	const T* step_fast = p_step_fast.has_value() ? &p_step_fast.value() : nullptr;
	return Internal::InputScalarN(label.data(), Traits::GetImGuiDataType<T>(), v, Component, &p_step, step_fast, format.data(), flags);
}

SXAVENGER_ENGINE_NAMESPACE_END
