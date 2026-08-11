#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <variant>
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderParameter structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief シェーダーのルートパラメータにバインドするデータを保持する構造体
struct ShaderParameter final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RootConstants structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 32bit定数としてルートパラメータにバインドするデータと個数を保持する構造体
	struct RootConstants {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* data *//

		UINT num32bit;
		const void* data;

		//* constant *//

		static constexpr size_t k32bitConstantSize = sizeof(UINT);

	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using Parameter = std::variant<
		RootConstants,                //!< root constants
		D3D12_GPU_VIRTUAL_ADDRESS,    //!< virtual address
		D3D12_GPU_DESCRIPTOR_HANDLE   //!< handle
	>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* parameter binding option *//

	void Set32bitConstants(const std::string& name, UINT num32bit, const void* data);

	template <typename T> requires std::is_trivially_copyable_v<T>
	void Set32bitConstants(const std::string& name, const T& data);

	void SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address);

	void SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	//* parameter container option *//

	void Clear();

	void Merge(const ShaderParameter& parameter);

	//* parameter access option *//

	bool Contains(const std::string& name) const;

	const Parameter& GetParameter(const std::string& name) const;

	const RootConstants& Get32bitConstants(const std::string& name) const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetAddress(const std::string& name) const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandle(const std::string& name) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::unordered_map<std::string, Parameter> parameters;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderParameter structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> requires std::is_trivially_copyable_v<T>
void ShaderParameter::Set32bitConstants(const std::string& name, const T& data) {
	Set32bitConstants(name, sizeof(T) / RootConstants::k32bitConstantSize, &data);
}

SXAVENGER_ENGINE_NAMESPACE_END
