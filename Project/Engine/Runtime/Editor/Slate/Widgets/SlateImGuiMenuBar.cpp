#include "SlateImGuiMenuBar.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* imgui
#include <imgui.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiMenuBar class methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector2f Slate::ImGuiMenuBar::ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const {
	return Vector2f{ 0.0f, (height_ > 0.0f) ? height_ : Height() };
}

int32_t Slate::ImGuiMenuBar::OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	if (renderer == nullptr) {
		return layer;
	}

	//!< 背景と下端の区切り線は Slate 側で描く(ImGui 側のメニューバー背景は透過させる).
	//!< 色は都度参照する(静的変数に写すとスタイル変更に追従しない).
	renderer->DrawRect(geometry, Style::GetStyle().colors.menuBar);

	const float bottom = geometry.absolutePosition.y + geometry.localSize.y - 0.5f;
	renderer->DrawLine(
		{ geometry.absolutePosition.x, bottom },
		{ geometry.absolutePosition.x + geometry.localSize.x, bottom },
		Style::GetStyle().colors.border, Style::GetStyle().metrics.borderThin
	);

	if (draw_ == nullptr) {
		return layer + 1;
	}

	//!< フレーム外・サイズ 0 では ImGui ウィンドウを開かない.
	if (!renderer->IsActiveFrame() || geometry.localSize.x <= 0.0f || geometry.localSize.y <= 0.0f) {
		return layer + 1;
	}

	//!< ImGuiRenderer::BeginRegion のフラグは固定で ImGuiWindowFlags_MenuBar を含まないため,
	//!< ImGui::BeginMenuBar() が使えない. メニューバー専用の領域はここで直接開く.
	//!< TODO: ImGuiRenderer 側に BeginMenuBarRegion / EndMenuBarRegion を用意して移す.
	renderer->SetCurrentContext();

	if (id_.empty()) {
		id_ = std::format("##slate_menubar_{:016X}", reinterpret_cast<uintptr_t>(this));
	}

	ImGui::SetNextWindowPos(ImVec2(geometry.absolutePosition.x, geometry.absolutePosition.y));
	ImGui::SetNextWindowSize(ImVec2(geometry.localSize.x, geometry.localSize.y));

	static const ImGuiWindowFlags kFlags
		= ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_MenuBar;

	//!< 行の高さちょうどにバー項目が収まるよう上下の余白を決める.
	//!< FramePadding は "File" 等のバー項目の高さを決めるため, BeginMenuBar 以降も効かせ続ける.
	const float paddingY = (geometry.localSize.y - Style::GetStyle().metrics.fontBody) * 0.5f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	//!< WindowMinSize の既定は 32x32 で, ImGui は SetNextWindowSize の値をこれで下から丸める.
	//!< メニュー行は 22px しかないため, そのままだと 32px のウィンドウになって
	//!< 下のタブバーへはみ出し, ホバーまで奪ってしまう.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(1.0f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Style::GetStyle().metrics.marginL, (paddingY > 0.0f) ? paddingY : 0.0f));

	//!< 背景は上で Slate 側が描いたので, ImGui 側は透過させる.
	ImGui::PushStyleColor(ImGuiCol_WindowBg,  IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 0));

	//!< 次の 2 つは Begin() の間だけ効かせ, 直後に戻す.
	//!<   WindowPadding   : メニュー行に余白は持てない. ただし積んだままだと BeginMenu() の
	//!<                     ポップアップが「そのときの style」を見て開くため, 中身も余白ゼロになる.
	//!<   FrameBorderSize : ImGui は「メニューバーの下端がウィンドウ下端より上」のとき
	//!<                     バーの下に枠線を引く. これが Slate 側のタブ上端の色帯を 1px 潰す.
	//!<                     区切り線は上で Slate 側が既に描いている.
	//!< Begin() は style の値を window 側へ写し取ってから進むため, 直後に戻してもこの
	//!< ウィンドウ自身には影響しない.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	ImGui::Begin(id_.c_str(), nullptr, kFlags);
	ImGui::PopStyleVar(2); //!< ドロップダウンには既定の余白・枠線を使わせる.

	//!< Begin() を呼んだ場合, 戻り値によらず End() を呼ぶ.
	if (ImGui::BeginMenuBar()) {
		draw_();
		ImGui::EndMenuBar();
	}

	ImGui::End();

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(4);

	return layer + 1;
}
