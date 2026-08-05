#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* framework
#include "../Core/IUnit.h"

//* engine
#include <Runtime/Foundation.hpp>

//* engine [core]
#include <Runtime/Core/Configuration/Configuration.h>

//* engine [application]
#include <Runtime/Application/Viewport.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Framework)

////////////////////////////////////////////////////////////////////////////////////////////
// WindowUnit class
////////////////////////////////////////////////////////////////////////////////////////////
class WindowUnit final
	: public IUnit {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* unit methods *//

	void Setup(Pipeline& pipeline) override;

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
