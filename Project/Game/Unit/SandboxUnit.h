#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [unit]
#include <Engine/Runtime/Framework/Core/IUnit.h>

//* engine [graphics]
#include <Engine/Runtime/Graphics/Pipeline/ReflectedGraphicsPipelineState.h>

//* engine [assets]
#include <Engine/Runtime/Assets/Texture/Texture.h>
#include <Engine/Runtime/Assets/Mesh/StaticMesh.h>
#include <Engine/Runtime/Assets/Handle/AssetHandle.h>

//* engine [world]
#include <Engine/Runtime/World/Entity/GameObject.h>

//* engine [rendering]
#include <Engine/Runtime/Rendering/Texture/RenderTargetTexture.h>
#include <Engine/Runtime/Rendering/Texture/DepthStencilTexture.h>

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

	Sxx::Rendering::RenderTargetTexture renderTarget_;
	Sxx::Rendering::DepthStencilTexture depthStencil_;

	Sxx::Assets::AssetHandle<Sxx::Assets::StaticMesh> handle_;

	Sxx::World::GameObject camera_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSandbox();

	void TermSandbox();

	void UpdateSandbox();

	void RenderSandbox();

};
