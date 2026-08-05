#include "SlateImGuiRenderer.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../Style/SlateStyleIO.h"
#include "../Style/SlateImGuiStyleIO.h"

//* engine
#include <Runtime/Graphics/Core.h>

//* imgui
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [ImGuiRenderer] Descriptors structure methods
////////////////////////////////////////////////////////////////////////////////////////////

const Graphics::Descriptor::Handle& Slate::ImGuiRenderer::Descriptors::Allocate() {

	//!< SRV用のDescriptorを確保.
	Graphics::Descriptor descriptor = Graphics::Core::AllocateDescriptor(Graphics::DescriptorCategory::SRV); 
	descriptors.emplace_back(std::move(descriptor));
	
	return descriptors.back().GetHandle(); //!< 追加したDescriptorのハンドルを返す.
}

void Slate::ImGuiRenderer::Descriptors::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) {
	//!< 指定されたCPUハンドルとGPUハンドルを持つDescriptorをリストから削除する.
	descriptors.remove_if([&](const Graphics::Descriptor& descriptor) {
		return (descriptor.GetCPUHandle().ptr == cpu.ptr) && (descriptor.GetGPUHandle().ptr == gpu.ptr);
	});
}

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::ImGuiRenderer::Init() {

	if (context_ != nullptr) {
		return; //!< contextが既に初期化済みの場合は初期化済みとみなす.
	}

	IMGUI_CHECKVERSION(); //!< imguiのバージョンチェック.
	
	CreateContext(); //!< contextの作成.

	//!< ImGuiIOの設定.
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr; //!< 状態の永続化は自己管理側で行う.

	InitContext(); //!< contextの初期化.

	LoadFont(); //!< フォントの読み込み.

	ImGuiStyleIO::SetCurrentStyle(ImGui::GetStyle()); //!< SlateのスタイルをImGuiに適用.
}

void Slate::ImGuiRenderer::Shutdown() {
	if (context_ != nullptr) {
		ImGui_ImplDX12_Shutdown(); //!< ImGuiのDX12レンダラーを終了.
		ImGui::DestroyContext(context_); //!< contextの破棄.
	}
	
	context_ = nullptr;
}

void Slate::ImGuiRenderer::SetCurrentContext() {
	StreamLogger::Assert(context_ != nullptr, "ImGui context is not initialized.");
	ImGui::SetCurrentContext(context_); //!< ImGuiの現在のコンテキストを設定.
}

void Slate::ImGuiRenderer::DrawRect(const Geometry& geometry, const Color4f& color) {
	ImVec2 a(geometry.absolutePosition.x, geometry.absolutePosition.y);
	ImVec2 b(geometry.absolutePosition.x + geometry.localSize.x, geometry.absolutePosition.y + geometry.localSize.y);
	GetTargetDrawList()->AddRectFilled(a, b, ImGui::ColorConvertFloat4ToU32(ImColor(color.r, color.g, color.b, color.a)));
}

void Slate::ImGuiRenderer::DrawRoundedRect(const Geometry& geometry, const Color4f& color, float rounding, Corner corner) {
	ImVec2 a(geometry.absolutePosition.x, geometry.absolutePosition.y);
	ImVec2 b(geometry.absolutePosition.x + geometry.localSize.x, geometry.absolutePosition.y + geometry.localSize.y);

	//!< 角丸のフラグを設定.
	ImDrawFlags flags = ImDrawFlags_None;
	if (corner.Test(CornerFlags::TopLeft))     flags |= ImDrawFlags_RoundCornersTopLeft;
	if (corner.Test(CornerFlags::TopRight))    flags |= ImDrawFlags_RoundCornersTopRight;
	if (corner.Test(CornerFlags::BottomLeft))  flags |= ImDrawFlags_RoundCornersBottomLeft;
	if (corner.Test(CornerFlags::BottomRight)) flags |= ImDrawFlags_RoundCornersBottomRight;

	GetTargetDrawList()->AddRectFilled(a, b, ImGui::ColorConvertFloat4ToU32(ImColor(color.r, color.g, color.b, color.a)), rounding, flags);
}

