#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>
#include <Runtime/Graphics/Pipeline/ReflectedComputePipelineState.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class SkinningPipeline final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Context structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Context {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		uint32_t vertexCount = 0; //!< skinning対象の頂点数
		uint32_t jointCount  = 0; //!< skinning対象のjoint数

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void BindPipeline(const Graphics::GraphicsCommandContext& command);

	static void BindShaderParameter(const Graphics::GraphicsCommandContext& command, const Graphics::ShaderParameter& parameter);

	static void Dispatch(const Graphics::GraphicsCommandContext& command, uint32_t vertexCount);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* pipeline *//

	static inline Graphics::ReflectedComputePipelineState pipeline_;

};

SXAVENGER_ENGINE_NAMESPACE_END
