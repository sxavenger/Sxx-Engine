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
#include <Lib/Container/ArrayEnum.h>

//* c++
#include <variant>

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

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using BlendColor       = std::variant<BlendModeColor, BlendDesc>;
	using BlendTransparent = std::variant<BlendModeTransparent, BlendDesc>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Index enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Index : uint8_t {
		Mode, //!< BlendMode
		Desc, //!< BlendDesc
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static D3D12_RENDER_TARGET_BLEND_DESC CreateBlendDesc(const BlendColor& color, const BlendTransparent& transparent, D3D12_COLOR_WRITE_ENABLE writeMask = D3D12_COLOR_WRITE_ENABLE_ALL);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const ArrayEnum<BlendDesc, EnumUtil<BlendModeColor>::GetCount(), BlendModeColor> kBlendDescColor;
	static const ArrayEnum<BlendDesc, EnumUtil<BlendModeTransparent>::GetCount(), BlendModeTransparent> kBlendDescTransparent;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static BlendDesc GetBlendDesc(const BlendColor& color);

	static BlendDesc GetBlendDesc(const BlendTransparent& transparent);

};

SXAVENGER_ENGINE_NAMESPACE_END
