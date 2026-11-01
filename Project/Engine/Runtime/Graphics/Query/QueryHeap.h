#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Device/Device.h"
#include "../Command/GraphicsCommandContext.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// QueryHeap class
////////////////////////////////////////////////////////////////////////////////////////////
class QueryHeap final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint32_t {
		Occlusion = D3D12_QUERY_HEAP_TYPE_OCCLUSION,
		Timestamp = D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	QueryHeap() noexcept  = default;
	~QueryHeap() noexcept = default;

	//* query option *//

	void BeginQuery(const GraphicsCommandContext& context, uint32_t index);

	void EndQuery(const GraphicsCommandContext& context, uint32_t index);

	// TODO: ResolveQueryDataのラッパーの作成.

	//* static methods *//

	static QueryHeap Create(const Device& device, Type type, uint32_t capacity);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12QueryHeap> heap_ = nullptr;

	Type type_ = Type::Occlusion;
	uint32_t capacity_ = 0;

};

SXAVENGER_ENGINE_NAMESPACE_END
