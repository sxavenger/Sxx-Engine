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
// HierarchyPanel class
////////////////////////////////////////////////////////////////////////////////////////////
class HierarchyPanel final
	: public Slate::EditorPanel {
public:

	//=====================================================================================
	// public methods
	//=====================================================================================

	//* constructor / destructor *//

	HierarchyPanel();
	~HierarchyPanel() noexcept override = default;

	//* panel methods *//

	void OnDraw() override;

};

SXAVENGER_ENGINE_NAMESPACE_END
