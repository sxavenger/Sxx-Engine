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

			Color4f background   = static_cast<Color4f>(Color4ui{ 0x20, 0x20, 0x20 }); //!< 分割の溝.
			Color4f panel        = static_cast<Color4f>(Color4ui{ 0x2D, 0x2D, 0x30 }); //!< パネル面.
			Color4f titleBar     = static_cast<Color4f>(Color4ui{ 0x25, 0x25, 0x26 });
			Color4f menuBar      = static_cast<Color4f>(Color4ui{ 0x2D, 0x2D, 0x30 });
			Color4f tabBar       = static_cast<Color4f>(Color4ui{ 0x25, 0x25, 0x26 });
			Color4f tabActive    = static_cast<Color4f>(Color4ui{ 0x2D, 0x2D, 0x30 }); //!< パネルと地続きに見せる.
			Color4f tabUnderline = static_cast<Color4f>(Color4ui{ 0x3F, 0x3F, 0x46 }); //!< タブバーの下線.

			//* lines *//

			Color4f border = static_cast<Color4f>(Color4ui{ 0x3F, 0x3F, 0x46 });

			//* hover / accent *//

			Color4f hover      = static_cast<Color4f>(Color4ui{ 0x3E, 0x3E, 0x42 });
			Color4f selection  = static_cast<Color4f>(Color4ui{ 0xFF, 0x8A, 0x1E });       //!< 選択（橙）.
			Color4f active     = static_cast<Color4f>(Color4ui{ 0x00, 0x7A, 0xCC });       //!< 実行中（青）.
			Color4f activeSoft = static_cast<Color4f>(Color4ui{ 0x00, 0x7A, 0xCC, 0x40 }); //!< 実行中（青）.

			//* controls *//

			Color4f buttonHover = static_cast<Color4f>(Color4ui{ 0x3E, 0x3E, 0x42 });

			//* text *//

			Color4f text    = static_cast<Color4f>(Color4ui{ 0xDC, 0xDC, 0xDC });
			Color4f textDim = static_cast<Color4f>(Color4ui{ 0x9A, 0x9A, 0x9E });

			//* icon *//

			Color4f iconDim = static_cast<Color4f>(Color4ui{ 0x8A, 0x8A, 0x8E }); //!< 単色・線画のアイコン.
			Color4f close   = static_cast<Color4f>(Color4ui{ 0xC4, 0x2B, 0x2B });

			//* tooltip *//

			Color4f tooltipBg     = static_cast<Color4f>(Color4ui{ 0x30, 0x30, 0x38, 0xE0 });
			Color4f tooltipBorder = static_cast<Color4f>(Color4ui{ 0x50, 0x50, 0x58 });

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Metrics structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 余白・寸法.
		//! @note UE5 は余白を重視する. Padding 4〜8px / Margin 4〜10px.
		struct Metrics {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			//* spacing *//

			float paddingM = 6.0f;
			float marginL  = 10.0f;

			//* border *//

			float borderThin  = 1.0f;  //!< 非常に細い線.
			float borderThick = 2.0f;  //!< 強調時のみ.

			//* font *//

			float fontBody    = 13.0f; //!< 本文 10pt ≒ 13px.
			float fontHeading = 18.0f; //!< 見出しのみ 14pt 相当.

			//* bars *//

			float titleBarHeight = 30.0f;
			float menuBarHeight  = 22.0f;

			//* tabs *//

			float tabBarHeight     = 26.0f; //!< UE5 の Major Tab の見た目.
			float tabGap           = 2.0f;  //!< タブ同士の隙間.
			float tabRounding      = 3.0f;  //!< 上端だけ丸める.
			float tabAccentH       = 2.0f;  //!< フォーカス中タブ上端の色帯.
			float tabUnderlineSize = 1.0f;  //!< タブバー下端の線の太さ（0 で消える）.
			float tabIconSize      = 16.0f; //!< タブ名の前に置く Material Icon の描画サイズ.
			float tabIconOffsetY   = 0.0f;  //!< アイコンの縦位置の微調整（+で下へ）.
			float tabIconGap       = 3.0f;  //!< アイコンとタブ名の間隔.
			float tabCloseSize     = 12.0f; //!< × ボタン.
			float tabPaddingX      = 8.0f;

			//* splitter *//

			float dividerSize = 6.0f; //!< 仕切りの見た目の幅.

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
