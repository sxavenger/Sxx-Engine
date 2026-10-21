#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "BaseBuffer.h"
#include "../Texture/DepthStencilTexture.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>

//* lib
#include <Lib/Container/ArrayEnum.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilBuffer final
	: public BaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Scene,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	DepthStencilBuffer() noexcept           = default;
	~DepthStencilBuffer() noexcept override = default;

	//* buffer option *//

	void Create(const Vector2u& resolution) override;

	void Resize(const Vector2u& resolution) override;

	void ClearTarget(Graphics::GraphicsCommandContext& command) override;

	DepthStencilTexture& GetBuffer(Layout layout) { return buffers_[layout]; }
	const DepthStencilTexture& GetBuffer(Layout layout) const { return buffers_[layout]; }

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

	ArrayEnum<DepthStencilTexture, EnumUtil<Layout>::GetCount(), Layout> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
