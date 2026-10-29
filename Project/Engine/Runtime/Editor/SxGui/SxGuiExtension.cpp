#include "SxGuiExtension.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (Extension) namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

SxGui::ContentRect SxGui::CalcAspectFitRect(const Vector2f& image_size, const Vector2f& display_size) {

	ContentRect rect = {};

	float image_aspect_ratio   = image_size.x / image_size.y;
	float display_aspect_ratio = display_size.x / display_size.y;

	//!< 出力する画像サイズの設定
	rect.size = display_size;

	if (image_aspect_ratio <= display_aspect_ratio) {
		rect.size.x *= image_aspect_ratio / display_aspect_ratio;

	} else {
		rect.size.y *= display_aspect_ratio / image_aspect_ratio;
	}

	//!< 表示領域の中央の位置にカーソルを設定
	rect.cursol = (display_size - rect.size) / 2.0f;

	return rect;
}

bool SxGui::ButtonStretchWidth(const std::string_view& label, float height) {
	float width = SxGui::GetContentRegionAvail().x;
	return SxGui::Button(label, { width, height });
}

void SxGui::ImageAspectFit(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& display_size) {
	//!< 出力する画像サイズの設定
	ContentRect rect = CalcAspectFitRect(image_size, display_size);

	//!< 表示領域の中央に配置
	SxGui::SetCursorPos(SxGui::GetCursorPos() + rect.cursol);
	SxGui::Image(handle, rect.size);
}

void SxGui::ImageAspectFit(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& display_size, const Color4f& bg_color, const Color4f& tint_col) {
	//!< 出力する画像サイズの設定
	ContentRect rect = CalcAspectFitRect(image_size, display_size);

	//!< 表示領域の中央に配置
	SxGui::SetCursorPos(SxGui::GetCursorPos() + rect.cursol);
	SxGui::Image(handle, rect.size, bg_color, tint_col);
}
