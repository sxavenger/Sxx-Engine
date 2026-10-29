#include "SceneProxy.h"
#include "SceneProxy.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneProxy structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneProxy::Setup() {
	tlas.ResetInstance();

	for (const auto& mesh : staticMeshes) {
		if (!mesh.mesh->HasCache()) {
			continue; //!< キャッシュがない場合はスキップする.
		}

		Graphics::TopLevelAccelerationStructure::Instance instance = {};
		instance.blas         = &mesh.mesh->GetBottomLevelAccelerationStructure();
		instance.instanceId   = 0;
		instance.instanceMask = 0xFF;
		instance.flags        = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.transform    = mesh.transform->GetTransformationBuffer().At().world;
		
		tlas.AddInstance(instance);
	}

	Scheduler::GpuTask::Function function = [this](Graphics::GraphicsCommandContext& context) {
		tlas.SetupInstanceDesc(
			Graphics::Core::GetDevice(),
			context,
			Graphics::Core::GetResourceAllocator()
		);

		context.SubmitWait(); //!< GPUの処理完了まで待機する.
	};

	handle = Scheduler::System::PushComputeQueueTask(
		"Rendering::Foo | TopLevelAccelerationStructure - Setup",
		function,
		Scheduler::TaskPriority::Priority::High
	);
}

void SceneProxy::Reset() {
	staticMeshes.clear();
	directionalLights.clear();
	pointLights.clear();
	tlas.ResetInstance();
	handle = nullptr;
}

void SceneProxy::AddMeshInstance(const StaticMeshInstance& instance) {
	staticMeshes.emplace_back(instance);
}

void SceneProxy::AddLightInstance(const DirectionalLightInstance& instance) {
	directionalLights.emplace_back(instance);
}

void SceneProxy::AddLightInstance(const PointLightInstance& instance) {
	pointLights.emplace_back(instance);
}

const Graphics::TopLevelAccelerationStructure& SceneProxy::GetTopLevelAccelerationStructure() const {
	handle.Wait(); //!< GPUの処理完了まで待機する.
	return tlas;
}
