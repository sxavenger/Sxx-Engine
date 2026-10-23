#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "BaseBuffer.h"
#include "../Texture/RenderTargetTexture.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Container/ArrayEnum.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// LightingBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class LightingBuffer final
	: public BaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Direct,   //!< FLOAT3 DirectLightAccumulation(ACEScg) | FLOAT1 Unused
		Indirect, //!< FLOAT3 IndirectLightAccumulation(ACEScg) | FLOAT1 Unused
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	LightingBuffer() noexcept           = default;
	~LightingBuffer() noexcept override = default;

	//* buffer option *//

	void Create(const Vector2u& resolution) override;

	void Resize(const Vector2u& resolution) override;

	void ClearTarget(Graphics::GraphicsCommandContext& command) override;

	RenderTargetTexture& GetBuffer(Layout layout) { return buffers_[layout]; }
	const RenderTargetTexture& GetBuffer(Layout layout) const { return buffers_[layout]; }

	//* format option *//

	static DXGI_FORMAT GetFormat(Layout layout) { return kFormats[layout]; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const ArrayEnum<DXGI_FORMAT, EnumUtil<Layout>::GetCount(), Layout> kFormats;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ArrayEnum<RenderTargetTexture, EnumUtil<Layout>::GetCount(), Layout> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
