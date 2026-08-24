#include "DebugReporter.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* DirectX12
#include <dxgidebug.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DebugReporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugReporter::ReportLiveObjects() {
	ComPtr<IDXGIDebug1> debug;

	auto hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf()));
	if (FAILED(hr)) {
		STREAM_LOG_ERROR(
			
				L"Graphics::DebugReporter | failed to get debug interface. _com_error: {}",
				ComPtrUtil::GetComErrorMessage(hr)
			
		);
		return;
	}

	STREAM_LOG_DEBUG("Graphics::DebugReporter | reporting live objects...");
	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL); //!< 全てのAPIの全てのオブジェクトをレポート
}
