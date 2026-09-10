#include "DepthStencilTexture.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Buffer/ResourceDesc.h>
#include <Runtime/Graphics/Core.h>

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [DepthStencilTexture] Options structure methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_CLEAR_VALUE DepthStencilTexture::Options::GetClearValue() const {

	D3D12_CLEAR_VALUE value = {};
	value.Format               = format;
	value.DepthStencil.Depth   = depthClearValue;
	value.DepthStencil.Stencil = stencilClearValue;

	return value;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthStencilTexture::Transition(const Graphics::GraphicsCommandContext& context, D3D12_RESOURCE_STATES state) {
	STREAM_ASSERT(resource_.HasHandle(), "resource is not allocated.");
	resource_.GetResource().Transition(context, state);
}

void DepthStencilTexture::ClearDepthStencil(const Graphics::GraphicsCommandContext& context) {
	if (!resource_.HasHandle()) {
		return;  //!< resourceが未割り当ての場合は何もしない.
	}

	STREAM_ASSERT(descriptorDSV_.HasHandle(), "depth stencil descriptor is not allocated.");

	Transition(context, D3D12_RESOURCE_STATE_DEPTH_WRITE); //!< resourceをDepthWrite状態に遷移する.

	//!< DepthStencilをクリアする.
	context.GetCommandList()->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		options_.depthClearValue,
		options_.stencilClearValue,
		0, nullptr
	);
}

void DepthStencilTexture::SetName(const std::string_view& name) const {
	resource_.SetName(std::format("DepthStencilTexture | {}", name));
}

void DepthStencilTexture::SetName(const std::wstring_view& name) const {
	resource_.SetName(std::format(L"DepthStencilTexture | {}", name));
}

Graphics::Resource& DepthStencilTexture::GetResource() {
	STREAM_ASSERT(resource_.HasHandle(), "resource is not allocated.");
	return resource_.GetResource();
}

const Graphics::Resource& DepthStencilTexture::GetResource() const {
	STREAM_ASSERT(resource_.HasHandle(), "resource is not allocated.");
	return resource_.GetResource();
}

const Graphics::Descriptor::Handle& DepthStencilTexture::GetDescriptorDSV() const {
	STREAM_ASSERT(descriptorDSV_.HasHandle(), "depth stencil descriptor is not allocated.");
	return descriptorDSV_.GetHandle();
}

const Graphics::Descriptor::Handle& DepthStencilTexture::GetDescriptorSRV() const {
	STREAM_ASSERT(descriptorSRV_.HasHandle(), "shader resource descriptor is not allocated.");
	return descriptorSRV_.GetHandle();
}

DepthStencilTexture DepthStencilTexture::Create(const Options& options) {

	DepthStencilTexture texture;

	//!< resourceの作成
	texture.resource_ = DepthStencilTexture::CreateResource(options);

	//!< descriptorの作成
	DepthStencilTexture::CreateDescriptorDSV(texture.descriptorDSV_, options, texture.resource_.GetResource());
	DepthStencilTexture::CreateDescriptorSRV(texture.descriptorSRV_, options, texture.resource_.GetResource());

	//!< optionsの設定
	texture.options_ = options;

	return texture;
	
}

Graphics::ResourceHandle DepthStencilTexture::CreateResource(const Options& options) {

	//!< descの作成
	Graphics::ResourceDesc desc = Graphics::ResourceDesc::CreateTextureDesc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		options.resolution.x, options.resolution.y, 1, 1,
		options.format,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		options.GetClearValue()
	);

	//!< resourceの作成
	return Graphics::Core::AllocateResource(desc, 1);
	
}

void DepthStencilTexture::CreateDescriptorDSV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource) {

	if (!descriptor.HasHandle()) { //!< Descriptorが未割り当ての場合は新規に割り当てる.
		descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::DSV);
	}

	//!< descの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format        = options.format;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	//!< descriptorの作成
	Graphics::Core::GetDevice().GetDevice()->CreateDepthStencilView(
		resource.Get(),
		&desc,
		descriptor.GetCPUHandle()
	);
}

void DepthStencilTexture::CreateDescriptorSRV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource) {

	if (!descriptor.HasHandle()) { //!< Descriptorが未割り当ての場合は新規に割り当てる.
		descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::SRV);
	}

	//!< descの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                          = Graphics::ConvertDepthViewFormat(options.format);
	desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels             = 1;

	//!< descriptorの作成
	Graphics::Core::GetDevice().GetDevice()->CreateShaderResourceView(
		resource.Get(),
		&desc,
		descriptor.GetCPUHandle()
	);

}
