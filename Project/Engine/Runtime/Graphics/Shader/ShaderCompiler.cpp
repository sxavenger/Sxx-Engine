#include "ShaderCompiler.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [ShaderCompiler] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderCompiler::Settings::Parse(const toml::table& config) {
	TomlReader<D3D_SHADER_MODEL>::Find(config, "model", model);
	TomlReader<bool>::Find(config, "optimize", optimize);
}

ShaderCompiler::Settings ShaderCompiler::Settings::ParseFromConfig(const Configuration& config) {
	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Graphics::ShaderCompiler::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.
	
	return settings;
}

void ShaderCompiler::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Graphics::ShaderCompiler::Settings | model: {}", settings.model);
	StreamLogger::Debug("Graphics::ShaderCompiler::Settings | optimize: {}", settings.optimize);
}

////////////////////////////////////////////////////////////////////////////////////////////
// [ShaderCompiler] Arguments structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderCompiler::Arguments::Append(LPCWSTR argument) {
	arguments.emplace_back(argument); //!< 引数を追加
}

void ShaderCompiler::Arguments::Append(const std::wstring& argument) {
	lifetime.emplace_back(argument); //!< 引数の寿命を管理するためにリストに追加
	arguments.emplace_back(lifetime.back().c_str());
}

void ShaderCompiler::Arguments::AppendFilepath(const std::filesystem::path& filepath) {
	Append(filepath.generic_wstring()); //!< ファイルパスの引数を追加
}

void ShaderCompiler::Arguments::AppendProfile(CompileProfile profile, D3D_SHADER_MODEL model) {

	uint8_t major = (model >> 4) & 0xF; //!< 上位4bitがmajor
	uint8_t minor = model & 0xF;        //!< 下位4bitがminor
	std::wstring tier = std::format(L"{}_{}", major, minor); //!< tierを文字列に変換 (ex."6_5")

	Append(L"-T"); //!< プロフィールの引数を追加
	Append(std::format(L"{}_{}", GetCompileShaderStage(profile), tier)); //!< プロフィールの引数を追加 (ex."vs_6_5")
}

void ShaderCompiler::Arguments::AppendEntryPoint(const std::wstring& entryPoint) {
	Append(L"-E"); //!< entry pointの引数を追加
	Append(entryPoint.c_str());
}

void ShaderCompiler::Arguments::AppendDefine(LPCWSTR name) {
	Append(L"-D"); //!< defineの引数を追加
	Append(name);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderCompiler::Init(const Configuration& config, const Device& device) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ.

	//!< dxcompilerの初期化
	utils_          = ShaderCompiler::CreateUtils();
	compiler_       = ShaderCompiler::CreateCompiler();
	includeHandler_ = ShaderCompiler::CreateIncludeHandler(utils_.Get());

	//!< 共通defineの設定
	arguments_.Append(L"-Zpr"); //!< メモリレイアウトは行優先
	
	if (device.GetSupport().raytracingTier >= D3D12_RAYTRACING_TIER_1_1) {
		arguments_.AppendDefine(L"SUPPORT_INLINE_RAYTRACING"); //!< inline raytracingをサポートしていることをdefineで伝える
	}

	StreamLogger::Info("Graphics::ShaderCompiler | complete initialize.");
}

ShaderBlob ShaderCompiler::Compile(
	const std::filesystem::path& filepath,
	CompileProfile profile,
	const std::wstring& entryPoint) {

	//!< hlslを読み込む
	ComPtr<IDxcBlobEncoding> source;
	auto hr = utils_->LoadFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		&source
	);
	ComPtrUtil::Assert(hr, std::format(L"dxcompiler load file failed. filepath: {}", filepath.generic_wstring()));

	return Compile(filepath, source.Get(), profile, entryPoint);
}

