#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Flag/Flag.h>
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Color4.h>

//* c++
#include <cstdint>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class Widget;

	////////////////////////////////////////////////////////////////////////////////////////////
	// HAlign enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HAlign : uint8_t {
		Fill,
		Left,
		Center,
		Right
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// VAlign enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class VAlign : uint8_t {
		Fill,
		Top,
		Center,
		Bottom
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Visibility enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Visibility : uint8_t {
		Visible,
		Hidden,
		Collapsed,
		HitTestInvisible
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Cursor enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief マウスカーソルの種類
	enum class Cursor : uint8_t {
		Default,
		ResizeLeftRight,
		ResizeUpDown,
		Hand,
		TextEdit
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Corner enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 角丸を掛ける角の指定
	enum class CornerFlags : uint8_t{
		None        = 0,
		TopLeft     = 1 << 0,
		TopRight    = 1 << 1,
		BottomLeft  = 1 << 2,
		BottomRight = 1 << 3,

		Top         = TopLeft | TopRight,
		Bottom      = BottomLeft | BottomRight,
		All         = Top | Bottom
	};
	using Corner = FlagEnum<CornerFlags>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Geometry structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 絶対座標での配置
	struct Geometry {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 子要素へオフセット＋サイズを与えた新しいジオメトリを作る
		Geometry MakeChild(const Vector2f& offset, const Vector2f& size) const;

		//! @brief 絶対座標での点がこのジオメトリ内に含まれるかを判定する
		bool ContainsAbsolute(const Vector2f& p) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f absolutePosition{}; //!< ウィンドウ左上を原点とした絶対座標
		Vector2f localSize{};        //!< このウィジェットに割り当てられたサイズ
		float scale = 1.0f;          //!< DPI 等のレイアウトスケール

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Margin structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Margin {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		constexpr Margin() = default;
		constexpr Margin(float uniform) : left(uniform), top(uniform), right(uniform), bottom(uniform) {}
		constexpr Margin(float horizontal, float vertical) : left(horizontal), top(vertical), right(horizontal), bottom(vertical) {}

		constexpr float Horizontal() const { return left + right; }

		constexpr float Vertical() const { return top + bottom; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		float left   = 0;
		float top    = 0;
		float right  = 0;
		float bottom = 0;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reply class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 入力結果
	class Reply {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool IsHandled() const { return isHandled_; }

		//!< マウスキャプチャを要求
		Reply& CaptureMouse(std::shared_ptr<Widget> target) { captureTarget_ = target; return *this; }

		const std::shared_ptr<Widget>& GetMouseCaptor() const { return captureTarget_; }

		//!< キャプチャ解放を要求
		Reply& ReleaseMouseCapture() { isReleaseCapture_ = true; return *this; }

		bool ShouldReleaseCapture() const { return isReleaseCapture_; }

		//* static methods *//

		static Reply Handled() { return Reply(true); }

		static Reply Unhandled() { return Reply(false); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		bool isHandled_        = false;
		bool isReleaseCapture_ = false;

		std::shared_ptr<Widget> captureTarget_ = nullptr;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* constructor *//

		explicit Reply(bool handled) : isHandled_(handled) {}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// PointerEvent structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PointerEvent {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Button enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Button : uint8_t {
			Left,
			Right,
			Middle
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Modifier enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Modifier : uint8_t {
			None  = 0,
			Ctrl  = 1 << 0,
			Shift = 1 << 1,
			Alt   = 1 << 2
		};

	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f screenPosition     = {};
		Button button               = Button::Left;
		FlagEnum<Modifier> modifier = Modifier::None;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
