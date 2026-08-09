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

//* c++
#include <algorithm>
#include <cfloat>

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
		//!< contextはnative windowごとに存在するため, 破棄対象を必ずcurrentにしてから終了する.
		//!< これを行わないと, 別windowのbackend dataを破棄してしまう.
		ImGui::SetCurrentContext(context_);

		ImGui_ImplDX12_Shutdown(); //!< ImGuiのDX12レンダラーを終了.
		ImGui::DestroyContext(context_); //!< contextの破棄.
	}

	context_ = nullptr;
}

void Slate::ImGuiRenderer::SetCurrentContext() {
	StreamLogger::Assert(context_ != nullptr, "ImGui context is not initialized.");
	ImGui::SetCurrentContext(context_); //!< ImGuiの現在のコンテキストを設定.
}

void Slate::ImGuiRenderer::BeginFrame(const Vector2f& displaySize, TimePointf<TimeUnit::Second> deltaTime) {
	if (context_ == nullptr) {
		return; //!< contextが未初期化の場合は何もしない.
	}

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.

	StreamLogger::Assert(!isActiveFrame_, "ImGui frame is already active. (BeginFrame/EndFrame is not paired correctly)");

	if (isActiveFrame_) {
		return; //!< 多重にNewFrameが呼ばれるのを防ぐ.
	}

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize             = ImVec2(displaySize.x, displaySize.y);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	io.DeltaTime               = std::max(deltaTime.time, std::numeric_limits<float>::min()); //!< ImGuiはDeltaTime > 0を要求するため, 0を渡さないようにする.

	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();

	isActiveFrame_ = true; //!< フレームが開始されたことを記録.
}

void Slate::ImGuiRenderer::EndFrame(const Graphics::GraphicsCommandContext& context) {
	if (!isActiveFrame_) {
		return; //!< フレームが開始されていない場合は何もしない.
	}

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.

	StreamLogger::Assert(!isOpenRegion_, "ImGui region is still open. (BeginRegion/EndRegion is not paired correctly)");

	ImGui::Render();

	context.SetDescriptorHeaps(Graphics::Core::GetDescriptorHeaps()); //!< ImGuiの描画に必要なDescriptorHeapsを設定.
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context.GetCommandList().Get());

	isActiveFrame_ = false; //!< フレームが終了したことを記録.
}

void Slate::ImGuiRenderer::DrawRect(const Geometry& geometry, const Color4f& color) {
	ImVec2 a(geometry.absolutePosition.x, geometry.absolutePosition.y);
	ImVec2 b(geometry.absolutePosition.x + geometry.localSize.x, geometry.absolutePosition.y + geometry.localSize.y);
	GetTargetDrawList()->AddRectFilled(a, b, Slate::ImGuiRenderer::ToDrawColor(color));
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

	GetTargetDrawList()->AddRectFilled(a, b, Slate::ImGuiRenderer::ToDrawColor(color), rounding, flags);
}

void Slate::ImGuiRenderer::DrawBorder(const Geometry& geometry, const Color4f& color, float thickness) {
	ImVec2 a(geometry.absolutePosition.x, geometry.absolutePosition.y);
	ImVec2 b(geometry.absolutePosition.x + geometry.localSize.x, geometry.absolutePosition.y + geometry.localSize.y);

	GetTargetDrawList()->AddRect(a, b, Slate::ImGuiRenderer::ToDrawColor(color), 0.0f, ImDrawFlags_None, thickness);
}

void Slate::ImGuiRenderer::DrawLine(const Vector2f& p1, const Vector2f& p2, const Color4f& color, float thickness) {
	ImVec2 a(p1.x, p1.y);
	ImVec2 b(p2.x, p2.y);

	GetTargetDrawList()->AddLine(a, b, Slate::ImGuiRenderer::ToDrawColor(color), thickness);
}

