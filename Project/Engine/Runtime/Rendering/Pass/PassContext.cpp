#include "PassContext.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PassContext structure methods
////////////////////////////////////////////////////////////////////////////////////////////

FlagEnum<PassContext::Issue> PassContext::CheckIssue(const PassContext& context) {

	FlagEnum<Issue> flags = Issue::Success;

	if (context.buffer == nullptr) {
		flags |= Issue::Error_Buffer;
	}

	if (context.scene == nullptr) {
		flags |= Issue::Warning_Scene;
	}

	if (context.camera.camera == nullptr || context.camera.transform == nullptr) {
		flags |= Issue::Warning_Camera;
	}

	return flags;
}

Vector2u PassContext::GetRenderResolution() const {
	const float raito = option.GetUpscaleRaito(); //!< アップスケーリング倍率を取得する.
	return Math::Max(static_cast<Vector2u>(Math::Round(static_cast<Vector2f>(resolution) / raito)), Vector2u(1, 1)); //!< アップスケーリング前の解像度は、最小値を1x1に制限する.
	// TODO: DLSS実行時はNGX_DLSS_GET_OPTIMAL_SETTINGSから取得する.
}

BaseBuffer::Resolution PassContext::GetBufferResolution() const {

	BaseBuffer::Resolution result;
	result.output = resolution; //!< フル出力解像度を設定.
	result.render = GetRenderResolution(); //!< アップスケーリング前の解像度を設定.

	return result;
}

PassContext::ContextData PassContext::GetContextData() const {
	ContextData data;
	data.outputResolution  = resolution;
	data.renderResolution  = GetRenderResolution();
	data.workingColorSpace = option.workingColorSpace;
	data.displayColorSpace = option.displayColorSpace;
	data.displayEncoder    = option.displayEncoder;

	return data;
}
