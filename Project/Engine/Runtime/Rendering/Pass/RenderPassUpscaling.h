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
#include <Runtime/Rendering/Upscaler/UpscalerNIS.h>

//* lib
#include <Lib/Container/ArrayEnum.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassUpscaling class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassUpscaling final
	: public BaseRenderPass {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pipeline : uint8_t {
		Bilinear,
		NIS,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// PassData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PassData final {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		float sharpness = 0.5f; //!< [0.0 - 1.0]

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	RenderPassUpscaling();
	~RenderPassUpscaling() override = default;

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

	//* upscaler *//

	UpscalerNIS upscalerNIS_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginUpscalingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void EndUpscalingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void UpscalingBilinear(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data);

	void UpscalingNIS(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
