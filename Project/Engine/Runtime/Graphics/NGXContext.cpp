#include "NGXContext.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>

//* dlss
#include <DLSS/nvsdk_ngx.h>

////////////////////////////////////////////////////////////////////////////////////////////
// NGXContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void NGXContext::Install(const Device& device) {

	NVSDK_NGX_FeatureCommonInfo info = {};
	info.LoggingInfo.MinimumLoggingLevel      = NVSDK_NGX_LOGGING_LEVEL_ON;
	info.LoggingInfo.DisableOtherLoggingSinks = true;
	info.LoggingInfo.LoggingCallback          = [](const char* msg, NVSDK_NGX_Logging_Level level, NVSDK_NGX_Feature source) {
		std::string_view message(msg);
		if (!message.empty() && message.back() == '\n') {
			message.remove_suffix(1); //!< 末尾の改行文字を削除する.
		}

		STREAM_LOG_DEBUG("Graphics::NGXContext - Callback | <{} - {}> {}", source, level, message);
	};

	NVSDK_NGX_Result result = NVSDK_NGX_D3D12_Init_with_ProjectID(
		kProjectId.Serialize().c_str(),
		NVSDK_NGX_ENGINE_TYPE_CUSTOM,
		"0.0.0",
		L"",
		device.GetDevice(),
		&info
	);
	// note: NGXはDriver側のDLLをロードして初期化するため、初期化に失敗のログを出力する場合がある.

	if (NVSDK_NGX_FAILED(result)) {
		STREAM_LOG_ERROR("Graphics::NGXContext | NGX initialization failed.");
		return; //!< NGXの初期化に失敗した場合は、エラーをログに出力して終了する.
	}

}

void NGXContext::Uninstall(const Device& device) {

	NVSDK_NGX_D3D12_Shutdown1(device.GetDevice());

}
