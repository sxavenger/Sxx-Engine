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
// RenderPassCombineLighting class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassCombineLighting final
	: public BaseRenderPass {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pipeline : uint8_t {
		Combine
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

	RenderPassCombineLighting();
	~RenderPassCombineLighting() override = default;

	//* pass option *//

	void Setup(MAYBE_UNUSED const PassContext& context) override;

	void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	ArrayEnum<Graphics::ReflectedComputePipelineState, EnumUtil<Pipeline>::GetCount(), Pipeline> pipelines_;

	//* pass data *//

	PassData data_;

	// TODO: pass特有のGPU変数の調整方法を考える.

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginCombineLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void EndCombineLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void Combine(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
