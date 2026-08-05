#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* lib
#include <Lib/Time/TimePoint.h>
#include <Lib/Time/RunTimeTracker.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Application)

////////////////////////////////////////////////////////////////////////////////////////////
// FrameClock class
////////////////////////////////////////////////////////////////////////////////////////////
class FrameClock final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Parse(const toml::table& config);

		static Settings ParseFromConfig(const Configuration& config);

		static void Log(const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kConfigPath{ "Application.FrameClock" }; //!< 設定のパス.

		bool enableFrameLimit = false; //!< フレームレートの上限を有効にするか.
		uint32_t frameRateLimit = 60; //!< フレームレートの上限.

		bool enableFixedDeltaTime = false; //!< フレーム間の時間を固定するか.
		TimePointd<TimeUnit::Second> fixedDeltaTime = TimePointd<TimeUnit::Second>(1.0f / 60.0f); //!< 固定するフレーム間の時間

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Configuration& config);

	//* frame option *//

	void BeginFrame();

	void EndFrame();

	template <TimeUnit T>
	TimePointd<T> GetDeltaTimed() const;

	template <TimeUnit T>
	TimePointf<T> GetDeltaTimef() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* tracker *//

	RunTimeTracker runtime_;

	//* settings *//

	Settings settings_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FrameClock class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit T>
TimePointd<T> FrameClock::GetDeltaTimed() const {
	if (settings_.enableFixedDeltaTime) {
		//!< フレーム間の時間を固定する場合は、設定された固定時間を返す.
		return settings_.fixedDeltaTime;
	}

	return runtime_.GetMeasuredTime();
}

template <TimeUnit T>
TimePointf<T> FrameClock::GetDeltaTimef() const {
	if (settings_.enableFixedDeltaTime) {
		//!< フレーム間の時間を固定する場合は、設定された固定時間を返す.
		return settings_.fixedDeltaTime;
	}

	return runtime_.GetMeasuredTime();
}

SXAVENGER_ENGINE_NAMESPACE_END
