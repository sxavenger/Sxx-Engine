#include "RenderTargetTexture.h"
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
// [RenderTargetTexture] Options structure methods
////////////////////////////////////////////////////////////////////////////////////////////

std::optional<D3D12_CLEAR_VALUE> RenderTargetTexture::Options::GetClearValue() const {

	// note: RenderTargetが存在しない場合は, std::nulloptを返す.

	D3D12_CLEAR_VALUE value = {};
	value.Format   = format;
	value.Color[0] = clearColor.r;
	value.Color[1] = clearColor.g;
	value.Color[2] = clearColor.b;
	value.Color[3] = clearColor.a;

	return value;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTargetTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderTargetTexture::Transition(const Graphics::GraphicsCommandContext& context, D3D12_RESOURCE_STATES state) {
	STREAM_ASSERT(resource_.HasHandle(), "resource is not allocated.");
	resource_.GetResource().Transition(context, state);
}

void RenderTargetTexture::ClearRenderTarget(const Graphics::GraphicsCommandContext& context) {
	if (!resource_.HasHandle()) {
		return;  //!< resourceが未割り当ての場合は何もしない.
	}

	STREAM_ASSERT(descriptorRTV_.HasHandle(), "render target descriptor is not allocated.");

	Transition(context, D3D12_RESOURCE_STATE_RENDER_TARGET); //!< resourceをRenderTarget状態に遷移する.

	//!< RenderTargetをクリアする.
	context.GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		options_.clearColor.Data(),
		0, nullptr
	);

}

void RenderTargetTexture::SetName(const std::string_view& name) const {
	resource_.SetName(std::format("RenderTargetTexture | {}", name));
}

void RenderTargetTexture::SetName(const std::wstring_view& name) const {
	resource_.SetName(std::format(L"RenderTargetTexture | {}", name));
}

Graphics::Resource& RenderTargetTexture::GetResource() {
	STREAM_ASSERT(resource_.HasHandle(), "resource is not allocated.");
	return resource_.GetResource();
}

const Graphics::Resource& RenderTargetTexture::GetResource() const {
	STREAM_ASSERT(resource_.HasHandle(), "resource is not allocated.");
	return resource_.GetResource();
}

const Graphics::Descriptor::Handle& RenderTargetTexture::GetDescriptorRTV() const {
	STREAM_ASSERT(descriptorRTV_.HasHandle(), "render target descriptor is not allocated.");
	return descriptorRTV_.GetHandle();
}

const Graphics::Descriptor::Handle& RenderTargetTexture::GetDescriptorSRV() const {
	STREAM_ASSERT(descriptorSRV_.HasHandle(), "shader resource descriptor is not allocated.");
	return descriptorSRV_.GetHandle();
}

const Graphics::Descriptor::Handle& RenderTargetTexture::GetDescriptorUAV() const {
	STREAM_ASSERT(descriptorUAV_.HasHandle(), "unordered access descriptor is not allocated.");
	return descriptorUAV_.GetHandle();
}

RenderTargetTexture RenderTargetTexture::Create(const Options& options) {

	RenderTargetTexture texture;

	//!< resourceの作成
	texture.resource_ = RenderTargetTexture::CreateResource(options);

	//!< descriptorの作成
	RenderTargetTexture::CreateDescriptorRTV(texture.descriptorRTV_, options, texture.resource_.GetResource());
	RenderTargetTexture::CreateDescriptorSRV(texture.descriptorSRV_, options, texture.resource_.GetResource());
	RenderTargetTexture::CreateDescriptorUAV(texture.descriptorUAV_, options, texture.resource_.GetResource());

	//!< optionsの設定
	texture.options_ = options;

	return texture;
}

Graphics::ResourceHandle RenderTargetTexture::CreateResource(const Options& options) {

	//!< descの作成
	Graphics::ResourceDesc desc = Graphics::ResourceDesc::CreateTextureDesc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		options.resolution.x, options.resolution.y, 1, 1,
		options.format,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		options.GetClearValue()
	);

	//!< resourceの作成
	return Graphics::Core::AllocateResource(desc, 1);
	
}

void RenderTargetTexture::CreateDescriptorRTV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource) {

	if (!descriptor.HasHandle()) { //!< Descriptorが未割り当ての場合は新規に割り当てる.
		descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::RTV);
	}

	//!< descの設定
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format        = options.format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//!< descriptorの作成
	Graphics::Core::GetDevice().GetDevice()->CreateRenderTargetView(
		resource.Get(),
		&desc,
		descriptor.GetCPUHandle()
	);
}

void RenderTargetTexture::CreateDescriptorSRV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource) {

	if (!descriptor.HasHandle()) { //!< Descriptorが未割り当ての場合は新規に割り当てる.
		descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::SRV);
	}

	//!< descの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = options.format;
	desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels     = 1;

	//!< descriptorの作成
	Graphics::Core::GetDevice().GetDevice()->CreateShaderResourceView(
		resource.Get(),
		&desc,
		descriptor.GetCPUHandle()
	);
}

void RenderTargetTexture::CreateDescriptorUAV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource) {

	if (!descriptor.HasHandle()) { //!< Descriptorが未割り当ての場合は新規に割り当てる.
		descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::UAV);
	}

	//!< descの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format        = options.format;
	desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	//!< descriptorの作成
	Graphics::Core::GetDevice().GetDevice()->CreateUnorderedAccessView(
		resource.Get(),
		nullptr,
		&desc,
		descriptor.GetCPUHandle()
	);
}
