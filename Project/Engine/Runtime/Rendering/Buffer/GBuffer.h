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
// GBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class GBuffer final
	: public BaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		GBufferA, //!< FLOAT3 Albedo(sRGB) | FLOAT1 Unused
		GBufferB, //!< FLOAT3 WorldNormal | FLOAT1 Unused
		GBufferC, //!< FLOAT1 AmbientOcclusion | FLOAT1 Roughness | FLOAT1 Metallic | FLOAT1 Unused
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	GBuffer() noexcept           = default;
	~GBuffer() noexcept override = default;

	//* buffer option *//

	void Create(const Vector2u& resolution) override;

	void Resize(const Vector2u& resolution) override;

	void ClearTarget(Graphics::GraphicsCommandContext& command) override;

	RenderTargetTexture& GetBuffer(Layout layout) { return buffers_[layout]; }

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
