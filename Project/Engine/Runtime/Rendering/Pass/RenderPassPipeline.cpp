#include "RenderPassPipeline.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderPassPipeline::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) const {

	FlagEnum<PassContext::Issue> issues = PassContext::CheckIssue(context);

	if (issues.Test(PassContext::Issue::Error)) {
		STACK_LOG_ERROR("Rendering::RenderPassPipeline | pass context has critical error.");
		return; //!< PassContextに致命的なエラーがある場合, 実行を中止する
	}

	if (issues.Test(PassContext::Issue::Warning)) {
		STACK_LOG_WARNING("Rendering::RenderPassPipeline | pass context has warning.");
		//!< PassContextに警告がある場合, 実行を続行する
	}

	command.BeginEvent(std::format("RenderPassPipeline | {}", context.name));

	{ //!< PassのSetup関数を実行
		for (const auto& pass : passes_) {
			pass->Setup(context);
		}
	}

	{ //!< Bufferのクリア処理を実行
		command.BeginEvent("Clear Target Process");

		context.buffer->ClearTarget(command);

		command.EndEvent();
	}

	{ //!< PassのExecute関数を実行
		command.BeginEvent("Execute Process");

		for (const auto& pass : passes_) {
			pass->Execute(command, context);
		}

		command.EndEvent();
	}

	command.EndEvent();
}
