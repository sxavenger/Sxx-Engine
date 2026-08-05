#include "SlateImGuiStyleIO.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* imgui
#include <imgui.h>

//* c++
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// anonymous namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	ImColor ToImCol(const Color4f& c) { return ImColor(c.r, c.g, c.b, c.a); }

}

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiStyleIO class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::ImGuiStyleIO::SetCurrentStyle(ImGuiStyle& style) {
	style.WindowRounding    = 0.0f;
	style.FrameRounding     = 2.0f;
	style.GrabRounding      = 2.0f;
	style.TabRounding       = StyleIO::GetMetrics().tabRounding;
	style.WindowBorderSize  = 0.0f;
	style.FrameBorderSize   = StyleIO::GetMetrics().borderThin;
	style.WindowPadding     = ImVec2(StyleIO::GetMetrics().paddingM, StyleIO::GetMetrics().paddingM);
	style.FramePadding      = ImVec2(StyleIO::GetMetrics().paddingM, 3.0f);
	style.ItemSpacing       = ImVec2(StyleIO::GetMetrics().paddingM, 4.0f);
	style.ScrollbarSize     = 12.0f;
	style.ScrollbarRounding = 0.0f;

	std::span<ImVec4, ImGuiCol_COUNT> colors = style.Colors;
	colors[ImGuiCol_Text]                 = ToImCol(StyleIO::GetColors().text);
	colors[ImGuiCol_TextDisabled]         = ToImCol(StyleIO::GetColors().textDim);
	colors[ImGuiCol_WindowBg]             = ToImCol(StyleIO::GetColors().panel);
	colors[ImGuiCol_ChildBg]              = ImVec4(0, 0, 0, 0);
	colors[ImGuiCol_PopupBg]              = ToImCol(StyleIO::GetColors().titleBar);
	colors[ImGuiCol_Border]               = ToImCol(StyleIO::GetColors().border);
	colors[ImGuiCol_FrameBg]              = ToImCol(StyleIO::GetColors().titleBar);
	colors[ImGuiCol_FrameBgHovered]       = ToImCol(StyleIO::GetColors().hover);
	colors[ImGuiCol_FrameBgActive]        = ToImCol(StyleIO::GetColors().hover);
	colors[ImGuiCol_TitleBg]              = ToImCol(StyleIO::GetColors().titleBar);
	colors[ImGuiCol_TitleBgActive]        = ToImCol(StyleIO::GetColors().titleBar);
	colors[ImGuiCol_MenuBarBg]            = ToImCol(StyleIO::GetColors().menuBar);
	colors[ImGuiCol_Header]               = ToImCol(StyleIO::GetColors().hover);
	colors[ImGuiCol_HeaderHovered]        = ToImCol(StyleIO::GetColors().buttonHover);
	colors[ImGuiCol_HeaderActive]         = ToImCol(StyleIO::GetColors().active);
	colors[ImGuiCol_Button]               = ToImCol(StyleIO::GetColors().hover);
	colors[ImGuiCol_ButtonHovered]        = ToImCol(StyleIO::GetColors().buttonHover);
	colors[ImGuiCol_ButtonActive]         = ToImCol(StyleIO::GetColors().active);
	colors[ImGuiCol_CheckMark]            = ToImCol(StyleIO::GetColors().active);
	colors[ImGuiCol_SliderGrab]           = ToImCol(StyleIO::GetColors().active);
	colors[ImGuiCol_SliderGrabActive]     = ToImCol(StyleIO::GetColors().selection);
	colors[ImGuiCol_Separator]            = ToImCol(StyleIO::GetColors().border);
	colors[ImGuiCol_ResizeGrip]           = ImVec4(0, 0, 0, 0);
	colors[ImGuiCol_ScrollbarBg]          = ImVec4(0, 0, 0, 0);
	colors[ImGuiCol_ScrollbarGrab]        = ToImCol(StyleIO::GetColors().hover);
	colors[ImGuiCol_ScrollbarGrabHovered] = ToImCol(StyleIO::GetColors().buttonHover);
	colors[ImGuiCol_ScrollbarGrabActive]  = ToImCol(StyleIO::GetColors().active);
}
