#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "Device.h"
#include "DescriptorAllocator.h"
#include "Descriptor.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* lib
#include <Lib/CXXAttribute.hpp>
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Container/IndexAllocator.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorHeaps class
////////////////////////////////////////////////////////////////////////////////////////////
class DescriptorHeaps final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Parse(const toml::table& config);

		static Settings ParseFromConfig(const Configuration& config);

		static void Log(const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.DescriptorHeaps" }; //!< 設定のパス.

		UINT descriptorCapacityRTV         = 12; //!< RTVのデスクリプタヒープのデスクリプタ数.
		UINT descriptorCapacityDSV         = 12; //!< DSVのデスクリプタヒープのデスクリプタ数.
		UINT descriptorCapacityCBV_SRV_UAV = 12; //!< CBV/SRV/UAVのデスクリプタヒープのデスクリプタ数.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Configuration& config, const Device& device);

	//* descriptor option *//

	NODISCARD Descriptor Allocate(DescriptorCategory category);

	//* descriptor allocator option *//

	DescriptorAllocator& GetAllocator(DescriptorCategory category);
	const DescriptorAllocator& GetAllocator(DescriptorCategory category) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor pools *//

	std::array<DescriptorAllocator, EnumUtil<DescriptorCategory>::GetCount()> pools_;

	//* status *//

	Settings settings_;

};

SXAVENGER_ENGINE_NAMESPACE_END
