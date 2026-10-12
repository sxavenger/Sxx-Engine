#include "SlateChromeBand.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* imgui
#include <imgui.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// ChromeBand class methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector2f Slate::ChromeBand::ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const {
	return Vector2f{ 0.0f, height_ };
}

int32_t Slate::ChromeBand::OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {

	if (renderer == nullptr) {
		return layer;
	}

	//!< 背景と区切り線はSlate側で描く. 色は都度参照する(静的変数に写すとスタイル変更に追従しない).
	renderer->DrawRect(geometry, Style::GetStyle().colors.panel);

	const float y = (separator_ == Separator::Top)
		? geometry.absolutePosition.y + 0.5f
		: geometry.absolutePosition.y + geometry.localSize.y - 0.5f;

	renderer->DrawLine(
		{ geometry.absolutePosition.x, y },
		{ geometry.absolutePosition.x + geometry.localSize.x, y },
		Style::GetStyle().colors.border, Style::GetStyle().metrics.borderThin
	);

	//!< フレーム外・サイズ0・callback未設定ではImGuiウィンドウを開かない.
	if (draw_ == nullptr || !renderer->IsActiveFrame() || geometry.localSize.x <= 0.0f || geometry.localSize.y <= 0.0f) {
		return layer + 1;
	}

	renderer->SetCurrentContext();

	if (id_.empty()) {
		id_ = std::format("##slate_band_{:016X}", reinterpret_cast<uintptr_t>(this));
	}

	ImGui::SetNextWindowPos(ImVec2(geometry.absolutePosition.x, geometry.absolutePosition.y));
	ImGui::SetNextWindowSize(ImVec2(geometry.localSize.x, geometry.localSize.y));

	static const ImGuiWindowFlags kFlags
		= ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse
		| ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	//!< WindowMinSizeの既定は32x32で, SetNextWindowSizeの値をこれで下から丸める.
	//!< pushしないとstatusBar(24px)が本体へはみ出してホバーを奪ってしまう.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(1.0f, 1.0f));

	//!< Begin()の間だけ効かせ, 直後に戻す. 積んだままだとポップアップにも余白ゼロが波及するため.
	//!< note: 左右余白はStyleのpaddingMを使う. (Unit側の定数kPadM(8)から置き換えた. 既定値は同じ8)
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(Style::GetStyle().metrics.paddingM, 0.0f));
	const bool isOpen = ImGui::Begin(id_.c_str(), nullptr, kFlags);
	ImGui::PopStyleVar(1);

	//!< Begin()を呼んだ場合, 戻り値によらずEnd()を呼ぶ.
	if (isOpen) {
		draw_();
	}

	ImGui::End();

	ImGui::PopStyleVar(3);

	return layer + 1;
}
