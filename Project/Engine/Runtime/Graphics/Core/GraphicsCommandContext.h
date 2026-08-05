#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "Device.h"
#include "DescriptorHeaps.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsCommandContext class
////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsCommandContext final {
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

		static Settings ParseFromConfig(const Configuration& config, GraphicsCommandType type);

		static void Log(GraphicsCommandType type, const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kBaseConfigPath{ "Graphics.GraphicsCommandContext" }; //!< 設定のベースパス.

		uint8_t allocatorCount = 1; //!< コマンドアロケータの数.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	GraphicsCommandContext() noexcept = default;
	~GraphicsCommandContext();

	void Init(const Configuration& config, const Device& device, GraphicsCommandType type);

	//* graphics command option *//

	void SetName(const std::wstring_view& name) const;
	void SetName(const std::string_view& name) const;

	void BeginEvent(const std::wstring_view& name);
	void BeginEvent(const std::string_view& name);

	void EndEvent();

	void SetDescriptorHeaps(const DescriptorHeaps& heaps) const;

	//! @brief コマンドアロケータを切り替える.
	void ExecuteAdvance();

	//! @brief コマンドアロケータを全て実行する.
	void ExecuteAll();

	RefPtr<ID3D12GraphicsCommandList6> GetCommandList() const { return commandList_.Get(); }

	RefPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue_.Get(); }

	//* type option *//

	GraphicsCommandType GetType() const { return type_; }

	//! @brief 指定されたコマンドタイプがサポートされているかを確認する.
	bool CheckSupportType(GraphicsCommandType request) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics command type *//

	GraphicsCommandType type_;

	//* DirectX12 *//

	std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators_; //!< コマンドアロケータの配列.
	ComPtr<ID3D12CommandQueue> commandQueue_;                       //!< コマンドキュー.
	ComPtr<ID3D12GraphicsCommandList6> commandList_;                //!< コマンドリスト.

	ComPtr<ID3D12Fence> fence_; //!< フェンス.

	//* synchronization *//

	HANDLE fenceEvent_; //!< フェンスイベント.

	uint8_t currentAllocatorIndex_ = 0; //!< 現在のアロケータのインデックス.
	uint64_t currentFenceValue_    = 0; //!< 現在のフェンス値.

	std::vector<uint64_t> allocatorFenceValues_; //!< 各アロケータの完了を示すフェンス値.

	//* settings *//

	Settings settings_;

	//* runtime parameter *//

	uint8_t eventIndent_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initailize helper methods *//

	static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(RefPtr<ID3D12Device> device, GraphicsCommandType type);

	static ComPtr<ID3D12CommandQueue> CreateCommandQueue(RefPtr<ID3D12Device> device, GraphicsCommandType type);

	static ComPtr<ID3D12GraphicsCommandList6> CreateCommandList(RefPtr<ID3D12Device> device, RefPtr<ID3D12CommandAllocator> allocator, GraphicsCommandType type);

	static ComPtr<ID3D12Fence> CreateFence(RefPtr<ID3D12Device> device);

	static HANDLE CreateFenceEvent();

	//* synchronization helper methods *//

	void Execute();

	void Signal();

	void WaitGpu(uint8_t allocatorIndex);

	void Reset(uint8_t allocatorIndex);
};

SXAVENGER_ENGINE_NAMESPACE_END