void Slate::ImGuiRenderer::DrawBorder(const Geometry& geometry, const Color4f& color, float thickness) {
	ImVec2 a(geometry.absolutePosition.x, geometry.absolutePosition.y);
	ImVec2 b(geometry.absolutePosition.x + geometry.localSize.x, geometry.absolutePosition.y + geometry.localSize.y);

	GetTargetDrawList()->AddRect(a, b, ImGui::ColorConvertFloat4ToU32(ImColor(color.r, color.g, color.b, color.a)), 0.0f, ImDrawFlags_None, thickness);
}

void Slate::ImGuiRenderer::DrawLine(const Vector2f& p1, const Vector2f& p2, const Color4f& color, float thickness) {
	ImVec2 a(p1.x, p1.y);
	ImVec2 b(p2.x, p2.y);

	GetTargetDrawList()->AddLine(a, b, ImGui::ColorConvertFloat4ToU32(ImColor(color.r, color.g, color.b, color.a)), thickness);
}

void Slate::ImGuiRenderer::DrawTextA(const Vector2f& position, const std::string_view& text, const Color4f& color, float fontSize) {
	GetTargetDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(position.x, position.y), ImGui::ColorConvertFloat4ToU32(ImColor(color.r, color.g, color.b, color.a)), text.data(), text.data() + text.size());
}

void Slate::ImGuiRenderer::DrawTextU8(const Vector2f& position, const std::u8string_view& text, const Color4f& color, float fontSize) {
	DrawTextA(position, reinterpret_cast<const char*>(text.data()), color, fontSize); //!< UTF-8文字列をchar*にキャストしてDrawTextAを呼び出す.
}

bool Slate::ImGuiRenderer::BeginRegion(const char* id, const Geometry& geometry) {
	StreamLogger::Assert(!isOpenRegion_, "ImGui region is already open."); //!< 既に描画領域が開かれている場合はエラー.

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.

	if (!isActiveFrame_) {
		return false; //!< ImGuiのフレームがアクティブでない場合は描画領域を開始しない.
	}

	//!< windowの位置とサイズを設定.
	ImGui::SetNextWindowPos(ImVec2(geometry.absolutePosition.x, geometry.absolutePosition.y));
	ImGui::SetNextWindowSize(ImVec2(geometry.localSize.x, geometry.localSize.y));

	//!< windowのフラグを設定.
	static const ImGuiWindowFlags kFlags
		= ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoBringToFrontOnFocus;
	// slate側が背景やレイアウトを管理するため、ImGui側では背景やレイアウトの管理を行わないようにする.

	//!< windowのスタイルの指定.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(1.0f, 1.0f));

	//!< ImGuiの描画領域を開始.
	const bool isOpen = ImGui::Begin(id, nullptr, kFlags);

	isOpenRegion_ = true; //!< 描画領域が開かれたことを記録.
	target_ = DrawTarget::Window; //!< 描画ターゲットを現在のウィンドウに設定.

	return isOpen;
}

void Slate::ImGuiRenderer::EndRegion() {
	if (!isOpenRegion_) {
		return; //!< 描画領域が開かれていない場合は何もしない.
	}

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.
	ImGui::End(); //!< ImGuiの描画領域を終了.
	ImGui::PopStyleVar(3); //!< ImGuiのスタイルを元に戻す.

	isOpenRegion_ = false; //!< 描画領域が閉じられたことを記録.
	target_ = DrawTarget::Background; //!< 描画ターゲットを背景に戻す.
}

void Slate::ImGuiRenderer::CreateContext() {
	context_ = ImGui::CreateContext();
	ImGui::SetCurrentContext(context_);
}

