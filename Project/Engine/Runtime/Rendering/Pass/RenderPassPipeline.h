#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "BaseRenderPass.h"
#include "PassContext.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>

//* c++
#include <memory>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPassPipeline final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* pipeline option *//

	//! @brief 指定した型の描画パスの追加
	template <RenderPass T>
	void Insert();

	//! @brief 全パスを順番に実行する
	void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<BaseRenderPass>> passes_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassPipeline class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <RenderPass T>
void RenderPassPipeline::Insert() {
	std::unique_ptr<T> pass = std::make_unique<T>();
	passes_.emplace_back(std::move(pass));
}

SXAVENGER_ENGINE_NAMESPACE_END
