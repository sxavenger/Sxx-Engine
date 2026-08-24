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

//* DirectXTex
#include <DirectXTex/Common/d3dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureBuilder::Build(std::shared_ptr<Texture>& texture) {

	//!< ファイルパスの取得
	const std::filesystem::path& filepath = texture->GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		STREAM_LOG_ERROR("Asset::TextureBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< Assetファイルの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const TextureMetadata& metadata = texture->metadata_ = TextureMetadata::Deserialize(node["metadata"]);

	if (metadata.GetType() == MetadataType::Unknown) {
		STREAM_LOG_ERROR("Asset::TextureBuilder | texture metadata type is unknown. filepath: {}", filepath.generic_string());
		return; //!< metadataがUnknownの場合は処理を終了
	}

	const TextureMetadata::ReferenceData& reference = metadata.GetReferenceData(); //!< 参照データの取得

	//!< 画像ファイルの読み込み
	const DirectX::ScratchImage& image = texture->image_ = TextureBuilder::LoadTextureFile(texture->GetDirectory(), reference);

	if (reference.encoding != Graphics::GetColorEncoding(image.GetMetadata().format)) {
		STREAM_LOG_WARNING("Asset::TextureBuilder | encoding is mismatched. filepath: {}", filepath.generic_string()); //!< 読み込み結果とencodingが異なる場合は警告を出す
	}

	texture->description_ = Texture::Description::Parse(image.GetMetadata()); //!< descriptionの設定

	STREAM_LOG_INFO("Asset::TextureBuilder | texture build completed. name: {}", texture->GetName());
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
	std::filesystem::path filepath = (directory / reference.filepath).lexically_normal();

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
	std::filesystem::path filepath = (directory / reference.filepath).lexically_normal();

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
	std::filesystem::path filepath = (directory / reference.filepath).lexically_normal();

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
	std::filesystem::path filepath = (directory / reference.filepath).lexically_normal();

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
