#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "ShaderBlob.h"
#include "ShaderReflection.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* lib
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <string>
#include <vector>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderCompiler class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderCompiler final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Parse(const toml::table& config);

		static Settings ParseFromConfig(const Configuration& config);

		static void Log(const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.ShaderCompiler" }; //!< 設定のパス.

		D3D_SHADER_MODEL model = kRequireShaderModel; //!< Shader Modelの機能レベル.
		bool optimize          = true;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Arguments structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Arguments {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Append(LPCWSTR argument);
		void Append(const std::wstring& argument);

		//* argument option *//

		void AppendFilepath(const std::filesystem::path& filepath);

		void AppendProfile(CompileProfile profile, D3D_SHADER_MODEL model);

		void AppendEntryPoint(const std::wstring& entryPoint);

		void AppendDefine(LPCWSTR name);

		//* compile option *//

		LPCWSTR* GetArguments() { return arguments.data(); }

		UINT32 GetCount() const { return static_cast<UINT32>(arguments.size()); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<LPCWSTR> arguments; //!< コンパイルオプションの引数
		std::list<std::wstring> lifetime; //!< 引数の寿命を管理するリスト

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / desctructor *//

	ShaderCompiler() noexcept  = default;
	~ShaderCompiler() noexcept = default;

	//* compiler option *//

	void Init(const Configuration& config, const Device& device);

	ShaderBlob Compile(
		const std::filesystem::path& filepath,
		CompileProfile profile,
		const std::wstring& entryPoint = L""
	) const;

	ShaderReflection Reflect(const ShaderBlob& blob) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dxcompiler *//

	ComPtr<IDxcUtils> utils_;
	ComPtr<IDxcCompiler3> compiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	//* settings *//

	Settings settings_;

	//* compile arguments *//

	Arguments arguments_; //!< 基本的な引数

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initialize helper methods *//

	static ComPtr<IDxcUtils> CreateUtils();

	static ComPtr<IDxcCompiler3> CreateCompiler();

	static ComPtr<IDxcIncludeHandler> CreateIncludeHandler(RefPtr<IDxcUtils> utils);

	//* compile option *//

	ShaderBlob Compile(
		const std::filesystem::path& filepath,
		RefPtr<IDxcBlobEncoding> source,
		CompileProfile profile,
		std::wstring entryPoint
	) const;

};

SXAVENGER_ENGINE_NAMESPACE_END
