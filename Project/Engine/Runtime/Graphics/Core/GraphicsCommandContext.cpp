#include "GraphicsCommandContext.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../PixEvent.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Toml/TomlReader.h>
#include <Lib/String/UnicodeConverter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [GraphicsCommandContext] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsCommandContext::Settings::Parse(const toml::table& config) {
	TomlReader<uint8_t>::Find(config, "allocatorCount", allocatorCount); //!< コマンドアロケータの数の取得.
}

GraphicsCommandContext::Settings GraphicsCommandContext::Settings::ParseFromConfig(const Configuration& config, GraphicsCommandType type) {

	Settings settings;

	//!< Commandの種類に応じた設定のパスを作成. (ex."Graphics.GraphicsCommandContext.Direct")
	std::string str = std::format("{}.{}", kBaseConfigPath.GetPath(), EnumUtil<GraphicsCommandType>::GetName(type));
	Configuration::Path path = Configuration::Path(str);

	if (!config.Contains(path.GetPath())) {
		StreamLogger::Warning(
			"Graphics::GraphicsCommandContext::Settings | config does not exist. path: {}", path.GetPath()
		);

		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(path.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(path.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void GraphicsCommandContext::Settings::Log(GraphicsCommandType type, const Settings& settings) {
	StreamLogger::Debug("Graphics::GraphicsCommandContext::Settings<{}> | allocatorCount: {}", type, settings.allocatorCount);
}

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsCommandContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

GraphicsCommandContext::~GraphicsCommandContext() {
	StreamLogger::Info("Graphics::GraphicsCommandContext<{}> | terminated.", type_);
}

void GraphicsCommandContext::Init(const Configuration& config, const Device& device, GraphicsCommandType type) {

	//!< コマンドの種類の保存.
	type_ = type;

	//!< 設定の取得.
	settings_ = Settings::ParseFromConfig(config, type_);
	Settings::Log(type_, settings_); //!< 設定のログ.

	commandAllocators_.resize(settings_.allocatorCount); //!< allocatorの数だけスロットを確保.
	allocatorFenceValues_.resize(settings_.allocatorCount); //!< allocatorの数だけフェンス値のスロットを確保.

	//!< DirectXの初期化.
	for (uint8_t i = 0; i < settings_.allocatorCount; ++i) {
		commandAllocators_[i] = GraphicsCommandContext::CreateCommandAllocator(device.GetDevice(), type_);
	}

	commandQueue_ = GraphicsCommandContext::CreateCommandQueue(device.GetDevice(), type_);
	commandList_  = GraphicsCommandContext::CreateCommandList(device.GetDevice(), commandAllocators_[currentAllocatorIndex_].Get(), type_);
	fence_        = GraphicsCommandContext::CreateFence(device.GetDevice());

	fenceEvent_ = GraphicsCommandContext::CreateFenceEvent(); //!< フェンスイベントの作成.

	StreamLogger::Info("Graphics::GraphicsCommandContext<{}> | initialized.", type_);
}

void GraphicsCommandContext::SetName(const std::wstring_view& name) const {

	std::wstring type = UnicodeConverter::ConvertW(EnumUtil<GraphicsCommandType>::GetName(type_));

	commandList_->SetName(std::format(L"{} | Command List <{}>", name, type).c_str());
	commandQueue_->SetName(std::format(L"{} | Command Queue <{}>", name, type).c_str());

	for (uint8_t i = 0; i < settings_.allocatorCount; ++i) {
		commandAllocators_[i]->SetName(std::format(L"{} | Command Allocator [{}] <{}>", name, i, type).c_str());
	}
}

void GraphicsCommandContext::SetName(const std::string_view& name) const {
	SetName(UnicodeConverter::ConvertW(name));
}

void GraphicsCommandContext::BeginEvent(const std::wstring_view& name) {
	PixEvent::BeginEvent(commandList_.Get(), name, eventIndent_);
	eventIndent_++;
}

void GraphicsCommandContext::BeginEvent(const std::string_view& name) {
	BeginEvent(UnicodeConverter::ConvertW(name));
}

void GraphicsCommandContext::EndEvent() {
	StreamLogger::Assert(eventIndent_ > 0, "Graphics::GraphicsCommandContext | event indent is zero.");
	PixEvent::EndEvent(commandList_.Get());
	eventIndent_--;
}

void GraphicsCommandContext::SetDescriptorHeaps(const DescriptorHeaps& heaps) const {
	if (type_ == GraphicsCommandType::Copy) {
		StreamLogger::Warning(
			"Graphics::GraphicsCommandContext | SetDescriptorHeaps is not supported. Copy command does not use SRV/CBV/UAV descriptor heaps."
		);
		return; //!< CopyコマンドはSRV/CBV/UAVのデスクリプタヒープを使用しないので, 設定しない.
	}

	std::vector<ID3D12DescriptorHeap*> descriptorHeaps;
	descriptorHeaps.emplace_back(heaps.GetAllocator(DescriptorCategory::SRV_CBV_UAV).GetDescriptorHeap());

	commandList_->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()), descriptorHeaps.data());
}

void GraphicsCommandContext::ExecuteAdvance() {
	Execute();
	Signal();

	uint8_t next = (currentAllocatorIndex_ + 1) % settings_.allocatorCount; //!< 次のアロケータのインデックス.
	Reset(next);
}

void GraphicsCommandContext::ExecuteAll() {
	Execute();
	Signal();

	Reset(currentAllocatorIndex_);
	//!< 全てのアロケータがGPUに処理されるのを待つ.
}

bool GraphicsCommandContext::CheckSupportType(GraphicsCommandType request) const {
	return type_ <= request; //!< 現在のコマンドタイプが要求されたコマンドタイプ以上であればサポートされている. (ex. DirectはCompute, Copyをサポートするが, ComputeはDirectをサポートしない)
}

ComPtr<ID3D12CommandAllocator> GraphicsCommandContext::CreateCommandAllocator(RefPtr<ID3D12Device> device, GraphicsCommandType type) {

	ComPtr<ID3D12CommandAllocator> allocator;

	auto hr = device->CreateCommandAllocator(
		GetCommandListType(type),
		IID_PPV_ARGS(&allocator)
	);
	ComPtrUtil::Assert(hr, L"create command allocator failed.");

	return allocator;
}

ComPtr<ID3D12CommandQueue> GraphicsCommandContext::CreateCommandQueue(RefPtr<ID3D12Device> device, GraphicsCommandType type) {

	ComPtr<ID3D12CommandQueue> queue;

	//!< descの設定
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type     = GetCommandListType(type);
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	auto hr = device->CreateCommandQueue(
		&desc,
		IID_PPV_ARGS(&queue)
	);
	ComPtrUtil::Assert(hr, L"create command queue failed.");

	return queue;
}

ComPtr<ID3D12GraphicsCommandList6> GraphicsCommandContext::CreateCommandList(RefPtr<ID3D12Device> device, RefPtr<ID3D12CommandAllocator> allocator, GraphicsCommandType type) {

	ComPtr<ID3D12GraphicsCommandList6> commandList;

	auto hr = device->CreateCommandList(
		0,
		GetCommandListType(type),
		allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList)
	);
	ComPtrUtil::Assert(hr, L"create command list failed.");

	return commandList;
}

ComPtr<ID3D12Fence> GraphicsCommandContext::CreateFence(RefPtr<ID3D12Device> device) {

	ComPtr<ID3D12Fence> fence;

	auto hr = device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence)
	);
	ComPtrUtil::Assert(hr, L"create fence failed.");

	return fence;
}

