#include "MeshletBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshletBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshletBuffer::SetName(const std::string_view& name) const {
	meshlets.SetName(std::format("MeshletBuffer | {} | meshlets", name));
	vertexIndices.SetName(std::format("MeshletBuffer | {} | vertex indices", name));
	triangles.SetName(std::format("MeshletBuffer | {} | triangles", name));
	bounds.SetName(std::format("MeshletBuffer | {} | bounds", name));
}

void MeshletBuffer::SetName(const std::wstring_view& name) const {
	meshlets.SetName(std::format(L"MeshletBuffer | {} | meshlets", name));
	vertexIndices.SetName(std::format(L"MeshletBuffer | {} | vertex indices", name));
	triangles.SetName(std::format(L"MeshletBuffer | {} | triangles", name));
	bounds.SetName(std::format(L"MeshletBuffer | {} | bounds", name));
}
