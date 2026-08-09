#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateTypes.h"
#include "Widgets/SlateWidget.h"
#include "Widgets/SlateWindow.h"
#include "Renderer/SlateImGuiRenderer.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Time/TimePoint.h>
#include <Lib/Math/Vector2.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Application class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Slate のレイアウト/描画の駆動と入力ルーティングを統括するクラス.
	//! @note グラフィックスAPIには依存せず, ImGuiRenderer を介して描画する.
	class Application final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Application() noexcept  = default;
		~Application() noexcept = default;

		//* frame option *//

		//! @brief レイアウト(prepass)と描画を行う.
		//! @param window   ルートウィジェット.
		//! @param renderer 描画に使うレンダラ.
		//! @param time     フレームの経過時間.
		//! @note renderer は ImGuiRenderer::BeginFrame() 済みである必要がある.
		//!       フレームの開始/終了は呼び出し側(Unit)の責務であり, ここでは行わない.
		//! @note renderer をメンバに保持してはいけない. レンダラの所有者が破棄されると
		//!       保持したポインタが宙に浮き, フレーム外描画で落ちる.
		void Tick(Window& window, ImGuiRenderer* renderer, TimePointf<TimeUnit::Second> time);

		//* input option *//

		//! @brief マウス移動を配送する. ホバーの Enter/Leave とカーソル形状を更新する.
		void ProcessMouseMove(const Window& window, const PointerEvent& event);

		//! @brief マウスボタンの押下を葉から根へバブリングさせる.
		void ProcessMouseButtonDown(const Window& window, const PointerEvent& event);

		//! @brief マウスボタンの解放を配送する. キャプチャ中はキャプチャ先を優先する.
		void ProcessMouseButtonUp(const Window& window, const PointerEvent& event);

		//* cursor option *//

		//! @brief 直近のマウス位置に対応するカーソル形状を返す.
		//! @note Win32 への SetCursor は Application の責務外. 値を返すだけに留める.
		Cursor GetCurrentCursor() const { return currentCursor_; }

		//* capture option *//

		//! @brief Slate 側がマウスをキャプチャ中かどうか.
		//! @note キャプチャ中はカーソルが ImGui のパネル上へ移っても Slate へ配送し続ける必要がある.
		//!       そうしないと ImGui が入力を奪ってドラッグが途中で止まる.
		bool HasMouseCapture() const { return !mouseCaptor_.expired(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* hover parameter *//

		//!< Enter/Leave 管理用. ドッキングでツリーが差し替わっても dangling しないよう weak_ptr で保持する.
		std::weak_ptr<Widget> hoveredWidget_ = {};

		//* capture parameter *//

		std::weak_ptr<Widget> mouseCaptor_ = {}; //!< キャプチャ中のウィジェット
		Geometry captorGeometry_           = {}; //!< キャプチャ時のジオメトリ

		//* cursor parameter *//

		Cursor currentCursor_ = Cursor::Default;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* hit test methods *//

		//! @brief 絶対座標 point を含むウィジェット列を根から葉の順に集める.
		void BuildHitPath(const WidgetPointer& widget, const Geometry& geometry, const Vector2f& point, Widget::ArrangedChildren& out) const;

		//* capture methods *//

		//! @brief Reply の副作用(キャプチャの取得/解放)を適用する.
		//! @param capturedGeometry キャプチャを要求したウィジェットのジオメトリ.
		void ApplyReply(const Reply& reply, const Geometry& capturedGeometry);

		//* helper methods *//

		//! @brief ルートジオメトリ(原点はクライアント左上)を作る.
		static Geometry MakeRootGeometry(const Window& window);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
