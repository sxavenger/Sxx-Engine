#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* engine [editor]
#include <Runtime/Editor/Slate/SlateEditorBase.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Slate editorのuser側の設定を書くUnit.
//! @note panelの登録, main windowのlayout, main menu, statusBarの中身をここに書く.
//!       window / docking / chrome / 入力などの仕組みはEditor::Slate::EditorBase (Runtime/Editor/Slate/SlateEditorBase.h) にある.
class SlateEditorUnit final
	: public Editor::Slate::EditorBase {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	//! @brief 既定のpanelを生成する.
	//! @note 他Unitの Initialize から GetPanel<T>() で取得できるように, layoutより先に生成する.
	//!       (InitEditorは Priority::Lowest のため, 他Unitの Initialize より後に走る)
	SlateEditorUnit();

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* editor hook methods *//

	void OnInitEditor() override;

	Editor::Slate::WidgetPointer BuildMainLayout() override;

	void DrawStatusBar() override;

};

SXAVENGER_ENGINE_NAMESPACE_END
