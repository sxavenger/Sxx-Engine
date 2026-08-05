#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <cstdint>
#include <limits>
#include <functional>
#include <array>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Framework)

////////////////////////////////////////////////////////////////////////////////////////////
// Phase enum class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Frameworkの処理フェーズ
enum class Phase : uint8_t {
	//* 初期化
	Initialize,

	//* loop処理
	BeginFrame,
	Update,
	Render,
	EndFrame,

	//* 終了処理
	Terminate,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Priority enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class Priority : uint8_t {
	Highest = std::numeric_limits<uint8_t>::max(), //!< 最優先
	Normal  = Highest / 2,                         //!< 標準
	Lowest  = std::numeric_limits<uint8_t>::min(), //!< 最終
};

////////////////////////////////////////////////////////////////////////////////////////////
// Pipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class Pipeline final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Process   = std::function<void()>; //!< 実行処理
	using Condition = std::function<bool()>; //!< loopを抜ける条件式

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Layer {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint8_t priority; //!< 処理順序
		Process process;  //!< 実行処理

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* process option *//

	//! @brief contextの実行処理
	void Execute();

	//! @brief FrameworkPhaseの処理を登録する
	//! @param[in] phase    実行する箇所
	//! @param[in] priority 処理順序
	//! @param[in] function 処理内容
	void SetProcess(Phase phase, uint8_t priority, const Process& function);

	//! @brief FrameworkPhaseの処理を登録する
	//! @param[in] phase    実行する箇所
	//! @param[in] priority 処理順序
	//! @param[in] function 処理内容
	void SetProcess(Phase phase, Priority priority, const Process& function);

	//! @brief Gameloopの終了条件を登録する
	//! @param[in] function 終了条件
	void SetCondition(const Condition& function);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::list<Layer>, EnumUtil<Phase>::GetCount()> processes_; //!< Phaseの処理を格納する配列
	std::list<Condition> conditions_;                                     //!< loopを抜ける条件式

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RunPhase(Phase phase, bool reverse = false);

};

SXAVENGER_ENGINE_NAMESPACE_END