HANDLE GraphicsCommandContext::CreateFenceEvent() {

	HANDLE event = CreateEvent(
		nullptr,
		false,
		false,
		nullptr
	);

	StreamLogger::Assert(event != nullptr, "create fence event failed.");
	return event;
	
}

void GraphicsCommandContext::Execute() {
	//!< コマンドリストをクローズ.
	auto hr = commandList_->Close();
	ComPtrUtil::Assert(hr, L"close command list failed.");

	//!< コマンドリストをコマンドキューに送信.
	ID3D12CommandList* lists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(_countof(lists), lists);
}

void GraphicsCommandContext::Signal() {
	currentFenceValue_++; //!< 現在のfence値を更新
	commandQueue_->Signal(fence_.Get(), currentFenceValue_);
	allocatorFenceValues_[currentAllocatorIndex_] = currentFenceValue_; //!< 現在のアロケータの完了を示すフェンス値を更新.
}

void GraphicsCommandContext::WaitGpu(uint8_t allocatorIndex) {

	uint64_t value = allocatorFenceValues_[allocatorIndex]; //!< 指定したアロケータの完了を示すフェンス値.

	if (fence_->GetCompletedValue() < value) {
		//!< 指定したSignalにたどり着いていないので, たどり着くまで待つようにイベントを設定.
		fence_->SetEventOnCompletion(value, fenceEvent_);
		//!< イベントを待機.
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

}

void GraphicsCommandContext::Reset(uint8_t allocatorIndex) {
	//!< allocatorがGPUに処理されるのを待つ.
	WaitGpu(allocatorIndex);

	//!< allocatorのリセット
	auto hr = commandAllocators_[allocatorIndex]->Reset();
	ComPtrUtil::Assert(hr, L"command allocator reset failed.");

	//!< commandListのリセット.
	hr = commandList_->Reset(commandAllocators_[allocatorIndex].Get(), nullptr);
	ComPtrUtil::Assert(hr, L"command list reset failed.");

	// 現在のindexとして設定
	currentAllocatorIndex_ = allocatorIndex;
}
