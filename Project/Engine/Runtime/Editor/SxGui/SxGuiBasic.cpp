#include "SxGuiBasic.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (Basic) namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector2f SxGui::GetContentRegionAvail() {
	ImVec2 region = ImGui::GetContentRegionAvail();
	return { region.x, region.y };
}

Vector2f SxGui::GetCursorPos() {
	ImVec2 pos = ImGui::GetCursorPos();
	return { pos.x, pos.y };
}

void SxGui::SetCursorPos(const Vector2f& pos) {
	ImGui::SetCursorPos({ pos.x, pos.y });
}

Vector2f SxGui::CalcTextSize(const std::string_view& text, bool hide_text_after_double_hash, float wrap_width) {
	ImVec2 size = ImGui::CalcTextSize(text.data(), nullptr, hide_text_after_double_hash, wrap_width);
	return { size.x, size.y };
}

bool SxGui::ColorEdit3(const std::string_view& label, float col[3], ImGuiColorEditFlags flags) {
	return Internal::ColorEdit3(label.data(), col, flags);
}

bool SxGui::ColorEdit3(const std::string_view& label, Color3f& col, ImGuiColorEditFlags flags) {
	return Internal::ColorEdit3(label.data(), col.data.data(), flags);
}

bool SxGui::ColorEdit4(const std::string_view& label, float col[4], ImGuiColorEditFlags flags) {
	return Internal::ColorEdit4(label.data(), col, flags);
}

bool SxGui::ColorEdit4(const std::string_view& label, Color4f& col, ImGuiColorEditFlags flags) {
	return Internal::ColorEdit4(label.data(), col.data.data(), flags);
}

bool SxGui::Button(const std::string_view& label, const Vector2f& size) {
	return ImGui::Button(label.data(), { size.x, size.y });
}

void SxGui::Image(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& uv0, const Vector2f& uv1) {
	ImGui::Image(handle.GetGPUHandle().ptr, { image_size.x, image_size.y }, { uv0.x, uv0.y }, { uv1.x, uv1.y });
}

void SxGui::Image(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Color4f& bg_color, const Color4f& tint_color, const Vector2f& uv0, const Vector2f& uv1) {
	ImGui::ImageWithBg(handle.GetGPUHandle().ptr, { image_size.x, image_size.y }, { uv0.x, uv0.y }, { uv1.x, uv1.y }, { bg_color.r, bg_color.g, bg_color.b, bg_color.a }, { tint_color.r, tint_color.g, tint_color.b, tint_color.a });
}

void SxGui::Text(const std::string_view& text) {
	ImGui::Text(text.data());
}
