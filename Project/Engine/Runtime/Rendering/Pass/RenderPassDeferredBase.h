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

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassDeferredBase class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassDeferredBase final
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

		float alphaCutoff = 0.1f; //!< 不透明メッシュのアルファカットオフ値
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	RenderPassDeferredBase();
	~RenderPassDeferredBase() override = default;

	//* pass option *//

	void Setup(const PassContext& context) override;

	void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	Graphics::ReflectedGraphicsPipelineState pipeline_;

	//* pass data *//

	PassData data_;

	// TODO: pass特有のGPU変数の調整方法を考える.

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginOpaqueMeshRenderPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void EndOpaqueMeshRenderPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer);

	void RenderStaticMesh(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
