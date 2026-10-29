#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui [Internal] namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui::Internal {

	////////////////////////////////////////////////////////////////////////////////////////////
	// widget internal methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//* DragScalarX *//

	bool DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);

	bool DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);

	//* SliderScalarX *//

	bool SliderScalar(const char* label, ImGuiDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags);

	bool SliderScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags);

	//* InputScalarN *//

	bool InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);

	bool InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);

	//* ColorEditX *//

	bool ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags);

	bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags);
	
}

SXAVENGER_ENGINE_NAMESPACE_END