ShaderReflection ShaderCompiler::Reflect(const ShaderBlob& blob) {

	DxcBuffer buffer = blob.GetBuffer(ShaderBlob::CodePage::ACP); //!< バイナリの内容を設定.

	ComPtr<ID3D12ShaderReflection> reflection;
	auto hr = utils_->CreateReflection(
		&buffer,
		IID_PPV_ARGS(&reflection)
	);
	ComPtrUtil::Assert(hr, L"dxcompiler reflection create failed.");

	return reflection;
}

ComPtr<IDxcUtils> ShaderCompiler::CreateUtils() {

	ComPtr<IDxcUtils> utils;

	auto hr = DxcCreateInstance(
		CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, L"dxcompiler utils create failed.");

	return utils;
}

ComPtr<IDxcCompiler3> ShaderCompiler::CreateCompiler() {

	ComPtr<IDxcCompiler3> compiler;

	auto hr = DxcCreateInstance(
		CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, L"dxcompiler compiler create failed.");

	return compiler;
	
}

ComPtr<IDxcIncludeHandler> ShaderCompiler::CreateIncludeHandler(RefPtr<IDxcUtils> utils) {

	ComPtr<IDxcIncludeHandler> handler;

	auto hr = utils->CreateDefaultIncludeHandler(
		handler.GetAddressOf()
	);
	ComPtrUtil::Assert(hr, L"dxcompiler include handler create failed.");

	return handler;
}

ShaderBlob ShaderCompiler::Compile(
	const std::filesystem::path& filepath,
	RefPtr<IDxcBlobEncoding> source,
	CompileProfile profile,
	std::wstring entryPoint) {

	//!< sourceの内容を設定する
	DxcBuffer buffer = {};
	buffer.Ptr      = source->GetBufferPointer();
	buffer.Size     = source->GetBufferSize();
	buffer.Encoding = DXC_CP_UTF8;

	Arguments arguments = arguments_; //!< 基本的な引数をコピー
	arguments.AppendFilepath(filepath); //!< ファイルパスの引数を追加
	arguments.AppendProfile(profile, settings_.model); //!< プロフィールの引数を追加

	//!< entry pointの引数を追加
	if (!entryPoint.empty()) {
		arguments.AppendEntryPoint(entryPoint);
	}

#ifdef DEVELOPMENT
	//!< 最適化の有無を設定
	if (settings_.optimize) {
		arguments.Append(L"-O3"); //!< 最適化を最大にする

	} else {
		arguments.Append(L"-Od"); //!< 最適化を無効にする
	}

	arguments.Append(L"-Zi");           //!< デバッグ情報を生成
	arguments.Append(L"-Qembed_debug"); //!< デバッグ情報をシェーダバイナリに埋め込む

#else
	arguments.Append(L"-O3"); //!< 最適化を最大にする
#endif

	//!< shaderのコンパイル
	ComPtr<IDxcResult> result;
	auto hr = compiler_->Compile(
		&buffer,
		arguments.GetArguments(),
		arguments.GetCount(),
		includeHandler_.Get(),
		IID_PPV_ARGS(result.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, std::format(L"shader compile failed. filepath: {}", filepath.generic_wstring()));

	//!< コンパイルエラーがある場合はログに出力する
	ComPtr<IDxcBlobUtf8> error;
	hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(error.GetAddressOf()), nullptr);
	ComPtrUtil::Assert(hr, std::format(L"shader compile error output failed. filepath: {}", filepath.generic_wstring()));

	if (error != nullptr && error->GetStringLength() != 0) {
		std::string_view message(error->GetStringPointer(), error->GetStringLength());
		StreamLogger::Exception(
			std::format("shader compile failed. filepath: {}", filepath.generic_string()),
			message
		);
	}

	//!< コンパイル結果からバイナリを取得する
	ComPtr<IDxcBlob> blob;
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr);
	ComPtrUtil::Assert(hr, std::format(L"shader compile object output failed. filepath: {}", filepath.generic_wstring()));

	StreamLogger::Info(
		std::format("shader compile success. filepath: {}", filepath.generic_string())
	);
	return ShaderBlob(blob, profile);
}
