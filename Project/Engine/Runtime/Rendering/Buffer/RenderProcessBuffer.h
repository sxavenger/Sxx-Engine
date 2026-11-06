#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "BaseBuffer.h"
#include "../Texture/RenderTargetTexture.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderProcessBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderProcessBuffer final
	: public BaseBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	RenderProcessBuffer() noexcept = default;
	~RenderProcessBuffer() noexcept override = default;

	//* buffer option *//

	void Create(const Resolution& resolution) override;

	void Resize(const Resolution& resolution) override;

	void ClearTarget(Graphics::GraphicsCommandContext& command) override;

	//! @brief 次のbufferに切り替える.
	void IncrementBuffer();

	RenderTargetTexture& GetCurrentBuffer();
	const RenderTargetTexture& GetCurrentBuffer() const;

	RenderTargetTexture& GetPreviousBuffer(uint8_t offset = 1);
	const RenderTargetTexture& GetPreviousBuffer(uint8_t offset = 1) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const DXGI_FORMAT kFormat;
	static const uint8_t kBufferCount = 2; //!< buffer数.

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<RenderTargetTexture, kBufferCount> buffers_ = {};
	uint8_t currentBufferIndex_ = 0; //!< 現在のbuffer index.

};

SXAVENGER_ENGINE_NAMESPACE_END
