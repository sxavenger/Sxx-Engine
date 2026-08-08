#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../SlateJson.h"
#include "SlateStyle.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Color4.h>

//* c++
#include <span>
#include <string>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// StyleIO class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Slate のスタイル(Colors / Metrics)への参照と, JSON での保存 / 復元を行う.
	//! @note 対象は「外枠の共通スタイル」だけ. タブの種別色は DockPanel::TabColor() で
	//!       パネルごとに指定するものなので, ここには含めない
	//!       (スタイルを差し替えてもタブの種別色は変わらない).
	//!
	//! JSON 例:
	//! {
	//!   "colors":  { "Panel": "#2D2D30", "Border": "#3F3F46", "ActiveSoft": "#007ACC40" },
	//!   "metrics": { "TabBarHeight": 26, "DividerSize": 6 }
	//! }
	//!
	//! 色は "#RRGGBB" または "#RRGGBBAA". '#' は省略してもよい.
	class StyleIO {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// ColorEntry structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 色1項目ぶんの定義.
		//! @note 保存・復元・既定値リセット・エディタ表示のすべてがこの表を参照する.
		//!       項目を増やすときは .cpp の表に1行足すだけでよい.
		struct ColorEntry {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			const char* name        = nullptr; //!< JSON のキー(PascalCase).
			Color4f* value          = nullptr; //!< 実体へのポインタ.
			Color4f def             = {};      //!< 既定値(Style::Colors のメンバ初期化子と同じ).
			const char* group       = nullptr; //!< エディタでの見出し.
			const char* description = nullptr; //!< エディタの tooltip(どこに使われる色か).

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// MetricEntry structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 寸法1項目ぶんの定義.
		//! @note min / max は「エディタのスライダ範囲」と「Load() のクランプ範囲」を兼ねる.
		//!       負の値が正当な項目もあるため, 符号で弾いてはいけない.
		struct MetricEntry {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			const char* name        = nullptr;
			float* value            = nullptr;
			float def               = 0.0f;
			float min               = 0.0f;
			float max               = 0.0f;
			const char* group       = nullptr;
			const char* description = nullptr;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* style option *//

		static Style::Metrics& GetMetrics();

		static Style::Colors& GetColors();

		//* entry option *//

		//! @brief 色の項目表. 要素の value は Style::GetStyle() の実体を指す.
		static std::span<const ColorEntry> GetColorEntries();

		//! @brief 寸法の項目表. 要素の value は Style::GetStyle() の実体を指す.
		static std::span<const MetricEntry> GetMetricEntries();

		//* json option *//

		//! @brief 現在のスタイルを JSON へ書き出す.
		static Json Save();

		//! @brief JSON から現在のスタイルへ読み込む.
		//! @note 不明なキー・型違いの値は無視する(その項目は現在値を保つ).
		//!       object でない場合だけ false を返す.
		static bool Load(const Json& value);

		//* default option *//

		//! @brief すべての項目を既定値へ戻す.
		static void ResetToDefault();

		//* hex option *//

		//! @brief 色を "#RRGGBB" / "#RRGGBBAA" へ変換する.
		//! @note alpha が 1.0 のときは "#RRGGBB"(6桁)になる.
		static std::string ToHex(const Color4f& color);

		//! @brief "#RRGGBB" / "#RRGGBBAA" を色へ変換する.
		//! @note '#' は省略可. 桁数違い・16進でない文字が混ざる場合は fallback を返す.
		static Color4f FromHex(const std::string_view& hex, const Color4f& fallback);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
