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
#include <Runtime/Graphics/Pipeline/ReflectedGraphicsPipelineState.h>

//* lib
#include <Lib/Container/ArrayEnum.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassDirectLighting class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassDirectLighting final
	: public BaseRenderPass {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Desc : uint8_t {
		Forward, //!< Depthが1.0より手前の場合に有効
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pipeline : uint8_t {
		DirectionalLight,
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

	RenderPassDirectLighting();
	~RenderPassDirectLighting() override = default;

	//* pass option *//

	void Setup(const PassContext& context) override;

	void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	ArrayEnum<Graphics::ReflectedGraphicsPipelineState::Desc, EnumUtil<Pipeline>::GetCount(), Desc> descs_;
	ArrayEnum<Graphics::ReflectedGraphicsPipelineState, EnumUtil<Pipeline>::GetCount(), Pipeline> pipelines_;

	//* pass data *//

	PassData data_;

	// TODO: pass特有のGPU変数の調整方法を考える.

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginDirectLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void EndDirectLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void LightingDirectionalLight(Graphics::GraphicsCommandContext& command, const PassContext& context);

};

SXAVENGER_ENGINE_NAMESPACE_END
