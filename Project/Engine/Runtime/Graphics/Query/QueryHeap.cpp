#include "QueryHeap.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// QueryHeap class methods
////////////////////////////////////////////////////////////////////////////////////////////

void QueryHeap::BeginQuery(const GraphicsCommandContext& context, uint32_t index) {
	context.GetCommandList()->BeginQuery(
		heap_.Get(),
		static_cast<D3D12_QUERY_TYPE>(type_),
		index
	);
	// note: TimestampはBeginQueryを行わない
}

void QueryHeap::EndQuery(const GraphicsCommandContext& context, uint32_t index) {
	context.GetCommandList()->EndQuery(
		heap_.Get(),
		static_cast<D3D12_QUERY_TYPE>(type_),
		index
	);
}

QueryHeap QueryHeap::Create(const Device& device, Type type, uint32_t capacity) {

	QueryHeap heap = {};
	heap.type_     = type;
	heap.capacity_ = capacity;

	D3D12_QUERY_HEAP_DESC desc = {};
	desc.Type     = static_cast<D3D12_QUERY_HEAP_TYPE>(type);
	desc.Count    = capacity;
	desc.NodeMask = 0;

	auto hr = device.GetDevice()->CreateQueryHeap(
		&desc,
		IID_PPV_ARGS(&heap.heap_)
	);
	STREAM_ASSERT_SUMMARY(SUCCEEDED(hr), L"query heap create failed.", L"_com_error_: {}", GetComErrorMessage(hr));

	return heap;
}
