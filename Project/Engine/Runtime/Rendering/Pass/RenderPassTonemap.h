#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "BaseRenderPass.h"
#include "PassContext.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>
#include <Runtime/Graphics/Pipeline/ReflectedComputePipelineState.h>

//* lib
#include <Lib/Container/ArrayEnum.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassTonemap class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassTonemap final
	: public BaseRenderPass {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PassData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PassData final {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	RenderPassTonemap();
	~RenderPassTonemap() override = default;

	//* pass option *//

	void Setup(const PassContext& context) override;

	void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	Graphics::ReflectedComputePipelineState pipeline_;

	//* pass data *//

	PassData data_;

	// TODO: pass特有のGPU変数の調整方法を考える.

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginTonemapPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void EndTonemapPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void Tonemap(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
