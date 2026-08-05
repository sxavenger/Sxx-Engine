#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "BaseScene.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// SceneContext class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* scene option *//

	//* factory option *//

	void Register(const std::string& name, const std::function<std::unique_ptr<BaseScene>()>& factory);
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* scene manager *//

	std::unique_ptr<BaseScene> current_ = nullptr; //!< 現在のScene
	std::unique_ptr<BaseScene> next_ = nullptr;    //!< 次のScene

	//* factory *//

	std::unordered_map<std::string, std::function<std::unique_ptr<BaseScene>()>> factory_; //!< Sceneの生成関数のマップ

};

SXAVENGER_ENGINE_NAMESPACE_END
