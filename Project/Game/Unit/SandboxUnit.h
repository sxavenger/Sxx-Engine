#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [unit]
#include <Engine/Runtime/Framework/Core/IUnit.h>

//* engine [graphics]
#include <Engine/Runtime/Graphics/Pipeline/ReflectedGraphicsPipelineState.h>
#include <Engine/Runtime/Graphics/Buffer/ResourceHandle.h>

//* engine [world]
#include <Engine/Runtime/World/Entity/GameObject.h>

//* engine [rendering]
#include <Engine/Runtime/Rendering/Texture/RenderTargetTexture.h>
#include <Engine/Runtime/Rendering/Texture/DepthStencilTexture.h>
#include <Engine/Runtime/Rendering/Cache/StaticMeshCache.h>
#include <Engine/Runtime/Rendering/Cache/CameraCache.h>
#include <Engine/Runtime/Rendering/Cache/TransformCache.h>

//* lib
#include <Lib/Math/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SandboxUnit class
////////////////////////////////////////////////////////////////////////////////////////////
class SandboxUnit final
	: public Sxx::Framework::IUnit {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SandboxUnit();
	~SandboxUnit();

	//* unit methods *//

	void Setup(Sxx::Framework::Pipeline& pipeline) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Sxx::Graphics::ReflectedGraphicsPipelineState pipeline0_;
	Sxx::Graphics::ReflectedGraphicsPipelineState pipeline1_;

	Sxx::Rendering::StaticMeshCache cache_;

	Sxx::Rendering::RenderTargetTexture renderTarget_;
	Sxx::Rendering::DepthStencilTexture depthStencil_;

	Sxx::World::GameObject object_;
	Sxx::Rendering::CameraCache cameraCache_;
	Sxx::Rendering::TransformCache transformCache_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSandbox();

	void TermSandbox();

	void UpdateSandbox();

	void RenderSandbox();

};
