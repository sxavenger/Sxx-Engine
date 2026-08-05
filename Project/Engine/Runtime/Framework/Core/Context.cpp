#include "Context.h"
SXAVENGER_ENGINE_USING_(Framework)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// Context class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Context::Run() {

	for (auto& unit : pool_.units_) {
		unit->Setup(pipeline_); //!< Unitの処理を登録する
	}

	pipeline_.Execute(); //!< Unitの処理を実行.

	pool_.pointers_.clear();

	//!< Unitのインターフェースを破棄.
	while (!pool_.units_.empty()) {
		pool_.units_.pop_back();
	}



}
