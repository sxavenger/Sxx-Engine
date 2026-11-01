#include "ViewportPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "SxGui/SxGui.h"

//* engine
#include <Runtime/Rendering/Buffer/MainBuffer.h>

//* imgui
#include <imgui.h>

//* c++
#include <algorithm>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// ViewportPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

ViewportPanel::ViewportPanel() {
	Slate::EditorPanel::SetTitle(std::format("{} Viewport", Slate::Icon::Window));
	Slate::EditorPanel::SetTabColor(Color4f::Blue());
}

void ViewportPanel::OnDraw() {

	Vector2f region = SxGui::GetContentRegionAvail();

	if (pointer_ == nullptr) {

		static const std::string_view kDisplayText = "No Viewport";

		// 中心に表示する
		Vector2f textSize = SxGui::CalcTextSize(kDisplayText.data());

		SxGui::SetCursorPos((region - textSize) / 2.0f);
		SxGui::Text(kDisplayText.data());
		
		return; //!< バッファが無いので描画しない.
	}

	const Rendering::MainBuffer& buffer = pointer_->GetInstance<Rendering::MainBuffer>();
	const Rendering::RenderTargetTexture& texture = buffer.GetBuffer(Rendering::MainBuffer::Layout::Scene);
	// TODO: 表示変更できるように設計

	//!< アスペクト比を維持した表示領域を計算する
	SxGui::ContentRect rect = SxGui::CalcAspectFitRect(static_cast<Vector2f>(texture.GetResolution()), region);

	{ //!< 透明な範囲内にチェッカーを表示する

		//!< チェッカーのサイズと色の設定
		static const float kCellSize     = 8.0f;
		static const Color4f kColorDark  = Slate::ConvertToLinearColor(Color4f(0x1F1F1FFF));
		static const Color4f kColorLight = Slate::ConvertToLinearColor(Color4f(0x292929FF));

		SxGui::DrawList drawList = SxGui::DrawList::GetCurrentDrawList(); //!< DrawListを取得する

		const Vector2f rectMin = SxGui::GetCursorScreenPos() + rect.cursol;
		const Vector2f rectMax = rectMin + rect.size;

		//!< 全体を暗い色で塗り, 明るいマスだけを重ねる
		drawList.RectFilled(rectMin, rectMax, kColorDark);

		const int32_t countX = static_cast<int32_t>(std::ceil(rect.size.x / kCellSize));
		const int32_t countY = static_cast<int32_t>(std::ceil(rect.size.y / kCellSize));

		for (int32_t y = 0; y < countY; ++y) {
			for (int32_t x = 0; x < countX; ++x) {

				if ((x + y) % 2 == 0) {
					continue; //!< 暗いマスは塗り済み
				}

				const Vector2f cellMin = { rectMin.x + x * kCellSize, rectMin.y + y * kCellSize };
				const Vector2f cellMax = { std::min(cellMin.x + kCellSize, rectMax.x), std::min(cellMin.y + kCellSize, rectMax.y) };

				drawList.RectFilled(cellMin, cellMax, kColorLight);
			}
		}
	}

	{ //!< 範囲内に画像を表示する
		SxGui::ImageContentRect(texture.GetDescriptorSRV(), rect);
	}
}
