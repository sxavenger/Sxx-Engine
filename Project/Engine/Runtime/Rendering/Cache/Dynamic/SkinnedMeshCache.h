#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "BaseDynamicCache.h"
#include "../Static/SkeletalMeshIntermediateCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/BottomLevelAccelerationStructure.h>
#include <Runtime/Graphics/Buffer/DimensionBuffer.h>
#include <Runtime/Graphics/Buffer/UnorderedDimensionBuffer.h>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>
#include <Runtime/Rendering/Meshlet/MeshVertexBuffer.h>
#include <Runtime/World/Entity/EntityHandle.h>
#include <Runtime/World/Component/Renderer/SkeletalMeshRendererComponent.h>
#include <Runtime/World/Component/Armature/ArmatureComponent.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Matrix4x4.h>

//* c++
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshCache class
////////////////////////////////////////////////////////////////////////////////////////////
class SkinnedMeshCache final
	: public BaseDynamicCache {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PaletteData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PaletteData final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static PaletteData Transfer(const Transformation3d& transformation);

		//* constant value methods *//

		static PaletteData Identity() noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4f world;
		Matrix4x4f worldInverse;
		
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const SkeletalMeshIntermediateCache& intermediate, const World::SkeletalMeshRendererComponent& renderer);

	void Skinning(const Graphics::GraphicsCommandContext& command, const SkeletalMeshIntermediateCache& intermediate);

	//* cache option *//

	bool HasCache() const { return intermediateAddress_.has_value() && armatureAddress_.has_value(); }

	RefPtr<const World::EntityBehaviour> GetAttachJointEntity() const { return attachJointEntity_; }

	const Graphics::UnorderedDimensionBuffer<Vector3f>& GetPositionBuffer() const { return positionBuffer_; }

	const Graphics::UnorderedDimensionBuffer<MeshVertexBuffer::Vertex>& GetMeshVertexBuffer() const { return meshVertexBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffers *//

	Graphics::UnorderedDimensionBuffer<Vector3f>                 positionBuffer_;
	Graphics::UnorderedDimensionBuffer<MeshVertexBuffer::Vertex> meshVertexBuffer_;

	Graphics::DimensionBuffer<PaletteData> paletteBuffer_;

	//* Acceleration Structure *//

	Graphics::BottomLevelAccelerationStructure bottomLevelAS_;

	//* joint entities *//

	std::vector<RefPtr<const World::EntityBehaviour>> jointEntities_;
	RefPtr<const World::EntityBehaviour> attachJointEntity_;

	//* cache address *//

	std::optional<uintptr_t> intermediateAddress_ = std::nullopt; //!< intermediate cacheのアドレス
	std::optional<uintptr_t> armatureAddress_ = std::nullopt;     //!< armatureのアドレス
	std::string resolvedJointName_;                               //!< resolve済みのjoint名

	std::shared_ptr<const Assets::SkeletalMesh> reference_ = nullptr; //!< キャッシュの参照データ

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::optional<Transformation3d> GetWorldTransformation(const RefPtr<const World::EntityBehaviour>& entity);

	void CreateBuffers(const SkeletalMeshIntermediateCache& intermediate);

	void Resolve(const RefPtr<const World::ArmatureComponent>& armature, const std::string_view& name, const SkeletalMeshIntermediateCache& intermediate);

	void UpdatePalette();

};

SXAVENGER_ENGINE_NAMESPACE_END
