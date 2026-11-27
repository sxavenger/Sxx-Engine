#include "SkinnedMeshCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../../Skinning/SkinningPipeline.h"

//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/World/Component/Transform/TransformComponent.h>
#include <Runtime/World/Entity/EntityBehaviour.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [SkinnedMeshCache] PaletteData structure
////////////////////////////////////////////////////////////////////////////////////////////

SkinnedMeshCache::PaletteData SkinnedMeshCache::PaletteData::Transfer(const Transformation3d& transformation) {
	PaletteData data = {};
	data.world        = Transformation3d::ConvertMatrix(transformation);
	data.worldInverse = Transformation3d::ConvertMatrix(Transformation3d::Inverse(transformation));

	return data;
}

SkinnedMeshCache::PaletteData SkinnedMeshCache::PaletteData::Identity() noexcept {
	PaletteData data = {};
	data.world        = Matrix4x4f::Identity();
	data.worldInverse = Matrix4x4f::Identity();

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedMeshCache::Cache(const SkeletalMeshIntermediateCache& intermediate, const World::SkeletalMeshRendererComponent& renderer) {

	if (!intermediate.HasCache()) {
		STACK_LOG_WARNING("Rendering::SkinnedMeshCache | skeletal mesh intermediate cache is not ready.");
		return; //!< intermediate cacheが作成されていない場合はキャッシュしない
	}

	bool isResolveRequired = false; //!< cacheのresolveが必要かどうか

	RefPtr<const World::EntityBehaviour> entity = renderer.FindArmatureEntity();
	if (entity == nullptr) {
		STACK_LOG_WARNING("Rendering::SkinnedMeshCache | armature entity is not found.");
		return; //!< armature entityが見つからない場合はキャッシュしない
	}

	RefPtr<const World::ArmatureComponent> armature = entity->GetComponent<World::ArmatureComponent>();
	const uintptr_t armatureAddress = (armature != nullptr) ? armature->GetAddress() : NULL;

	if (armatureAddress != armatureAddress_ || renderer.GetJointName() != resolvedJointName_) {
		isResolveRequired = true; //!< armature or attach jointが変更された場合
	}

	if (intermediateAddress_ != intermediate.GetAddress()) { //!< meshが変更された場合
		//!< bufferの再作成
		SkinnedMeshCache::CreateBuffers(intermediate);
		intermediateAddress_ = intermediate.GetAddress();
		reference_ = intermediate.GetReference(); //!< キャッシュの参照データを更新

		isResolveRequired = true; //!< joint数/joint名が変わるため再解決
	}

	if (isResolveRequired) {
		Resolve(armature, renderer.GetJointName(), intermediate);
	}

	UpdatePalette();
}

void SkinnedMeshCache::Skinning(const Graphics::GraphicsCommandContext& command, const SkeletalMeshIntermediateCache& intermediate) {

	if (!HasCache()) {
		STACK_LOG_WARNING("Rendering::SkinnedMeshCache | skinning skipped. cache is not ready.");
		return; //!< cacheが無効 or 別meshのintermediateの場合は処理しない
	}

	if (intermediateAddress_.value() != intermediate.GetAddress()) {
		STACK_LOG_WARNING("Rendering::SkinnedMeshCache | skinning skipped. intermediate cache is not matched.");
		return; //!< 別meshのintermediateの場合は処理しない
	}

	//!< bufferをUnorederedAccess状態に遷移
	positionBuffer_.GetResource().Transition(command, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	meshVertexBuffer_.GetResource().Transition(command, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	SkinningPipeline::BindPipeline(command);

	SkinningPipeline::Context context = {};
	context.jointCount  = intermediate.GetJointCount();
	context.vertexCount = intermediate.GetVertexCount();

	Graphics::ShaderParameter parameter;

	//!< コンテキスト情報
	parameter.Set32bitConstants("Context", context);

	//!< 頂点情報
	parameter.SetAddress("gPositions", intermediate.GetPositionVertexBuffer().positions.GetGpuVirtualAddress());
	parameter.SetAddress("gVertices",  intermediate.GetMeshVertexBuffer().vertices.GetGpuVirtualAddress());

	//!< 出力頂点情報
	parameter.SetAddress("gOutPositions", positionBuffer_.GetResource().GetGpuVirtualAddress());
	parameter.SetAddress("gOutVertices",  meshVertexBuffer_.GetResource().GetGpuVirtualAddress());

	//!< 影響頂点情報
	parameter.SetAddress("gInfluences", intermediate.GetInfluenceBuffer().influences.GetGpuVirtualAddress());

	//!< パレット変形行列
	parameter.SetAddress("gPalette", paletteBuffer_.GetResource().GetGpuVirtualAddress());

	SkinningPipeline::BindShaderParameter(command, parameter);
	SkinningPipeline::Dispatch(command, intermediate.GetVertexCount());

	//!< bufferをAllShader状態に遷移
	positionBuffer_.GetResource().Transition(command, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	meshVertexBuffer_.GetResource().Transition(command, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
}

std::optional<Transformation3d> SkinnedMeshCache::GetWorldTransformation(const RefPtr<const World::EntityBehaviour>& entity) {
	if (entity == nullptr || !entity->HasComponent<World::TransformComponent>()) {
		return std::nullopt; //!< entityが無い, もしくはTransformComponentが無い場合
	}

	return entity->GetComponent<World::TransformComponent>()->GetWorld();
}

void SkinnedMeshCache::CreateBuffers(const SkeletalMeshIntermediateCache& intermediate) {

	const uint32_t vertexCount = intermediate.GetVertexCount();
	const uint32_t jointCount  = intermediate.GetJointCount();

	//!< 出力bufferの作成
	positionBuffer_   = Graphics::Core::CreateUnorderedDimensionBuffer<Vector3f>(vertexCount, 1);
	positionBuffer_.SetName("SkinnedMeshCache | PositionBuffer");

	meshVertexBuffer_ = Graphics::Core::CreateUnorderedDimensionBuffer<MeshVertexBuffer::Vertex>(vertexCount, 1);
	meshVertexBuffer_.SetName("SkinnedMeshCache | MeshVertexBuffer");

	//!< Palette bufferの作成
	paletteBuffer_ = Graphics::Core::CreateDimensionBuffer<PaletteData>(jointCount, Graphics::kFrameCount); //!< joint数が変わる可能性があるため, FrameCount分確保する
	paletteBuffer_.SetName("SkinnedMeshCache | PaletteBuffer");
}

void SkinnedMeshCache::Resolve(const RefPtr<const World::ArmatureComponent>& armature, const std::string_view& name, const SkeletalMeshIntermediateCache& intermediate) {

	//!< 解決状態のclear
	jointEntities_.clear();
	attachJointEntity_ = nullptr;

	//!< jointの解決
	armatureAddress_   = (armature != nullptr) ? armature->GetAddress() : NULL;
	resolvedJointName_ = name;

	//!< palette順のjoint entityの解決 (resolve時のみ探索する. 見つからない場合はFindJointEntity内でwarningが出る)
	const auto& joints = intermediate.GetReference()->GetDescription().joints;
	jointEntities_.reserve(joints.size());

	for (const auto& joint : joints) {
		jointEntities_.emplace_back(armature->FindJointEntity(joint.name));
	}

	//!< attach jointの解決
	attachJointEntity_ = armature->FindJointEntity(name);

	if (attachJointEntity_ != nullptr) {
		STREAM_LOG_WARNING("Rendering::SkinnedMeshCache | attach joint has no transform component. palette is identity. name: {}", name);
	}

	STREAM_LOG_INFO("Rendering::SkinnedMeshCache | resolve completed.");
}

void SkinnedMeshCache::UpdatePalette() {

	const std::optional<Transformation3d> world = SkinnedMeshCache::GetWorldTransformation(attachJointEntity_);
	if (!world.has_value()) {
		paletteBuffer_.Fill(PaletteData::Identity());
		return; //!< attach jointが無効な場合は全てidentity (bind poseをそのまま出力)
	}

	Transformation3d attachWorld        = world.value();
	Transformation3d attachWorldInverse = Transformation3d::Inverse(attachWorld);

	//!< paletteの転送
	//!< palette_j = B_j * W_j * W_attach^-1 (row-vector, 左が先に適用)
	//!< VS/MS側で W_attach を掛けると B_j * W_j となり, Σ w_j * v * B_j * W_j と一致する.
	//!< W_attachはVS/MS側のTransformCacheと同じTransformComponentの値のため, 打ち消しは正確に一致する.
	const auto& joints = reference_->GetDescription().joints;

	for (uint32_t j = 0; j < static_cast<uint32_t>(joints.size()); ++j) {
		const std::optional<Transformation3d> jointWorld = SkinnedMeshCache::GetWorldTransformation(jointEntities_[j]);

		if (!jointWorld.has_value()) {
			paletteBuffer_.At(j) = PaletteData::Identity();
			continue;
		}

		paletteBuffer_.At(j) = PaletteData::Transfer(joints[j].bindPoseInverse * jointWorld.value() * attachWorldInverse);
	}

}
