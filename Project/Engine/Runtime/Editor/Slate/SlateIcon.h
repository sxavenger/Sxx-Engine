#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>
#include <array>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Icon enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//!< [Google Material Symbols](https://fonts.google.com/icons)
	enum class Icon : uint32_t {
		None = 0x0,

		Warning              = 0xE002,
		Error                = 0xE000,
		Home                 = 0xE88A,
		DeployedCode         = 0xF720,
		DeployedCodeUpdate   = 0xF5F4,
		ChessPawn            = 0xF3B6,
		AccountTree          = 0xE97A,
		Menu                 = 0xE5D2,
		MenuOpen             = 0xE9BD,
		Layers               = 0xE53B,
		Window               = 0xF088,
		Folder               = 0xE2C7,
		Files                = 0xEA85,
		Terminal             = 0xEB8E,
		Timer                = 0xE425,
		Schedule             = 0xE8B5,
		Stack                = 0xF500,
		Visibility           = 0xE8F4,
		VisibilityOff        = 0xE8F5,
		Delete               = 0xE872,
		Equal                = 0xF77B,
		Nearby               = 0xE6B7,
		NearbyError          = 0xF03B,
		Dialog               = 0xE99F,
		ShortText            = 0xE261,
		Videocam             = 0xE04B,
		LightMode            = 0xE518,
		LightBlob            = 0xE0F0,
		Light                = 0xF02A,
		BacklightHigh        = 0xF7ED,
		Search               = 0xE8B6,
		Label                = 0xE892,
		ControlCamera        = 0xE074,
		Capture              = 0xF727,
		Hourglass            = 0xEBFF,
		Texture              = 0xE421,
		FlipToBack           = 0xE882,
		Settings             = 0xE8B8,
		CheckBox             = 0xE834,
		CheckBoxOutlineBlank = 0xE835,
		Close                = 0xE5CD,

		Cube             = DeployedCode,
		CubeUpdate       = DeployedCodeUpdate,
		Hierarchy        = AccountTree,
		Camera           = Videocam,
		DirectionalLight = LightMode,
		PointLight       = LightBlob,
		SpotLight        = Light,
		RectLight        = BacklightHigh,
		CheckBoxOutline  = CheckBoxOutlineBlank,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// IconGlyph structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Icon の値を UTF-8 文字列に変換するための構造体
	struct IconGlyph {
	public:

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		using Data = std::array<char8_t, 5>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		std::u8string_view Get() const noexcept { return std::u8string_view(data.data()); }

		static Data Encode(Icon icon) noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Data data{};

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief UTF-8の継続バイトを生成
		static char8_t Continuation(uint32_t value) noexcept;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
