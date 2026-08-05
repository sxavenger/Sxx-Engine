#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "GraphicsUtil.h"
#include "PixEvent.h"

//* graphics [core]
#include "Core/DebugReporter.h"
#include "Core/DebugLayer.h"
#include "Core/Device.h"
#include "Core/InfoQueue.h"
#include "Core/DescriptorHeaps.h"
#include "Core/GraphicsCommandContext.h"
#include "Core/ShaderCompiler.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* lib
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// Core class
////////////////////////////////////////////////////////////////////////////////////////////
class Core final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* core option *//

	static void Init(const Configuration& config);

	static void Term();

	static void BeginFrame();

	//* device option *//

	static Device& GetDevice();

	//* descriptor heaps option *//

	static Descriptor AllocateDescriptor(DescriptorCategory category);

	static DescriptorHeaps& GetDescriptorHeaps();

	//* command context option *//

	static void SubmitQueue(GraphicsCommandType type);

	static void SubmitDirectQueue();

	static GraphicsCommandContext& GetCommandContext(GraphicsCommandType type);

	static GraphicsCommandContext& GetCommandContextDirect();

	//* shader compiler option *//

	static ShaderBlob CompileShader(
		const std::filesystem::path& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

	static ShaderCompiler& GetShaderCompiler();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* Graphics [core] *//

	static inline DebugReporter reporter_; //!< LiveObjectのレポートを行うオブジェクト.
	
	static inline DebugLayer debugLayer_;
	static inline Device device_;
	static inline InfoQueue infoQueue_;
	static inline DescriptorHeaps descriptorHeaps_;
	static inline GraphicsCommandContext contexts_[EnumUtil<GraphicsCommandType>::GetCount()]; //!< GraphicsCommandContextの配列.
	//!< copy, computeは非同期側の使用を想定.

	static inline ShaderCompiler shaderCompiler_;

};

SXAVENGER_ENGINE_NAMESPACE_END
