#include "SlateEditorPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "Widgets/SlateDecl.h"
#include "Widgets/SlateImGuiWidget.h"

////////////////////////////////////////////////////////////////////////////////////////////
// EditorPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::EditorPanel::SetTitle(const std::string_view& title) {

	title_ = title;

	//!< 既に生成済みならDockPanelへ反映する. tabの表示名がすぐ変わるようにする.
	if (dockPanel_ != nullptr) {
		dockPanel_->Title(title_);
	}
}

void Slate::EditorPanel::SetClosable(bool isClosable) {

	closable_ = isClosable;

	if (dockPanel_ != nullptr) {
		dockPanel_->Closable(closable_);
	}
}

void Slate::EditorPanel::SetTabColor(const Color4f& color) {

	tabColor_ = color; //!< 生成前に呼ばれても捨てないように保持する.

	if (dockPanel_ != nullptr) {
		dockPanel_->TabColor(color);
	}
}

void Slate::EditorPanel::SetDesiredSize(const Vector2f& size) {
	desiredSize_ = size;
	//!< note: 既に生成済みの場合は次に組み立て直すまで反映されない.
	//!<       希望サイズはlayoutがAutoのときだけ効くため, 実害は小さいと判断している.
}

const Slate::DockPanelPointer& Slate::EditorPanel::GetDockPanel() {

	if (dockPanel_ == nullptr) {
		CreateDockPanel();
	}

	return dockPanel_;
}

void Slate::EditorPanel::CreateDockPanel() {

	//!< callbackはweak_ptrを握る. panelが破棄された後にwidgetだけ生き残っても安全にするため.
	//!< note: shared_ptrを握るとpanelとwidgetが相互に参照して解放されない.
	std::weak_ptr<EditorPanel> self = weak_from_this();

	Decl<ImGuiWidget> content;
	content->Draw([self]() {
		if (const EditorPanelPointer panel = self.lock()) {
			panel->OnDraw();
		}
	});

	if (desiredSize_.x > 0.0f || desiredSize_.y > 0.0f) {
		content->DesiredSize(desiredSize_);
	}

	Decl<DockPanel> panel;
	panel->Title(title_);
	panel->Closable(closable_);
	panel->Content(content.pointer);

	if (tabColor_.has_value()) {
		panel->TabColor(tabColor_.value()); //!< 設定されていなければDockPanelの既定色を使う.
	}

	dockPanel_ = panel.pointer;

	OnOpen(); //!< tabが用意できたことを通知する.
}
