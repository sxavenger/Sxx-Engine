#include "Pipeline.h"
SXAVENGER_ENGINE_USING_(Framework)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <algorithm>
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// Pipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Pipeline::Execute() {

	//!< プロセスを優先度順にソートする
	for (auto& process : processes_) {
		process.sort([](const Layer& a, const Layer& b) { return a.priority > b.priority; });
	}
	// note: これ以降, processの追加は追加順番を保障できない.

	StreamLogger::Info("Framework::Pipeline | begin execution pipeline.");

	RunPhase(Phase::Initialize); //!< 初期化処理

	if (!conditions_.empty()) {

		while (true) {

			if (std::any_of(conditions_.begin(), conditions_.end(), [](const std::function<bool()>& function) { return function(); })) {
				break; //!< loopを抜ける条件式がtrueになった場合, loopを抜ける.
			}

			RunPhase(Phase::BeginFrame); //!< loopの開始処理

			RunPhase(Phase::Update); //!< loopの更新処理

			RunPhase(Phase::Render); //!< loopの描画処理

			RunPhase(Phase::EndFrame); //!< loopの終了処理

		}
		
	} else {
		//!< loopを抜ける条件式がないので不適格.
		StreamLogger::Warning("Framework::Pipeline | loop condition is empty. not execute loop phase.");
	}

	RunPhase(Phase::Terminate, true); //!< 終了処理(priorityの逆順で実行する.)

	StreamLogger::Info("Framework::Pipeline | end execution pipeline.");
}

void Pipeline::SetProcess(Phase phase, uint8_t priority, const Process& function) {
	uint8_t index = EnumUtil<Phase>::Cast(phase);
	processes_[index].emplace_back(priority, function);
}

void Pipeline::SetProcess(Phase phase, Priority priority, const Process& function) {
	uint8_t index = EnumUtil<Phase>::Cast(phase);
	processes_[index].emplace_back(EnumUtil<Priority>::Cast(priority), function);
}

void Pipeline::SetCondition(const Condition& function) {
	conditions_.emplace_back(function);
}

void Pipeline::RunPhase(Phase phase, bool reverse) {

	uint8_t index = EnumUtil<Phase>::Cast(phase);

	if (processes_[index].empty()) {
		return; //!< phaseに対応する処理がない場合は何もしない
	}

	if (reverse) { //!< phaseの実行. (reverseの場合は逆順で実行する.)
		for (const auto& layer : processes_[index] | std::views::reverse) {
			layer.process();
		}

	} else {
		for (auto& layer : processes_[index]) {
			layer.process();
		}
	}
}
