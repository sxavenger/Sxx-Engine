#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* imgui
#include <imgui.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui [Traits] namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui::Traits {

	//-----------------------------------------------------------------------------------------
	// concepts
	//-----------------------------------------------------------------------------------------

	template <typename T>
	concept Scalar
		= std::is_same_v<T, int8_t>
		|| std::is_same_v<T, uint8_t>
		|| std::is_same_v<T, int16_t>
		|| std::is_same_v<T, uint16_t>
		|| std::is_same_v<T, int32_t>
		|| std::is_same_v<T, uint32_t>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, uint64_t>
		|| std::is_same_v<T, float>
		|| std::is_same_v<T, double>;
	//!< ImGuiで扱えるスカラー型

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <Scalar T>
	constexpr ImGuiDataType GetImGuiDataType();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui [Traits] namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SxGui::Traits::Scalar T>
constexpr ImGuiDataType SxGui::Traits::GetImGuiDataType() {
	if constexpr (std::is_same_v<T, int8_t>)        return ImGuiDataType_S8;
	else if constexpr (std::is_same_v<T, uint8_t>)  return ImGuiDataType_U8;
	else if constexpr (std::is_same_v<T, int16_t>)  return ImGuiDataType_S16;
	else if constexpr (std::is_same_v<T, uint16_t>) return ImGuiDataType_U16;
	else if constexpr (std::is_same_v<T, int32_t>)  return ImGuiDataType_S32;
	else if constexpr (std::is_same_v<T, uint32_t>) return ImGuiDataType_U32;
	else if constexpr (std::is_same_v<T, int64_t>)  return ImGuiDataType_S64;
	else if constexpr (std::is_same_v<T, uint64_t>) return ImGuiDataType_U64;
	else if constexpr (std::is_same_v<T, float>)    return ImGuiDataType_Float;
	else if constexpr (std::is_same_v<T, double>)   return ImGuiDataType_Double;
	else static_assert(false, "unsupported type for ImGuiDataType");
}

SXAVENGER_ENGINE_NAMESPACE_END
