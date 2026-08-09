#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// StyleEditor class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Slate のスタイル(Style::Colors / Style::Metrics)を ImGui の GUI で調整するパネル.
	//! @note 編集の対象は StyleIO の項目表(GetColorEntries / GetMetricEntries)そのもの.
	//!       項目を増やすときは SlateStyleIO.cpp の表に1行足すだけでよく, ここは触らない.
	//! @note Style::GetStyle() の実体を直接書き換えるため, 編集は即座に画面へ反映される.
	//!       ImGui 自身の style は Slate の style から作られた写しなので,
	//!       書き換えたフレームの終わりに ImGuiStyleIO::SetCurrentStyle() で写し直す.
	//! @note ImGui 自身の style を直接いじりたい場合は ImGui::ShowStyleEditor() を使う.
	//!       こちらは Slate の外枠(タブ・仕切り・タイトルバー)のスタイル専用.
	//!
	//! 機能:
	//!   - Colors           … 色を分類ごとに編集(16進表示つき)
	//!   - Sizes            … 余白・高さなどをスライダで編集
	//!   - Filter           … 名前・分類・説明で絞り込み
	//!   - Reset to default … すべて既定値へ戻す
	//!   - Save Ref/Revert  … 控えた状態と往復する
	//!   - Copy JSON        … 現在のスタイルを JSON でクリップボードへ
	//!
	//! 使用例(DockPanel の中身にする):
	//!   Decl<ImGuiWidget> content;
	//!   content->Draw([]{ StyleEditor::Draw(); });
	//!
	//!   Decl<DockPanel> panel;
	//!   panel->Title("Style");
	//!   panel->Content(content.pointer);
	class StyleEditor final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* editor option *//

		//! @brief スタイルエディタの中身を描画する.
		//! @note ImGui のフレーム内で呼ぶこと(ImGuiWidget::Draw() のコールバックの中でよい).
		//!       ImGui::Begin / End はこの関数の外側の責務なので, ここでは呼ばない.
		static void Draw();

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
