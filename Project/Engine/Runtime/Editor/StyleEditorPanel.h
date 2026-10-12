#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "Slate/SlateEditorPanel.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// StyleEditorPanel class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief EditorPanelを継承したpanelの実例. Styleの調整タブ.
//! @note OnDraw()の中はImGuiの呼び出しを並べるだけでよい. Begin / Endは基底が面倒を見る.
class StyleEditorPanel final
	: public Slate::EditorPanel {
public:

	//=====================================================================================
	// public methods
	//=====================================================================================

	//* constructor / destructor *//

	StyleEditorPanel();
	~StyleEditorPanel() noexcept override = default;

	//* panel methods *//

	void OnDraw() override;

};

SXAVENGER_ENGINE_NAMESPACE_END
