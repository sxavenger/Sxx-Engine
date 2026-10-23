#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "Slate/SlateEditorPanel.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Rendering/Buffer/BufferCollection.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// ViewportPanel class
////////////////////////////////////////////////////////////////////////////////////////////
class ViewportPanel final
	: public Slate::EditorPanel {
public:

	//=====================================================================================
	// public methods
	//=====================================================================================

	//* constructor / destructor *//

	ViewportPanel();
	~ViewportPanel() noexcept override = default;

	//* panel methods *//

	void OnDraw() override;

	//* buffer option *//

	void SetBufferCollection(const Rendering::BufferCollection* buffer) { pointer_ = buffer; }

private:

	//=====================================================================================
	// private variables
	//=====================================================================================

	//* external object *//

	RefPtr<const Rendering::BufferCollection> pointer_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
