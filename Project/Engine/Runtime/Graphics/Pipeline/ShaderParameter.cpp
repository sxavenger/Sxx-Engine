#include "ShaderParameter.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderParameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderParameter::Set32bitConstants(const std::string& name, UINT num32bit, const void* data) {
	parameters[name] = RootConstants{ num32bit, data };
}

void ShaderParameter::SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	parameters[name] = address;
}

void ShaderParameter::SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	parameters[name] = handle;
}

void ShaderParameter::Clear() {
	parameters.clear();
}

void ShaderParameter::Merge(const ShaderParameter& parameter) {
	for (const auto& [name, param] : parameter.parameters) {
		parameters[name] = param;
	}
}

bool ShaderParameter::Contains(const std::string& name) const {
	return parameters.contains(name);
}

const ShaderParameter::Parameter& ShaderParameter::GetParameter(const std::string& name) const {
	StreamLogger::Assert(parameters.contains(name), std::format("parameter not found. name: {}", name));
	return parameters.at(name);
}

const ShaderParameter::RootConstants& ShaderParameter::Get32bitConstants(const std::string& name) const {
	const Parameter& parameter = GetParameter(name);

	StreamLogger::Assert(std::holds_alternative<RootConstants>(parameter), std::format("parameter type is not RootConstants. name: {}", name));
	return std::get<RootConstants>(parameter);
}

const D3D12_GPU_VIRTUAL_ADDRESS& ShaderParameter::GetAddress(const std::string& name) const {
	const Parameter& parameter = GetParameter(name);

	StreamLogger::Assert(std::holds_alternative<D3D12_GPU_VIRTUAL_ADDRESS>(parameter), std::format("parameter type is not D3D12_GPU_VIRTUAL_ADDRESS. name: {}", name));
	return std::get<D3D12_GPU_VIRTUAL_ADDRESS>(parameter);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& ShaderParameter::GetHandle(const std::string& name) const {
	const Parameter& parameter = GetParameter(name);

	StreamLogger::Assert(std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(parameter), std::format("parameter type is not D3D12_GPU_DESCRIPTOR_HANDLE. name: {}", name));
	return std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(parameter);
}
