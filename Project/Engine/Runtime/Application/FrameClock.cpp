#include "FrameClock.h"
SXAVENGER_ENGINE_USING_(Application)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Toml/TomlReader.h>

//* c++
#include <chrono>
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// [FrameClock] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FrameClock::Settings::Parse(const toml::table& config) {
	TomlReader<bool>::Find(config, "enableFrameLimit", enableFrameLimit);
	TomlReader<uint32_t>::Find(config, "frameRateLimit", frameRateLimit);
	TomlReader<bool>::Find(config, "enableFixedDeltaTime", enableFixedDeltaTime);
	TomlReader<double>::Find(config, "fixedDeltaTime", fixedDeltaTime.time);
}

FrameClock::Settings FrameClock::Settings::ParseFromConfig(const Configuration& config) {

	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Application::FrameClock::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void FrameClock::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Application::FrameClock::Settings | enableFrameLimit: {}", settings.enableFrameLimit);
	StreamLogger::Debug("Application::FrameClock::Settings | frameRateLimit: {}", settings.frameRateLimit);
	StreamLogger::Debug("Application::FrameClock::Settings | enableFixedDeltaTime: {}", settings.enableFixedDeltaTime);
	StreamLogger::Debug("Application::FrameClock::Settings | fixedDeltaTime: {}", settings.fixedDeltaTime.time);
}

////////////////////////////////////////////////////////////////////////////////////////////
// FrameClock class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FrameClock::Init(const Configuration& config) {
	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ出力.
}

void FrameClock::BeginFrame() {
	runtime_.Start();
}

void FrameClock::EndFrame() {
	//!< フレームレート制限の処理.
	if (settings_.enableFrameLimit) {
		TimePointd<TimeUnit::Microsecond> elapsed = runtime_.GetElapsedTime(); //!< 経過時間の取得.
		const TimePointd<TimeUnit::Microsecond> frameTime
			= TimePointd<TimeUnit::Second>(1.0 / static_cast<double>(settings_.frameRateLimit)) + TimePointd<TimeUnit::Microsecond>(4); //!< フレーム時間の取得. (モニターのHzの倍数出ない場合の誤差を吸収するために4マイクロ秒を加算)

		if (elapsed < frameTime) {
			const std::chrono::steady_clock::time_point reference = runtime_.GetReference();
			const auto time = reference + std::chrono::microseconds(static_cast<int64_t>(frameTime.time));
			std::this_thread::sleep_until(time); //!< フレーム時間に達するまでスリープする.
		}
	}

	runtime_.Stop();
}
