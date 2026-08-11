#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [unit]
#include <Engine/Runtime/Framework/Core/IUnit.h>

//* engine [graphics]
#include <Engine/Runtime/Graphics/Pipeline/GraphicsPipelineState.h>
#include <Engine/Runtime/Graphics/Pipeline/ShaderBindingLayout.h>

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

	Sxx::Graphics::GraphicsPipelineState pipeline_;
	Sxx::Graphics::ShaderBindingLayout layout_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSandbox();

	void TermSandbox();

	void UpdateSandbox();

	void RenderSandbox();

};
