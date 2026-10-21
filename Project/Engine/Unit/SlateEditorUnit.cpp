#include "SlateEditorUnit.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [editor/slate]
#include <Runtime/Editor/Slate/Widgets/SlateDecl.h>
#include <Runtime/Editor/Slate/Docking/SlateDockTabStack.h>
#include <Runtime/Editor/Slate/Docking/SlateSplitter.h>

//* engine [editor]
#include <Runtime/Editor/CaptureMainMenu.h>
#include <Runtime/Editor/EditorTestPanel.h>
#include <Runtime/Editor/StyleEditorPanel.h>

//* imgui
#include <imgui.h>

//* c++
#include <algorithm>
#include <format>
#include <string>

//-----------------------------------------------------------------------------------------
// constant
//-----------------------------------------------------------------------------------------

namespace {

	//!< design.md §3 の余白トークン. (4px grid)
	constexpr float kPadS = 4.0f;
	constexpr float kPadM = 8.0f;
	constexpr float kPadL = 16.0f;

}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

SlateEditorUnit::SlateEditorUnit() {

	//!< 既定のpanel. 生成はGPUにもImGuiにも依存しないため, Push時点で作っておく.
	//!< note: layoutはBuildMainLayoutでこれらを並べるだけにする.
	panels_.push_back(std::make_shared<Editor::EditorTestPanel>());
	panels_.push_back(std::make_shared<Editor::StyleEditorPanel>());
}

void SlateEditorUnit::OnInitEditor() {

	//!< 既定のmain menu. 追加するなら AddMainMenu<T>() を呼ぶ.
	AddMainMenu<Editor::CaptureMainMenu>();
}

Editor::Slate::WidgetPointer SlateEditorUnit::BuildMainLayout() {

	//!< main windowのlayout. splitterで左右に分割し, それぞれにtab stackを置く.
	//!< note: アイコンは専用APIではなくtitleの文字列に埋め込む. 位置も個数も自由に決められる.

	Editor::Slate::Decl<Editor::Slate::DockTabStack> left;
	{
		//!< EditorPanelを継承したpanelの例. titleとtab色はpanel側のconstructorで決まる.
		//!< note: panelはconstructorで生成済み. layoutを組んでいる途中でdockRootがまだ無いため
		//!<       AddPanel()は使えず, ここでは直接tab stackへ入れる.
		if (const std::shared_ptr<Editor::EditorTestPanel> test = GetPanel<Editor::EditorTestPanel>()) {
			left->AddPanel(test->GetDockPanel());
		}
	}

	Editor::Slate::Decl<Editor::Slate::DockTabStack> right;
	{
		//!< Styleの調整タブ. EditorPanelを継承して作る例になっている.
		//!< note: panelはconstructorで生成済み. layoutを組んでいる途中でdockRootがまだ無いため
		//!<       AddPanel()は使えず, ここでは直接tab stackへ入れる.
		if (const std::shared_ptr<Editor::StyleEditorPanel> style = GetPanel<Editor::StyleEditorPanel>()) {
			right->AddPanel(style->GetDockPanel());
		}
	}

	//!< 左35 : 右65のsplitterで分割する. (design.md §2.3)
	return std::make_shared<Editor::Slate::Splitter>(
		Editor::Slate::Splitter::Orientation::Horizontal, left.pointer, right.pointer, kListPaneRatio
	);
}

void SlateEditorUnit::DrawStatusBar() {

	//!< 縦中央に寄せる. 操作要素は置かない(Text系のみ).
	ImGui::SetCursorPosY(std::max(0.0f, (ImGui::GetWindowHeight() - ImGui::GetTextLineHeight()) * 0.5f));

	int32_t openCount = 0;

	for (const Editor::Slate::EditorPanelPointer& panel : GetPanels()) {
		if (panel != nullptr && IsPanelOpen(panel)) {
			++openCount;
		}
	}

	const int32_t totalCount  = static_cast<int32_t>(GetPanels().size());
	const int32_t closedCount = totalCount - openCount;

	ImGui::Text("%d panels", openCount);

	if (closedCount > 0) {
		ImGui::SameLine(0.0f, kPadS);
		ImGui::TextDisabled("(%d closed)", closedCount);
	}

	ImGui::SameLine(0.0f, kPadL);
	ImGui::TextDisabled("%d windows", static_cast<int32_t>(GetEditorWindows().size()));

	//!< 右端: frame timeとfps. CalcTextSizeで実寸を測ってから右揃えにする.
	const float fps = ImGui::GetIO().Framerate;
	const std::string text = std::format("{:.2f} ms  {:.0f} fps", (fps > 0.0f) ? 1000.0f / fps : 0.0f, fps);

	ImGui::SameLine(std::max(0.0f, ImGui::GetWindowWidth() - ImGui::CalcTextSize(text.c_str()).x - kPadM));
	ImGui::TextDisabled("%s", text.c_str()); //!< std::stringを渡すため書式文字列インジェクションを避ける.
}
