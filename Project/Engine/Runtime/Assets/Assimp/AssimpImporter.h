#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assimp
#include "AssimpOption.h"
#include "AssimpMesh.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Flag/Flag.h>

//* assimp
#include <assimp/Importer.hpp>

//* c++
#include <memory>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpImporter class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpImporter final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpImporter() noexcept = default;

	//* importer option *//

	std::string_view GetErrorString() const;

	//* scene option *//

	bool HasScene() const;

	uint32_t GetMeshCount() const;

	AssimpMesh GetMesh(uint32_t index) const;

	// TODO: Material/Animation関係の関数の追加.

	//* operator [copy / move] <AssimpImporter> *//

	AssimpImporter(const AssimpImporter&) noexcept            = default;
	AssimpImporter& operator=(const AssimpImporter&) noexcept = default;

	AssimpImporter(AssimpImporter&&) noexcept            = default;
	AssimpImporter& operator=(AssimpImporter&&) noexcept = default;

	//* operator [assign] <std::shared_ptr<Assimp::Importer>> *//

	AssimpImporter(const std::shared_ptr<Assimp::Importer>& importer) noexcept : importer_(importer) {}
	AssimpImporter& operator=(const std::shared_ptr<Assimp::Importer>& importer) noexcept { importer_ = importer; return *this; }

	//* operator [assign] <std::nullptr_t> *//

	AssimpImporter(std::nullptr_t) noexcept : importer_(nullptr) {}
	AssimpImporter& operator=(std::nullptr_t) noexcept { importer_ = nullptr; return *this; }

	//* static methods *//

	static AssimpImporter Load(const std::filesystem::path& filepath, FlagEnum<AssimpOption> option);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<const Assimp::Importer> importer_;

};

SXAVENGER_ENGINE_NAMESPACE_END
