#include "StyleEditorPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "Slate/SlateIcon.h"
#include "Slate/Style/SlateStyleEditor.h"

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// StyleEditorPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

StyleEditorPanel::StyleEditorPanel() {
	SetTitle(std::format("{} Style", Slate::Icon::Settings));
}

void StyleEditorPanel::OnDraw() {
	Slate::StyleEditor::Draw();
}
