#include "TextureBuilder.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/System.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>
#include <Lib/Format/Json/JsonReader.h>

//* DirectXTex
#include <DirectXTex/Common/d3dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureBuilder::Build(std::shared_ptr<Texture>& texture) {
	if (!std::filesystem::exists(texture->GetFilepath())) {
		StreamLogger::Error("Asset::TextureBuilder | file does not exist. filepath: {}", texture->GetFilepath().generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< Assetファイルの読み込み
	json::node node = JsonFile::Load(texture->GetFilepath());

	//!< metadataの設定
	texture->metadata_ = TextureMetadata::Deserialize(node["metadata"]);

	if (texture->metadata_.GetType() == MetadataType::Unknown) {
		StreamLogger::Error("Asset::TextureBuilder | texture metadata type is unknown. filepath: {}", texture->GetFilepath().generic_string());
		return; //!< metadataがUnknownの場合は処理を終了
	}

	const TextureMetadata::ReferenceData& reference = texture->metadata_.GetReferenceData(); //!< 参照データの取得

	//!< 画像ファイルの読み込み
	DirectX::ScratchImage image = TextureBuilder::LoadTextureFile(texture->GetDirectory(), reference);

	if (reference.encoding != Graphics::GetColorEncoding(image.GetMetadata().format)) {
		StreamLogger::Warning("Asset::TextureBuilder | encoding is mismatched. filepath: {}", texture->GetFilepath().generic_string()); //!< 読み込み結果とencodingが異なる場合は警告を出す
	}

	texture->description_ = Texture::Description::Parse(image.GetMetadata()); //!< descriptionの設定

	StreamLogger::Info("Asset::TextureBuilder | texture build completed. name: {}", texture->GetName());
}

DirectX::ScratchImage TextureBuilder::GenerateMipmaps(const DirectX::ScratchImage& source, DirectX::TEX_FILTER_FLAGS flags) {

	DirectX::ScratchImage image = {};

	//!< mipmapの生成
	auto hr = DirectX::GenerateMipMaps(
		source.GetImages(),
		source.GetImageCount(),
		source.GetMetadata(),
		flags,
		0,
		image
	);
	ComPtrUtil::Assert(hr, L"mipmaps create failed.");

	return image;
}

DirectX::ScratchImage TextureBuilder::LoadTextureFromDDS(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference) {

	DirectX::ScratchImage image = {};

	//!< ddsファイルのパスを作成
	std::filesystem::path filepath = directory / reference.filepath;

	//!< ddsファイルの読み込み
	auto hr = DirectX::LoadFromDDSFile(
		filepath.generic_wstring().c_str(),
		DirectX::DDS_FLAGS_NONE,
		nullptr,
		image
	);
	ComPtrUtil::Assert(hr, L"dds texture load failed. filepath: " + filepath.generic_wstring());

	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		return image; //!< 圧縮formatの場合, 変更できないのでここで終了.
	}

	if (reference.isGenerateMipmaps) {
		image = TextureBuilder::GenerateMipmaps(image, reference.GetFilterFlags()); //!< mipmap生成後のimageに置き換える
	}

	return image;
}

DirectX::ScratchImage TextureBuilder::LoadTextureFromHDR(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference) {

	DirectX::ScratchImage image = {};

	//!< hdrファイルのパスを作成
	std::filesystem::path filepath = directory / reference.filepath;

	//!< hdrファイルの読み込み
	auto hr = DirectX::LoadFromHDRFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	ComPtrUtil::Assert(hr, L"hdr texture load failed. filepath: " + filepath.generic_wstring());

	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		return image; //!< 圧縮formatの場合, 変更できないのでここで終了.
	}

	if (reference.isGenerateMipmaps) {
		image = TextureBuilder::GenerateMipmaps(image, reference.GetFilterFlags()); //!< mipmap生成後のimageに置き換える
	}

	return image;
}

DirectX::ScratchImage TextureBuilder::LoadTextureFromTGA(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference) {

	DirectX::ScratchImage image = {};

	//!< tgaファイルのパスを作成
	std::filesystem::path filepath = directory / reference.filepath;

	//!< tgaファイルの読み込み
	auto hr = DirectX::LoadFromTGAFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	ComPtrUtil::Assert(hr, L"tga texture load failed. filepath: " + filepath.generic_wstring());

	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		return image; //!< 圧縮formatの場合, 変更できないのでここで終了.
	}

	if (reference.isGenerateMipmaps) {
		image = TextureBuilder::GenerateMipmaps(image, reference.GetFilterFlags()); //!< mipmap生成後のimageに置き換える
	}

	return image;
}

DirectX::ScratchImage TextureBuilder::LoadTextureFromWIC(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference) {

	DirectX::ScratchImage image = {};

	//!< wicファイルのパスを作成
	std::filesystem::path filepath = directory / reference.filepath;

	//!< wicファイルの読み込み時のflagsの設定
	DirectX::WIC_FLAGS wicFlags = reference.GetWICFlags();

	//!< wicファイルの読み込み
	auto hr = DirectX::LoadFromWICFile(
		filepath.generic_wstring().c_str(),
		wicFlags,
		nullptr,
		image
	);
	ComPtrUtil::Assert(hr, L"wic texture load failed. filepath: " + filepath.generic_wstring());

	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		return image; //!< 圧縮formatの場合, 変更できないのでここで終了.
	}

	if (reference.isGenerateMipmaps) {
		image = TextureBuilder::GenerateMipmaps(image, reference.GetFilterFlags()); //!< mipmap生成後のimageに置き換える
	}

	return image;
}

DirectX::ScratchImage TextureBuilder::LoadTextureFile(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference) {

	const std::filesystem::path& extension = reference.filepath.extension();

	if (extension == ".dds" || extension == ".compress") { //!< filenameが".dds"または".compress"で終わっている場合
		return TextureBuilder::LoadTextureFromDDS(directory, reference);

	} else if (extension == ".hdr") { //!< filenameが".hdr"で終わっている場合
		return TextureBuilder::LoadTextureFromHDR(directory, reference);

	} else if (extension == ".tga") { //!< filenameが".tga"で終わっている場合
		return TextureBuilder::LoadTextureFromTGA(directory, reference);

	} else {
		return TextureBuilder::LoadTextureFromWIC(directory, reference);
	}
	
}

Graphics::Resource TextureBuilder::CreateTextureResource(const std::string_view& name, const Texture::Description& description) {

	Graphics::Resource resource = Graphics::Resource::CreateTexture(
		Graphics::Core::GetDevice(),
		description.dimension,
		description.size.x, description.size.y, description.GetDepthOrArraySize(),
		description.miplevels,
		description.format,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COMMON,
		std::nullopt
	);

	resource.SetName(std::format("Asset::Texture | {}", name));
	return resource;
}

Graphics::Resource TextureBuilder::UploadResourceData(const Graphics::GraphicsCommandContext& context, const Graphics::Resource& resource, const DirectX::ScratchImage& image) {

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
	Graphics::Resource intermediate = Graphics::Resource::CreateDimensionBuffer(
		Graphics::Core::GetDevice(),
		D3D12_HEAP_TYPE_UPLOAD,
		GetRequiredIntermediateSize(resource.Get(), 0, static_cast<UINT>(subresources.size())), //!< 中間bufferのサイズを計算
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	intermediate.SetName(L"Asset::Texture | Upload Intermediate Resource");

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

void TextureBuilder::CreateDescriptor(Graphics::Descriptor& descriptor, const Graphics::Resource& resource, const Texture::Description& description) {

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
