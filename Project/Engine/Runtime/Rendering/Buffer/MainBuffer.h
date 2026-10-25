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
// MainBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class MainBuffer final
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

	MainBuffer() noexcept           = default;
	~MainBuffer() noexcept override = default;

	//* buffer option *//

	void Create(const Vector2u& resolution) override;

	void Resize(const Vector2u& resolution) override;

	void ClearTarget(Graphics::GraphicsCommandContext& command) override;

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