void Slate::ImGuiRenderer::DrawTextA(const Vector2f& position, const std::string_view& text, const Color4f& color, float fontSize) {

	//!< 引数の評価順は未規定なので, GetTargetDrawList()(内部でSetCurrentContextする)より先に
	//!< ImGui::GetFont()が評価され得る. そうなると別windowのcontextのfontを掴んでしまい,
	//!< atlasがwindowごとに独立しているためグリフが出ない / 化ける.
	//!< contextを切り替えてからfontを取り, 順序を確定させる.
	RefPtr<ImDrawList> drawList = GetTargetDrawList();

	if (drawList == nullptr) {
		return;
	}

	drawList->AddText(
		ImGui::GetFont(), fontSize, ImVec2(position.x, position.y),
		Slate::ImGuiRenderer::ToDrawColor(color), text.data(), text.data() + text.size()
	);
}

void Slate::ImGuiRenderer::DrawTextU8(const Vector2f& position, const std::u8string_view& text, const Color4f& color, float fontSize) {
	DrawTextA(position, reinterpret_cast<const char*>(text.data()), color, fontSize); //!< UTF-8文字列をchar*にキャストしてDrawTextAを呼び出す.
}

Vector2f Slate::ImGuiRenderer::MeasureTextA(const std::string_view& text, float fontSize) {
	SetCurrentContext(); //!< フォントはコンテキストごとなので、計測前に切り替える.

	const ImVec2 size = ImGui::GetFont()->CalcTextSizeA(
		fontSize, FLT_MAX, 0.0f, text.data(), text.data() + text.size()
	);
	return { size.x, size.y };
}

Vector2f Slate::ImGuiRenderer::MeasureTextU8(const std::u8string_view& text, float fontSize) {
	return MeasureTextA(
		std::string_view(reinterpret_cast<const char*>(text.data()), text.size()), fontSize
	); //!< UTF-8文字列をchar*にキャストしてMeasureTextAを呼び出す.
}

void Slate::ImGuiRenderer::BeginClipRect(const Geometry& geometry) {

	RefPtr<ImDrawList> drawList = GetTargetDrawList();

	if (drawList == nullptr) {
		return;
	}

	const ImVec2 min(geometry.absolutePosition.x, geometry.absolutePosition.y);
	const ImVec2 max(
		geometry.absolutePosition.x + geometry.localSize.x,
		geometry.absolutePosition.y + geometry.localSize.y
	);

	//!< 既にある領域と積を取る. 外側のclipを無視して広げてしまわないようにする.
	drawList->PushClipRect(min, max, true);
}