void Slate::ImGuiRenderer::LoadFont() {

	ImGuiIO& io = ImGui::GetIO();

	float baseFontSize = StyleIO::GetMetrics().fontBody; //!< 基準となるフォントサイズを取得.

	ImFontConfig font = {};
	font.FontDataOwnedByAtlas = false; //!< フォントデータの所有権をImFontAtlasに渡さない. (自前で管理する)

	{ //!< 英数fontの読み込み. (Roboto-Regular.ttf)

		std::filesystem::path filepath = "Engine/Packages/fonts/Roboto-Regular.ttf"; // TODO: フォントのパスはConfigで管理するようにする.

		ImFontConfig conf = font;
		conf.MergeMode = false;

		ImFont* pointer = io.Fonts->AddFontFromFileTTF(
			filepath.generic_string().c_str(),
			baseFontSize * 1.0f, //!< TODO: fontsizeを基準からの倍率で指定できるようにする.
			&conf
		);
		StreamLogger::Assert(pointer != nullptr, std::format("failed to load font. filepath: {}", filepath.generic_string()));
	}

	{ //!< 日本語fontの読み込み. (MPLUS1p-Regular.ttf)

		std::filesystem::path filepath = "Engine/Packages/fonts/MPLUS1p-Regular.ttf";

		ImFontConfig conf = font;
		font.MergeMode = true;

		ImFont* pointer = io.Fonts->AddFontFromFileTTF(
			filepath.generic_string().c_str(),
			baseFontSize * 1.0f, //!< TODO: fontsizeを基準からの倍率で指定できるようにする.
			&conf
		);
		StreamLogger::Assert(pointer != nullptr, std::format("failed to load font. filepath: {}", filepath.generic_string()));
	}

	{ //!< アイコンfontの読み込み. (MaterialSymbols*.ttf)

		std::filesystem::path filepath = "Engine/Packages/fonts/MaterialSymbolsSharp[FILL,GRAD,opsz,wght].ttf";

		//!< アイコンフォントの範囲を指定する. (Material Symbolsの範囲はU+E000～U+F8FF)
		static const ImWchar kIconRanges[] = { 0xE000, 0xF8FF, 0 };

		ImFontConfig conf = font;
		conf.MergeMode = true;

		ImFont* pointer = io.Fonts->AddFontFromFileTTF(
			filepath.generic_string().c_str(),
			baseFontSize * 1.0f, //!< TODO: fontsizeを基準からの倍率で指定できるようにする.
			&conf,
			kIconRanges
		);
		StreamLogger::Assert(pointer != nullptr, std::format("failed to load font. filepath: {}", filepath.generic_string()));
	}
}

void Slate::ImGuiRenderer::InitContext() {

	//!< DirectX12の初期化情報を設定.
	ImGui_ImplDX12_InitInfo info = {};
	info.Device            = Graphics::Core::GetDevice().GetDevice();
	info.CommandQueue      = Graphics::Core::GetCommandContextDirect().GetCommandQueue();
	info.NumFramesInFlight = Graphics::kFrameCount;
	info.RTVFormat         = DXGI_FORMAT_R8G8B8A8_UNORM; //!< TODO: formatをuser側で設定できるように変更.
	info.DSVFormat         = DXGI_FORMAT_UNKNOWN;

	info.UserData = &descriptors_; //!< Descriptors構造体のポインタを設定.

	//!< ImGuiのSRV用Descriptorを確保するコールバック関数.
	info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle) {

		Descriptors& descriptors = *static_cast<Descriptors*>(info->UserData); //!< Descriptors構造体を取得.

		//!< Descriptorを確保.
		Graphics::Descriptor::Handle handle = descriptors.Allocate();
		*out_cpu_desc_handle = handle.GetCPUHandle();
		*out_gpu_desc_handle = handle.GetGPUHandle();
	};

	//!< ImGuiのSRV用Descriptorを解放するコールバック関数.
	info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle) {

		Descriptors& descriptors = *static_cast<Descriptors*>(info->UserData); //!< Descriptors構造体を取得.

		//!< Descriptorを解放.
		descriptors.Free(cpu_desc_handle, gpu_desc_handle);
	};

	bool result = ImGui_ImplDX12_Init(&info);
	StreamLogger::Assert(result, "failed to initialize ImGui DX12 renderer.");
}

RefPtr<ImDrawList> Slate::ImGuiRenderer::GetTargetDrawList() {
	SetCurrentContext(); //!< ImGuiの現在のコンテキストを設定.
	switch (target_) {
		case DrawTarget::Background: return ImGui::GetBackgroundDrawList(); //!< 背景描画用のImDrawListを返す.
		case DrawTarget::Window: return ImGui::GetWindowDrawList();         //!< ウィンドウ描画用のImDrawListを返す.
		case DrawTarget::Foreground: return ImGui::GetForegroundDrawList(); //!< 最前面描画用のImDrawListを返す.
		default: return nullptr; //!< 無効なターゲットの場合はnullptrを返す.
	}
}
