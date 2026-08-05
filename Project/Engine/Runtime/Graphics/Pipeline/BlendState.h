#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// BlendModeColor enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class BlendModeColor : uint8_t {
	None, 
	Normal,      //!< SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
	Additive,    //!< SrcColor * 1 + DestColor * 1
	Subtractive, //!< SrcColor * 1 - DestColor * 1
	Multiply,    //!< SrcColor * DestColor + DestColor * 0
	Screen,      //!< SrcColor * (1 - DestColor) + DestColor * 1
};

////////////////////////////////////////////////////////////////////////////////////////////
// BlendModeTransparent enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class BlendModeTransparent : uint8_t {
	None,
	Additive,    //!< SrcAlpha + DestAlpha
	Max,         //!< max(SrcAlpha, DestAlpha)
};

////////////////////////////////////////////////////////////////////////////////////////////
// BlendState class
////////////////////////////////////////////////////////////////////////////////////////////
class BlendState final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BlendDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BlendDesc {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool enable;
		D3D12_BLEND srcBlend;
		D3D12_BLEND destBlend;
		D3D12_BLEND_OP blendOp;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static D3D12_RENDER_TARGET_BLEND_DESC CreateBlendDesc(BlendModeColor color, BlendModeTransparent transparent, D3D12_COLOR_WRITE_ENABLE writeMask = D3D12_COLOR_WRITE_ENABLE_ALL);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const std::array<BlendDesc, EnumUtil<BlendModeColor>::GetCount()> kBlendDescColor;
	static const std::array<BlendDesc, EnumUtil<BlendModeTransparent>::GetCount()> kBlendDescTransparent;

};

SXAVENGER_ENGINE_NAMESPACE_END
