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

//* graphics [shader]
#include "Shader/ShaderCompiler.h"

//* graphics [buffer]
#include "Buffer/ResourceAllocator.h"
#include "Buffer/ConstantBuffer.h"
#include "Buffer/DimensionBuffer.h"

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

	//* device option *//

	static void CheckDeviceStatus();

	static Device& GetDevice();

	//* descriptor heaps option *//

	static Descriptor AllocateDescriptor(DescriptorCategory category);

	static void FreeDescriptor();

	static DescriptorHeaps& GetDescriptorHeaps();

	//* command context option *//

	static void SubmitQueueAdvance(GraphicsCommandType type);

	static void SubmitQueueWait(GraphicsCommandType type);

	static void SubmitDirectQueueAdvance();

	static void SubmitDirectQueueWait();

	static GraphicsCommandContext& GetCommandContext(GraphicsCommandType type);

	static GraphicsCommandContext& GetCommandContextDirect();

	//* resource allocator option *//

	static ResourceHandle AllocateResource(const ResourceDesc& desc, uint8_t count = 1);

	static void FreeResource();

	static void IncrementFrame();

	static ResourceAllocator& GetResourceAllocator();

	//* shader compiler option *//

	static ShaderBlob CompileShader(
		const std::filesystem::path& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	);

	static ShaderReflection ReflectShader(const ShaderBlob& blob);

	static ShaderCompiler& GetShaderCompiler();

	//* [dimension buffer / constant buffer] option *//

	template <typename T>
	static ConstantBuffer<T> CreateConstantBuffer(uint8_t frameCount) {
		return ConstantBuffer<T>::Create(Core::GetDevice(), Core::GetResourceAllocator(), frameCount);
	}

	template <typename T>
	static DimensionBuffer<T> CreateDimensionBuffer(uint32_t capacity, uint8_t frameCount) {
		return DimensionBuffer<T>::Create(Core::GetDevice(), Core::GetResourceAllocator(), capacity, frameCount);
	}

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

	//* Graphics [buffer] *//

	static inline ResourceAllocator resourceAllocator_;

	//* Graphics [shader] *//

	static inline ShaderCompiler shaderCompiler_;

};

SXAVENGER_ENGINE_NAMESPACE_END
