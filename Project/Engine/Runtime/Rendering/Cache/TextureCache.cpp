#include "TextureCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>
#include <Runtime/Scheduler/System.h>

//* lib
#include <Lib/Logger/StreamLogger.h>

//* DirectXTex
#include <DirectXTex/Common/d3dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureCache::Cache(const std::shared_ptr<Assets::Texture>& texture) {

	Scheduler::TaskHandle handle = texture->GetTaskHandle();

	if (handle.GetState() != Scheduler::TaskState::State::Completed) {
		return; //!< taskが完了していない場合はキャッシュしない
	}

	//!< resourceを作成
	handle_ = TextureCache::CreateTextureResource(texture->GetName(), texture->GetDescription());
	const Graphics::Resource& resource = handle_.GetResource();

	//!< descriptorを作成
	TextureCache::CreateDescriptor(descriptor_, resource, texture->GetDescription());

	//!< resourceデータをGPUにアップロードする
	Scheduler::TaskHandle task = Scheduler::System::PushCopyQueueTask(
		std::format("Rendering::TextureCache | Upload Texture Resource | {}", texture->GetName()),
		[this, texture = texture](Graphics::GraphicsCommandContext& context) {

			Graphics::Resource& resource = handle_.GetResource();

			//!< 中間bufferの作成.
			Graphics::Resource intermediate = TextureCache::UploadResourceData(context, resource, texture->GetImage());

			resource.TransitionExplicit(context, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
			// note: 内部側でDestとして処理されているのでCommonに遷移.

			context.SubmitWait(); //!< GPUの処理完了まで待機.
		}
	);

	task.Wait(); //!< taskが完了するまで待機.

	//!< addressの取得
	address_ = texture->GetAddress();

	STREAM_LOG_INFO("Rendering::TextureCache | cache texture completed. name: {}", texture->GetName());
}

Graphics::ResourceHandle TextureCache::CreateTextureResource(const std::string_view& name, const Assets::Texture::Description& description) {
	Graphics::ResourceHandle handle = Graphics::Core::AllocateResource(
		Graphics::ResourceDesc::CreateTextureDesc(
			description.dimension,
			description.size.x, description.size.y, description.GetDepthOrArraySize(),
			description.miplevels,
			description.format,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_COMMON,
			std::nullopt
		)
	);

	handle.SetName(std::format("Rendering::TextureCache | {}", name));
	return handle;
}

NODISCARD Graphics::Resource TextureCache::UploadResourceData(const Graphics::GraphicsCommandContext& context, const Graphics::Resource& resource, const DirectX::ScratchImage& image) {
	//!< subresourceの準備
	std::vector<D3D12_SUBRESOURCE_DATA> subresources = {};
	auto hr = DirectX::PrepareUpload(
		Graphics::Core::GetDevice().GetDevice(),
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		subresources
	);
	ComPtrUtil::Assert(hr, L"texture prepare upload failed.");

	//!< upload用の中間bufferを作成
	Graphics::Resource intermediate = Graphics::Resource::CreateCommitted(
		Graphics::Core::GetDevice(),
		Graphics::ResourceDesc::CreateBufferDesc(
			D3D12_HEAP_TYPE_UPLOAD,
			GetRequiredIntermediateSize(resource.Get(), 0, static_cast<UINT>(subresources.size())), //!< 中間bufferのサイズを計算
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_COPY_SOURCE
		)
	);
	intermediate.SetName(L"Rendering::TextureCache | Upload Intermediate Resource");

	//!< subresourceをuploadする
	UpdateSubresources(
		context.GetCommandList(),
		resource.Get(),
		intermediate.Get(),
		0,
		0,
		static_cast<UINT>(subresources.size()),
		subresources.data()
	);

	return intermediate;
}

void TextureCache::CreateDescriptor(Graphics::Descriptor& descriptor, const Graphics::Resource& resource, const Assets::Texture::Description& description) {
	if (!descriptor.HasHandle()) {
		//!< descriptorが未割り当ての場合は新規に割り当てる
		descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::SRV);
	}

	//!< descの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = description.format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension           = description.GetSRVDimension();

	switch (desc.ViewDimension) { //!< 各dimensionに応じてdescを設定
		//!< miplevelをすべて使用, arrayの場合はsizeを設定.
		case D3D12_SRV_DIMENSION_TEXTURE1D:
			desc.Texture1D.MipLevels = std::numeric_limits<UINT>::max();
			break;

		case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
			desc.Texture1DArray.MipLevels = std::numeric_limits<UINT>::max();
			desc.Texture1DArray.ArraySize = description.arraySize;
			break;

		case D3D12_SRV_DIMENSION_TEXTURE2D:
			desc.Texture2D.MipLevels = std::numeric_limits<UINT>::max();
			break;

		case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
			desc.Texture2DArray.MipLevels = std::numeric_limits<UINT>::max();
			desc.Texture2DArray.ArraySize = description.arraySize;
			break;

		case D3D12_SRV_DIMENSION_TEXTURECUBE:
			desc.TextureCube.MipLevels = std::numeric_limits<UINT>::max();
			break;

		case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
			desc.TextureCubeArray.MipLevels = std::numeric_limits<UINT>::max();
			desc.TextureCubeArray.NumCubes  = description.arraySize / 6; //!< 6面を1つのcubeとして扱うため, arraySizeを6で割る
			break;

		case D3D12_SRV_DIMENSION_TEXTURE3D:
			desc.Texture3D.MipLevels = std::numeric_limits<UINT>::max();
			break;
	}

	//!< descriptorの作成
	Graphics::Core::GetDevice().GetDevice()->CreateShaderResourceView(
		resource.Get(),
		&desc,
		descriptor.GetCPUHandle()
	);
}
