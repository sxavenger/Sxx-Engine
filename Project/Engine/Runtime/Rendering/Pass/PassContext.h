#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "PassOption.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Rendering/Cache/Dynamic/CameraCache.h>
#include <Runtime/Rendering/Cache/Dynamic/TransformCache.h>
#include <Runtime/Rendering/Scene/SceneProxy.h>
#include <Runtime/Rendering/Buffer/BufferCollection.h>

//* lib
#include <Lib/Flag/Flag.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PassContext structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PassContext final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Issue enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Issue : size_t {

		Success = 0,

		//* critical error *//

		Error_Buffer = 1 << 0,

		//* warning *//

		Warning_Camera = 1 << 1,
		Warning_Scene  = 1 << 2,

		//* issues *//

		Error   = Error_Buffer,
		Warning = Warning_Camera | Warning_Scene,

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::shared_ptr<const CameraCache> camera = nullptr;
		std::shared_ptr<const TransformCache> transform = nullptr;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ContextData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief GPU側で使用されるContextのデータを格納する構造体.
	struct ContextData {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2u resolution                             = {};
		PassOption::WorkingColorSpace workingColorSpace = PassOption::WorkingColorSpace::Linear;
		Graphics::DisplayColorSpace displayColorSpace   = Graphics::DisplayColorSpace::Rec709;
		PassOption::DisplayEncoder displayEncoder       = PassOption::DisplayEncoder::Hardware;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* issue option *//

	static FlagEnum<Issue> CheckIssue(const PassContext& context);

	//* context data option *//

	ContextData GetContextData() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::string name = "---";

	const SceneProxy* scene = nullptr;

	BufferCollection* buffer = nullptr;
	Vector2u resolution = {};

	Camera camera = {};

	PassOption option = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
