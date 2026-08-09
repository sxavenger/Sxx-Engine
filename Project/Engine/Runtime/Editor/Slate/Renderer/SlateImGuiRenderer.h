#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../SlateTypes.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Core/Descriptor.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Time/TimePoint.h>

//* c++
#include <cstdint>
#include <list>
#include <string>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
//!< ImGui関係の構造体の前方宣言
struct ImDrawList;
struct ImGuiContext;

//!< engine関係のクラスの前方宣言
namespace Sxx::Graphics {
	class GraphicsCommandContext;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ImGuiRenderer class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief SlateUIの描画を Dear ImGui で実装するための補助クラス
	class ImGuiRenderer {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// DrawTarget enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class DrawTarget : uint8_t {
			Background, //!< Slate ウィジェットの通常描画（ImGui ウィジェットより奥）
			Window,     //!< ImGui ウィンドウ／領域の内側
			Foreground  //!< 最前面（ドッキングのオーバーレイなど）
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Descriptors structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief ImGuiの動的Descriptor確保用のユーザーデータ構造体
		struct Descriptors {
		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			const Graphics::Descriptor::Handle& Allocate();

			void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu);

			//=========================================================================================
			// public variables
			//=========================================================================================

			std::list<Graphics::Descriptor> descriptors; //!< SRV用のDescriptorリスト

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void Shutdown();

		void SetCurrentContext();

		//* frame option *//

		//! @brief ImGuiのフレームを開始する.
		//! @note プラットフォームバックエンドを使わないため、表示サイズとdeltaTimeは呼び出し側が渡す.
		void BeginFrame(const Vector2f& displaySize, TimePointf<TimeUnit::Second> deltaTime);

		//! @brief ImGuiのフレームを終了し、DrawDataをコマンドリストに記録する.
		void EndFrame(const Graphics::GraphicsCommandContext& context);

		bool IsActiveFrame() const { return isActiveFrame_; }

		//* window render option *//

		void DrawRect(const Geometry& geometry, const Color4f& color);

		void DrawRoundedRect(const Geometry& geometry, const Color4f& color, float rounding, Corner corner = CornerFlags::All);

		void DrawBorder(const Geometry& geometry, const Color4f& color, float thickness);

		void DrawLine(const Vector2f& p1, const Vector2f& p2, const Color4f& color, float thickness);

		void DrawTextA(const Vector2f& position, const std::string_view& text, const Color4f& color, float fontSize);
		void DrawTextU8(const Vector2f& position, const std::u8string_view& text, const Color4f& color, float fontSize);

		//* measure option *//

		//! @brief 文字列を描画したときの大きさを返す.
		//! @note レイアウトで使う. タブの幅はタブ名の実寸から決まるため、
		//!       描画と同じフォント・同じサイズで計測する必要がある.
		Vector2f MeasureTextA(const std::string_view& text, float fontSize);
		Vector2f MeasureTextU8(const std::u8string_view& text, float fontSize);

		void SetDrawTarget(DrawTarget target) { target_ = target; }

		//* region option *//

		//! @brief ImGuiの描画領域を開始する.
		//! @note trueを返す場合、ImGuiの描画領域が開始され、falseを返す場合、描画領域は開始されない.
		bool BeginRegion(const char* id, const Geometry& geometry);

		//! @brief ImGuiの描画領域を終了する.
		//! @note BeginRegionと必ずペアで呼ぶ.
		void EndRegion();

		//* input option *//

		//! @brief マウス位置をImGuiに通知する.
		void InjectMousePosition(const Vector2f& position);

		//! @brief マウスボタンの状態をImGuiに通知する.
		//! @param index 0:左 1:右 2:中
		void InjectMouseButton(int32_t index, bool isDown);

		//! @brief マウスホイールの回転量をImGuiに通知する.
		void InjectMouseWheel(float horizontal, float vertical);

		//! @brief ImGuiのウィジェットが操作中かどうか.
		//! @note Slate側の入力ルーティングで使う. マウス位置による判定はSlate側のヒットテストが行うため、
		//!       ここでは「ImGuiがアクティブに操作を握っているか」だけを見る.
		bool IsInteracting();

		//! @brief ImGuiがキーボード入力を要求しているかどうか.
		bool WantCaptureKeyboard();

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* ImGui objects *//

		RefPtr<ImGuiContext> context_ = nullptr; //!< Renderer専用のコンテキスト
		Descriptors descriptors_;

		//* render parameter *//

		DrawTarget target_ = DrawTarget::Background; //!< 描画ターゲットの種類

		//* runtime parameter *//

		bool isActiveFrame_ = false; //!< 描画フレームが開始されているかどうか (BeginWindow/EndWindowの間にあるかどうか)
		bool isOpenRegion_  = false; //!< 描画領域が開始されているかどうか (BeginRegion/EndRegionの間にあるかどうか)

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* initialization methods *//

		void CreateContext();

		void LoadFont();

		void InitContext();

		//* render helper methods *//

		//! @brief Slateの色(sRGB)をImGuiの描画色へ変換する.
		//! @note sRGB RTVへの書き込みでガンマが二重にかからないよう, linearへ戻してから渡す.
		static uint32_t ToDrawColor(const Color4f& color);

		RefPtr<ImDrawList> GetTargetDrawList();

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
