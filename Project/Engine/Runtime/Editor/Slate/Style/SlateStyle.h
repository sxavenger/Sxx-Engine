#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Style structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Style {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Colors structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief カラーパレット（UE5 Editor のダークテーマ相当）.
		struct Colors {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			//* surfaces *//
			//!< 面は奥から 3 階層で重ねる.
			//!<   Recessed(1A1A1A) : 奥まった面. 分割の溝 / タイトルバー / タブバー.
			//!<   Panel(242424)    : 作業面. パネル / メニューバー / アクティブタブ.
			//!<   Header(2F2F2F)   : 最前の面. ツールチップの背景.

			Color4f background   = static_cast<Color4f>(Color4u{ 0x1A, 0x1A, 0x1A }); //!< 分割の溝. (Recessed)
			Color4f panel        = static_cast<Color4f>(Color4u{ 0x24, 0x24, 0x24 }); //!< パネル面. (Panel)
			Color4f titleBar     = static_cast<Color4f>(Color4u{ 0x1A, 0x1A, 0x1A }); //!< (Recessed)
			Color4f menuBar      = static_cast<Color4f>(Color4u{ 0x24, 0x24, 0x24 }); //!< (Panel)
			Color4f tabBar       = static_cast<Color4f>(Color4u{ 0x1A, 0x1A, 0x1A }); //!< (Recessed)
			Color4f tabActive    = static_cast<Color4f>(Color4u{ 0x24, 0x24, 0x24 }); //!< パネルと地続きに見せる. (Panel)
			Color4f tabUnderline = static_cast<Color4f>(Color4u{ 0x38, 0x38, 0x38 }); //!< タブバーの下線.

			//* lines *//

			Color4f border = static_cast<Color4f>(Color4u{ 0x38, 0x38, 0x38 });

			//* hover / accent *//

			Color4f hover      = static_cast<Color4f>(Color4u{ 0x38, 0x38, 0x38 });       //!< ボタン面・入力枠. ImGuiのButton / FrameBgHoveredに写される.
			Color4f selection  = static_cast<Color4f>(Color4u{ 0x00, 0x70, 0xE0 });       //!< 選択（青）.
			Color4f active     = static_cast<Color4f>(Color4u{ 0x00, 0x70, 0xE0 });       //!< 実行中（青）.
			Color4f activeSoft = static_cast<Color4f>(Color4u{ 0x00, 0x70, 0xE0, 0x40 }); //!< 実行中（青）. ドロップ先プレビュー専用.

			//* controls *//

			Color4f buttonHover = static_cast<Color4f>(Color4u{ 0x57, 0x57, 0x57 });

			//* text *//

			Color4f text    = static_cast<Color4f>(Color4u{ 0xC0, 0xC0, 0xC0 });
			Color4f textDim = static_cast<Color4f>(Color4u{ 0x9A, 0x9A, 0x9A }); //!< Panel(242424)上でコントラスト比 4.5:1 以上を保つため 9A 以上にする.

			//* icon *//

			Color4f iconDim = static_cast<Color4f>(Color4u{ 0x8A, 0x8A, 0x8A }); //!< 単色・線画のアイコン.
			Color4f close   = static_cast<Color4f>(Color4u{ 0xEF, 0x35, 0x35 });

			//* tooltip *//

			Color4f tooltipBg     = static_cast<Color4f>(Color4u{ 0x2F, 0x2F, 0x2F }); //!< (Header) 不透明. alphaを付けない.
			Color4f tooltipBorder = static_cast<Color4f>(Color4u{ 0x4C, 0x4C, 0x4C });

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Metrics structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 余白・寸法.
		//! @note UE5 は余白を重視する. 余白と高さは 4px grid (2 / 4 / 8 / 16 / 24 / 32) に揃える.
		struct Metrics {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			//* spacing *//

			float paddingM = 8.0f;
			float marginL  = 8.0f; //!< メニュー項目の左右余白にも使われるため, 密度を保つ目的で 16 にはしない.

			//* border *//

			float borderThin  = 1.0f;  //!< 非常に細い線.
			float borderThick = 2.0f;  //!< 強調時のみ.

			//* font *//

			float fontBody    = 13.0f; //!< 本文 10pt ≒ 13px.
			float fontHeading = 18.0f; //!< 見出しのみ 14pt 相当.

			//* bars *//

			float titleBarHeight = 32.0f;
			float menuBarHeight  = 24.0f;

			//* tabs *//

			float tabBarHeight     = 24.0f; //!< UE5 の Major Tab の見た目.
			float tabGap           = 2.0f;  //!< タブ同士の隙間.
			float tabRounding      = 4.0f;  //!< 上端だけ丸める.
			float tabAccentH       = 2.0f;  //!< フォーカス中タブ上端の色帯.
			float tabUnderlineSize = 1.0f;  //!< タブバー下端の線の太さ（0 で消える）.
			float tabCloseSize     = 12.0f; //!< × ボタン.
			float tabPaddingX      = 8.0f;

			//* splitter *//

			float dividerSize = 4.0f; //!< 仕切りの見た目の幅.

			//* tooltip *//

			float tooltipDelay   = 0.5f; //!< 表示までの秒数.
			float tooltipPadding = 8.0f;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static Style& GetStyle();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Colors colors;
		Metrics metrics;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
