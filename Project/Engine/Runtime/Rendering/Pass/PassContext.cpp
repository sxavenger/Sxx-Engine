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

PassContext::ContextData PassContext::GetContextData() const {
	ContextData data;
	data.resolution        = resolution;
	data.workingColorSpace = option.workingColorSpace;

	return data;
}
