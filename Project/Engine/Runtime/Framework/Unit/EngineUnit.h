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

//* engine [platform]
#include <Runtime/Platform/Input/InputSystem.h>

//* engine [application]
#include <Runtime/Application/FrameClock.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Framework)

////////////////////////////////////////////////////////////////////////////////////////////
// EngineUnit class
////////////////////////////////////////////////////////////////////////////////////////////
class EngineUnit final
	: public IUnit {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	EngineUnit();
	~EngineUnit();

	//* unit methods *//

	void Setup(Pipeline& pipeline) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Configuration configuration_; //!< エンジンの設定情報

	Platform::InputSystem input_;

	Application::FrameClock frameClock_; //!< フレームクロック

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitEngine();

	void TermEngine();

};

SXAVENGER_ENGINE_NAMESPACE_END
