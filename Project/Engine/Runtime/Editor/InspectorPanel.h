#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "Slate/SlateEditorPanel.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/World/Entity/EntityHandle.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// InspectorPanel class
////////////////////////////////////////////////////////////////////////////////////////////
class InspectorPanel final
	: public Slate::EditorPanel {
public:

	//=====================================================================================
	// public methods
	//=====================================================================================

	//* constructor / destructor *//

	InspectorPanel();
	~InspectorPanel() noexcept override = default;

	//* panel methods *//

	void OnDraw() override;

	//* inspector methods *//

	void SetSelectedEntity(const World::EntityHandle& handle) { handle_ = handle; }

	World::EntityHandle GetSelectedEntity() const { return handle_; }

private:

	//=====================================================================================
	// private variables
	//=====================================================================================

	World::EntityHandle handle_;

};

SXAVENGER_ENGINE_NAMESPACE_END
