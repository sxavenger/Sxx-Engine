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
// RenderPassAntiAliasing class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassAntiAliasing final
	: public BaseRenderPass {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pipeline : uint8_t {
		FXAA,
	};

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

	RenderPassAntiAliasing();
	~RenderPassAntiAliasing() override = default;

	//* pass option *//

	void Setup(MAYBE_UNUSED const PassContext& context) override;

	void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ArrayEnum<Graphics::ReflectedComputePipelineState, EnumUtil<Pipeline>::GetCount(), Pipeline> pipelines_;

	//* pass data *//

	PassData data_ = {};

	// TODO: pass特有のGPU変数の調整方法を考える.

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginAntiAliasingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void EndAntiAliasingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void AntiAliasingFXAA(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