void Slate::ImGuiRenderer::EndClipRect() {

	RefPtr<ImDrawList> drawList = GetTargetDrawList();

	if (drawList == nullptr) {
		return;
	}

	drawList->PopClipRect();
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
	//!< slate側が背景やレイアウトを管理するため、ImGui側では背景やレイアウトの管理を行わないようにする.

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

void Slate::ImGuiRenderer::InjectMousePosition(const Vector2f& position) {
	if (context_ == nullptr) {
		return; //!< contextが未初期化の場合は何もしない.
	}

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.
	ImGui::GetIO().AddMousePosEvent(position.x, position.y);
}

void Slate::ImGuiRenderer::InjectMouseButton(int32_t index, bool isDown) {
	if (context_ == nullptr) {
		return; //!< contextが未初期化の場合は何もしない.
	}

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.
	ImGui::GetIO().AddMouseButtonEvent(index, isDown);
}

void Slate::ImGuiRenderer::InjectMouseWheel(float horizontal, float vertical) {
	if (context_ == nullptr) {
		return; //!< contextが未初期化の場合は何もしない.
	}

	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.
	ImGui::GetIO().AddMouseWheelEvent(horizontal, vertical);
}

bool Slate::ImGuiRenderer::IsInteracting() {
	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.

	if (!isActiveFrame_) {
		return false; //!< フレーム外ではIsAnyItemActive()等の呼び出しが不正なため.
	}

	//!< io.WantCaptureMouseは前フレームのマウス位置を基にNewFrame()内で計算されるため1フレーム古く、
	//!<       ドラッグ操作が固まる原因になるので使わない.
	return ImGui::IsAnyItemActive() || ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
}

bool Slate::ImGuiRenderer::WantCaptureKeyboard() {
	SetCurrentContext(); //!< Renderer専用のImGuiコンテキストを設定.

	if (!isActiveFrame_) {
		return false; //!< フレーム外ではWantCaptureKeyboardの値が不定なため.
	}

	return ImGui::GetIO().WantCaptureKeyboard;
}

void Slate::ImGuiRenderer::CreateContext() {
	context_ = ImGui::CreateContext();
	ImGui::SetCurrentContext(context_);
}

void Slate::ImGuiRenderer::LoadFont() {

	ImGuiIO& io = ImGui::GetIO();

	float baseFontSize = StyleIO::GetMetrics().fontBody; //!< 基準となるフォントサイズを取得.

	//!< TODO: fontのパスと基準サイズからの倍率をConfigurationで管理できるようにする.
	//!<       以下3ブロックのfilepathと baseFontSize への係数がその対象.

	ImFontConfig font = {};
	font.FontDataOwnedByAtlas = false; //!< フォントデータの所有権をImFontAtlasに渡さない. (自前で管理する)

	{ //!< 英数fontの読み込み. (Roboto-Regular.ttf)

		std::filesystem::path filepath = "Engine/Packages/fonts/Roboto-Regular.ttf";

		ImFontConfig conf = font;
		conf.MergeMode = false;

		ImFont* pointer = io.Fonts->AddFontFromFileTTF(
			filepath.generic_string().c_str(),
			baseFontSize * 1.0f,
			&conf
		);
		StreamLogger::Assert(pointer != nullptr, std::format("failed to load font. filepath: {}", filepath.generic_string()));
	}

	{ //!< 日本語fontの読み込み. (MPLUS1p-Regular.ttf)

		std::filesystem::path filepath = "Engine/Packages/fonts/MPLUS1p-Regular.ttf";

		ImFontConfig conf = font;
		conf.MergeMode = true; //!< 先に読み込んだfontへ統合する.
		//!< note: ここをfont.MergeModeにすると, confのMergeModeがfalseのままで別のfontが作られる.
		//!<       すると後続のicon fontもそちらへ統合され, GetFont()が返す先頭のfontに
		//!<       日本語もiconも入らなくなる. (iconが描画されない原因だった)

		ImFont* pointer = io.Fonts->AddFontFromFileTTF(
			filepath.generic_string().c_str(),
			baseFontSize * 1.0f,
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
			baseFontSize * 1.2f,
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
	//!< PSOのRTV formatは, 実際にbindされるRTVのformatと完全に一致していなければ
	//!< DrawIndexedInstancedが EXECUTION ERROR #613 で失敗する.
	//!< note: SwapChainのresource自体は ConvertToLinearFormat で作られるが(FLIP_DISCARDが
	//!<       sRGB backbufferを許さないため), bindされるRTVは Init / Resize ともに
	//!<       ConvertToSRGBFormat で生成される. 合わせるべき相手はresourceではなくRTV側.
	//!< TODO: formatをuser側で設定できるように変更. (SwapChainのformatと二重管理になっているため.)
	info.RTVFormat         = Graphics::ConvertToSRGBFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
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

uint32_t Slate::ImGuiRenderer::ToDrawColor(const Color4f& color) {
	//!< RTVがsRGB formatのため, 書き込み時にGPUがlinear->sRGB変換を行う.
	//!< Styleの色はsRGB値なので, linearへ戻してから渡さないと二重にガンマがかかる.
	const Color4f linear = Slate::ConvertToLinearColor(color);
	return ImGui::ColorConvertFloat4ToU32(ImColor(linear.r, linear.g, linear.b, linear.a));
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
