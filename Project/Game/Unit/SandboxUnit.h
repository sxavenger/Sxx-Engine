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
#include <Engine/Runtime/Rendering/Scene/SceneProxy.h>
#include <Engine/Runtime/Rendering/Buffer/BufferCollection.h>
#include <Engine/Runtime/Rendering/Pass/RenderPassPipeline.h>

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

	Sxx::Graphics::ReflectedGraphicsPipelineState pipeline1_;

	Sxx::World::GameObject camera_;
	Sxx::World::GameObject object_;
	Sxx::World::GameObject light_;

	Sxx::Rendering::SceneProxy scene_;
	Sxx::Rendering::BufferCollection buffer_;
	Sxx::Rendering::RenderPassPipeline pass_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSandbox();

	void TermSandbox();

	void UpdateSandbox();

	void RenderSandbox();

};
