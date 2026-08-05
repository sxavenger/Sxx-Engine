#include "Core.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Core class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Core::Init(const Configuration& config) {

	PixEvent::Init(config); //!< PixEventの初期化

	//!< Graphicsの初期化
	debugLayer_.Init(config);
	device_.Init(config);
	infoQueue_.Init(config, device_);
	descriptorHeaps_.Init(config, device_);

	for (uint8_t i = 0; i < EnumUtil<GraphicsCommandType>::GetCount(); ++i) {
		contexts_[i].Init(config, device_, static_cast<GraphicsCommandType>(i));
		contexts_[i].SetName(L"Graphics::Core");

		if (i != EnumUtil<GraphicsCommandType>::Cast(GraphicsCommandType::Copy)) {
			contexts_[i].SetDescriptorHeaps(descriptorHeaps_);
		}
	}

	shaderCompiler_.Init(config, device_);

	StreamLogger::Info("Graphics::Core | graphics initialized.");
}

void Core::Term() {

	for (uint8_t i = 0; i < EnumUtil<GraphicsCommandType>::GetCount(); ++i) {
		contexts_[i].ExecuteAll(); //!< CommandListを全て実行する.
	}
	
	StreamLogger::Info("Graphics::Core | graphics terminated.");
}

void Core::BeginFrame() {
	{
		Device::Status status = device_.CheckDeviceStatus(); //!< デバイスの状態をチェックする.

		if (status == Device::Status::Removed) {
			StreamLogger::Exception(
				"graphics device removed."
			); //!< デバイスが削除された場合は例外を投げる.
		}
	}
}

Device& Core::GetDevice() {
	return device_;
}

Descriptor Core::AllocateDescriptor(DescriptorCategory category) {
	return descriptorHeaps_.Allocate(category);
}

DescriptorHeaps& Core::GetDescriptorHeaps() {
	return descriptorHeaps_;
}

void Core::SubmitQueue(GraphicsCommandType type) {
	contexts_[EnumUtil<GraphicsCommandType>::Cast(type)].ExecuteAll();

	if (type != GraphicsCommandType::Copy) {
		contexts_[EnumUtil<GraphicsCommandType>::Cast(type)].SetDescriptorHeaps(descriptorHeaps_);
	}
}

void Core::SubmitDirectQueue() {
	Core::SubmitQueue(GraphicsCommandType::Direct);
}

GraphicsCommandContext& Core::GetCommandContext(GraphicsCommandType type) {
	return contexts_[EnumUtil<GraphicsCommandType>::Cast(type)];
}

GraphicsCommandContext& Core::GetCommandContextDirect() {
	return contexts_[EnumUtil<GraphicsCommandType>::Cast(GraphicsCommandType::Direct)];
}

ShaderBlob Core::CompileShader(const std::filesystem::path& filepath, CompileProfile profile, const std::wstring& entryPoint) {
	return shaderCompiler_.Compile(filepath, profile, entryPoint);
}

ShaderCompiler& Core::GetShaderCompiler() {
	return shaderCompiler_;
}
