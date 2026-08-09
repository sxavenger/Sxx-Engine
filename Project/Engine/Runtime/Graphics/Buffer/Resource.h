#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "ResourceDesc.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/CXXAttribute.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// Resource class
////////////////////////////////////////////////////////////////////////////////////////////
class Resource final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Resource() noexcept  = default;
	~Resource() noexcept = default;

	//* resource option *//

	void Reset();

	void SetName(const std::wstring_view& name) const;
	void SetName(const std::string_view& name) const;

	void Map(void** data, const std::optional<D3D12_RANGE>& range = std::nullopt);

	void Unmap(const std::optional<D3D12_RANGE>& range = std::nullopt);

	RefPtr<ID3D12Resource> Get() const { return resource_.Get(); }

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;

	D3D12_RESOURCE_DESC GetDesc() const;

	//* transition option *//

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> CreateTransitionBarrier(D3D12_RESOURCE_STATES state);

	void Transition(const GraphicsCommandContext& context, D3D12_RESOURCE_STATES state);

	void TransitionExplicit(const GraphicsCommandContext& context, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	D3D12_RESOURCE_STATES GetCurrentState() const { return current_; }

	static void AppendTransitionBarrier(std::vector<D3D12_RESOURCE_BARRIER>& barriers, Resource& resource, D3D12_RESOURCE_STATES state);

	//* operator [copy] <Resource> (delete) *//

	Resource(const Resource&)            = delete;
	Resource& operator=(const Resource&) = delete;

	//* operator [move] <Resource> *//

	Resource(Resource&& other) noexcept : resource_(std::move(other.resource_)), current_(other.current_) { other.Reset(); }
	Resource& operator=(Resource&& other) noexcept { resource_ = std::move(other.resource_); current_ = other.current_; other.Reset(); return *this; }

	//* operator [assign] <std::nullptr_t> *//

	Resource(std::nullptr_t) noexcept { Reset(); }
	Resource& operator=(std::nullptr_t) noexcept { Reset(); return *this; }

	//* operator [comparison] <Resource> *//

	bool operator==(const Resource& rhs) const { return resource_ == rhs.resource_; }
	bool operator!=(const Resource& rhs) const { return resource_ != rhs.resource_; }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return resource_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return resource_ != nullptr; }

	//* resource create option *//

	static Resource Wrap(ComPtr<ID3D12Resource>&& resource, D3D12_RESOURCE_STATES state); //!< 既存のID3D12ResourceをラップしてResourceを作成する.

	static Resource CreateCommitted(const Device& device, const ResourceDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	//* state tracker *//

	D3D12_RESOURCE_STATES current_ = D3D12_RESOURCE_STATE_COMMON;

};

SXAVENGER_ENGINE_NAMESPACE_END
