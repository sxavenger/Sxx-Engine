#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Framework/Core/IUnit.h>

//* engine [core]
#include <Runtime/Core/Configuration/Configuration.h>

//* engine [application]
#include <Runtime/Application/Viewport.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// WindowUnit class
////////////////////////////////////////////////////////////////////////////////////////////
class WindowUnit final
	: public Framework::IUnit {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* unit methods *//

	void Setup(Framework::Pipeline& pipeline) override;

	//* window option *//

	Application::Viewport& GetViewport() { return viewport_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Application::Viewport viewport_; //!< ビューポート

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitWindow();

	void UpdateWindow();

	void PresentWindow();

};

SXAVENGER_ENGINE_NAMESPACE_END
