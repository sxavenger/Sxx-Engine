#include "SlateStyleIO.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// static helper methods
////////////////////////////////////////////////////////////////////////////////////////////

namespace {

	//!< 0.0 - 1.0 の成分を 0 - 255 へ丸める.
	uint32_t ToByte(float value) {
		const int32_t result = static_cast<int32_t>(value * 255.0f + 0.5f);
		return static_cast<uint32_t>(std::clamp(result, 0, 255));
	}

	//!< hex[index] から2文字を16進として読む. 16進でない文字が混ざっていれば -1.
	int32_t ToByteFromHex(const std::string_view& hex, size_t index) {
		int32_t result = 0;

		for (size_t i = 0; i < 2; ++i) {
			const char c = hex[index + i];

			if (c >= '0' && c <= '9') {
				result = result * 16 + (c - '0');

			} else if (c >= 'a' && c <= 'f') {
				result = result * 16 + (c - 'a' + 10);

			} else if (c >= 'A' && c <= 'F') {
				result = result * 16 + (c - 'A' + 10);

			} else {
				return -1; //!< 16進でない文字.
			}
		}

		return result;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// StyleIO class methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::Style::Metrics& Slate::StyleIO::GetMetrics() {
	return Style::GetStyle().metrics;
}

Slate::Style::Colors& Slate::StyleIO::GetColors() {
	return Style::GetStyle().colors;
}

std::span<const Slate::StyleIO::ColorEntry> Slate::StyleIO::GetColorEntries() {
	//!< 既定値は Style::Colors のメンバ初期化子から取る(同じ数値を2か所に書かないため).
	static const Style::Colors kDefault = {};

	//!< Color4f は単項 operator& を float* を返すよう多重定義しているため, &color では
	//!< Color4f* にならない. 実体へのポインタは std::addressof で作る.
	static const std::array<ColorEntry, 19> kEntries = {{
		//* surfaces *//
		{ "Background",    std::addressof(GetColors().background),    kDefault.background,    "Surfaces",       "分割の溝(パネルの隙間)" },
		{ "Panel",         std::addressof(GetColors().panel),         kDefault.panel,         "Surfaces",       "パネルの地色" },
		{ "TitleBar",      std::addressof(GetColors().titleBar),      kDefault.titleBar,      "Surfaces",       "ウィンドウのタイトルバー" },
		{ "MenuBar",       std::addressof(GetColors().menuBar),       kDefault.menuBar,       "Surfaces",       "メニューバーの背景" },

		//* tabs *//
		{ "TabBar",        std::addressof(GetColors().tabBar),        kDefault.tabBar,        "Tabs",           "タブバーの地色(非アクティブタブ)" },
		{ "TabActive",     std::addressof(GetColors().tabActive),     kDefault.tabActive,     "Tabs",           "アクティブタブ. パネル面と同色にすると地続きに見える" },
		{ "TabUnderline",  std::addressof(GetColors().tabUnderline),  kDefault.tabUnderline,  "Tabs",           "タブバー下端の線. アクティブタブの区間だけ切り欠かれる" },
		{ "Close",         std::addressof(GetColors().close),         kDefault.close,         "Tabs",           "タブの close ボタンのホバー" },

		//* lines / states *//
		{ "Border",        std::addressof(GetColors().border),        kDefault.border,        "Lines & States", "枠線・区切り線" },
		{ "Hover",         std::addressof(GetColors().hover),         kDefault.hover,         "Lines & States", "分割の仕切りなどのホバー" },
		{ "ButtonHover",   std::addressof(GetColors().buttonHover),   kDefault.buttonHover,   "Lines & States", "タイトルバーのボタンのホバー" },
		{ "Selection",     std::addressof(GetColors().selection),     kDefault.selection,     "Lines & States", "選択(橙)" },
		{ "Active",        std::addressof(GetColors().active),        kDefault.active,        "Lines & States", "ドラッグ中・ドロップ先(青)" },
		{ "ActiveSoft",    std::addressof(GetColors().activeSoft),    kDefault.activeSoft,    "Lines & States", "ドロップゾーンの塗り(半透明)" },

		//* text / icons *//
		{ "Text",          std::addressof(GetColors().text),          kDefault.text,          "Text & Icons",   "本文" },
		{ "TextDim",       std::addressof(GetColors().textDim),       kDefault.textDim,       "Text & Icons",   "非アクティブタブ名・補足" },
		{ "IconDim",       std::addressof(GetColors().iconDim),       kDefault.iconDim,       "Text & Icons",   "単色・線画のアイコンの線" },

		//* tooltip *//
		{ "TooltipBg",     std::addressof(GetColors().tooltipBg),     kDefault.tooltipBg,     "Tooltip",        "ツールチップの背景(半透明)" },
		{ "TooltipBorder", std::addressof(GetColors().tooltipBorder), kDefault.tooltipBorder, "Tooltip",        "ツールチップの枠線" },
	}};

	return kEntries;
}

std::span<const Slate::StyleIO::MetricEntry> Slate::StyleIO::GetMetricEntries() {
	static const Style::Metrics kDefault = {};

	static const std::array<MetricEntry, 18> kEntries = {{
		//* spacing *//
		{ "PaddingM",         std::addressof(GetMetrics().paddingM),         kDefault.paddingM,          0.0f, 20.0f, "Spacing",  "基本の内側余白" },
		{ "MarginL",          std::addressof(GetMetrics().marginL),          kDefault.marginL,           0.0f, 30.0f, "Spacing",  "タイトル文字などの外側余白" },
		{ "BorderThin",       std::addressof(GetMetrics().borderThin),       kDefault.borderThin,        0.0f,  4.0f, "Spacing",  "通常の枠線の太さ" },
		{ "BorderThick",      std::addressof(GetMetrics().borderThick),      kDefault.borderThick,       0.0f,  8.0f, "Spacing",  "強調時の枠線の太さ" },

		//* font *//
		{ "FontBody",         std::addressof(GetMetrics().fontBody),         kDefault.fontBody,          8.0f, 32.0f, "Font",     "本文の文字サイズ(px)" },
		{ "FontHeading",      std::addressof(GetMetrics().fontHeading),      kDefault.fontHeading,       8.0f, 48.0f, "Font",     "見出しの文字サイズ(px)" },

		//* bars *//
		{ "TitleBarHeight",   std::addressof(GetMetrics().titleBarHeight),   kDefault.titleBarHeight,   16.0f, 60.0f, "Bars",     "ウィンドウのタイトルバーの高さ" },
		{ "MenuBarHeight",    std::addressof(GetMetrics().menuBarHeight),    kDefault.menuBarHeight,    14.0f, 48.0f, "Bars",     "メニューバーの高さ" },
		{ "TabBarHeight",     std::addressof(GetMetrics().tabBarHeight),     kDefault.tabBarHeight,     16.0f, 48.0f, "Bars",     "タブバーの高さ" },

		//* tabs *//
		{ "TabPaddingX",      std::addressof(GetMetrics().tabPaddingX),      kDefault.tabPaddingX,       0.0f, 24.0f, "Tabs",     "タブ内の左右余白" },
		{ "TabGap",           std::addressof(GetMetrics().tabGap),           kDefault.tabGap,            0.0f, 12.0f, "Tabs",     "タブ同士の隙間" },
		{ "TabRounding",      std::addressof(GetMetrics().tabRounding),      kDefault.tabRounding,       0.0f, 12.0f, "Tabs",     "タブ上端の角丸" },
		{ "TabAccentH",       std::addressof(GetMetrics().tabAccentH),       kDefault.tabAccentH,        0.0f,  8.0f, "Tabs",     "アクティブタブ上端の色帯の太さ" },
		{ "TabUnderlineSize", std::addressof(GetMetrics().tabUnderlineSize), kDefault.tabUnderlineSize,  0.0f,  8.0f, "Tabs",     "タブバー下端の線の太さ(0 で消える)" },
		{ "TabCloseSize",     std::addressof(GetMetrics().tabCloseSize),     kDefault.tabCloseSize,      0.0f, 24.0f, "Tabs",     "タブの close ボタンの大きさ" },

		//* splitter *//
		{ "DividerSize",      std::addressof(GetMetrics().dividerSize),      kDefault.dividerSize,       2.0f, 20.0f, "Splitter", "仕切りの幅(掴める範囲も同じ)" },

		//* tooltip *//
		{ "TooltipDelay",     std::addressof(GetMetrics().tooltipDelay),     kDefault.tooltipDelay,      0.0f,  3.0f, "Tooltip",  "ツールチップを出すまでの秒数" },
		{ "TooltipPadding",   std::addressof(GetMetrics().tooltipPadding),   kDefault.tooltipPadding,    0.0f, 24.0f, "Tooltip",  "ツールチップの内側余白" },
	}};

	return kEntries;
}

Slate::Json Slate::StyleIO::Save() {
	Json colors = Json::object();
	for (const ColorEntry& entry : GetColorEntries()) {
		colors[entry.name] = ToHex(*entry.value);
	}

	Json metrics = Json::object();
	for (const MetricEntry& entry : GetMetricEntries()) {
		metrics[entry.name] = *entry.value;
	}

	return Json{ { "colors", std::move(colors) }, { "metrics", std::move(metrics) } };
}

bool Slate::StyleIO::Load(const Json& value) {
	if (!value.is_object()) {
		return false;
	}

	//* colors *//

	//!< 不正な値は現在値を保つ(FromHex の fallback に現在値を渡す).
	const auto colors = value.find("colors");
	if (colors != value.end() && colors->is_object()) {
		for (const ColorEntry& entry : GetColorEntries()) {
			const auto it = colors->find(entry.name);

			if (it == colors->end() || !it->is_string()) {
				continue;
			}

			*entry.value = FromHex(it->get<std::string>(), *entry.value);
		}
	}

	//* metrics *//

	//!< 各エントリの min / max へクランプする.
	//!< 「負の値は無視」のような一律の符号判定にすると, 負が正当な項目を読み込めなくなる.
	const auto metrics = value.find("metrics");
	if (metrics != value.end() && metrics->is_object()) {
		for (const MetricEntry& entry : GetMetricEntries()) {
			const auto it = metrics->find(entry.name);

			//!< JsonReader<float> は is_number_float() 判定なので "TabBarHeight": 26 のような
			//!< 整数リテラルを弾く. ここでは生の is_number() を使う.
			if (it == metrics->end() || !it->is_number()) {
				continue;
			}

			*entry.value = std::clamp(it->get<float>(), entry.min, entry.max);
		}
	}

	return true;
}

void Slate::StyleIO::ResetToDefault() {
	for (const ColorEntry& entry : GetColorEntries()) {
		*entry.value = entry.def;
	}

	for (const MetricEntry& entry : GetMetricEntries()) {
		*entry.value = entry.def;
	}
}

std::string Slate::StyleIO::ToHex(const Color4f& color) {
	char buffer[16] = {};

	//!< alpha が 1.0 のときは 6桁で書き出す(手で編集しやすいため).
	if (color.a >= 0.999f) {
		std::snprintf(buffer, sizeof(buffer), "#%02X%02X%02X", ToByte(color.r), ToByte(color.g), ToByte(color.b));

	} else {
		std::snprintf(buffer, sizeof(buffer), "#%02X%02X%02X%02X", ToByte(color.r), ToByte(color.g), ToByte(color.b), ToByte(color.a));
	}

	return buffer;
}

Color4f Slate::StyleIO::FromHex(const std::string_view& hex, const Color4f& fallback) {
	//!< "#RRGGBB" / "#RRGGBBAA". '#' は省略可.
	const size_t begin  = (!hex.empty() && hex.front() == '#') ? 1 : 0;
	const size_t length = hex.size() - begin;

	if (length != 6 && length != 8) {
		return fallback;
	}

	const int32_t r = ToByteFromHex(hex, begin + 0 * 2);
	const int32_t g = ToByteFromHex(hex, begin + 1 * 2);
	const int32_t b = ToByteFromHex(hex, begin + 2 * 2);
	const int32_t a = (length == 8) ? ToByteFromHex(hex, begin + 3 * 2) : 255;

	if (r < 0 || g < 0 || b < 0 || a < 0) {
		return fallback;
	}

	return Color4f{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
}
