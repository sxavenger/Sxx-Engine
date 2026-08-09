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
	texture->image_ = TextureBuilder::LoadTextureFile(texture->GetDirectory(), reference);

	if (reference.encoding != Graphics::GetColorEncoding(texture->image_.GetMetadata().format)) {
		StreamLogger::Warning("Asset::TextureBuilder | encoding is mismatched. filepath: {}", texture->GetFilepath().generic_string()); //!< 読み込み結果とencodingが異なる場合は警告を出す
	}

	texture->description_ = Texture::Description::Parse(texture->image_.GetMetadata()); //!< descriptionの設定

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
