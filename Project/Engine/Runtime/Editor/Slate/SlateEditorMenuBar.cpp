#include "SlateEditorMenuBar.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "Widgets/SlateDecl.h"
#include "Widgets/SlateImGuiMenuBar.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EditorMenuBar class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::EditorMenuBar::SetHeight(float height) {

	height_ = height;

	//!< 既に生成済みならwidgetへ反映する. 実行中に高さを変えられるようにする.
	if (const std::shared_ptr<ImGuiMenuBar> menuBar = std::dynamic_pointer_cast<ImGuiMenuBar>(widget_)) {
		menuBar->SetHeight(height_);
	}
}

const Slate::WidgetPointer& Slate::EditorMenuBar::GetWidget() {

	if (widget_ == nullptr) {
		CreateWidget();
	}

	return widget_;
}

void Slate::EditorMenuBar::CreateWidget() {

	//!< callbackはweak_ptrを握る. menu barが破棄された後にwidgetだけ生き残っても安全にするため.
	//!< note: shared_ptrを握るとmenu barとwidgetが相互に参照して解放されない.
	std::weak_ptr<EditorMenuBar> self = weak_from_this();

	Decl<ImGuiMenuBar> menuBar;
	menuBar->Draw([self]() {
		if (const EditorMenuBarPointer bar = self.lock()) {
			bar->OnDraw();
		}
	});

	if (height_ > 0.0f) {
		menuBar->SetHeight(height_);
	}

	widget_ = menuBar.pointer;
}
