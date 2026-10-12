#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

//* slate [style]
#include "../Style/SlateStyle.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ChromeBand class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 帯状のchrome(statusBarなど)に使う葉のwidget.
	//! @note 背景と区切り線はSlate側(renderer->DrawRect / DrawLine)で描き, 中身は自前で開いたImGui
	//!       ウィンドウで描く. (ImGuiMenuBarと同じ手法)
	//! @note ImGuiWidgetは使わない. ImGuiWidgetはBeginRegionでWindowPaddingが効いてしまい,
	//!       24px等の高さに縦が収まらずスクロールバーが出るため.
	//! @note 現在の利用箇所はstatusBar(Separator::Top)のみ. Separator::Bottomはwidget単体のAPIとして残している.
	class ChromeBand final
		: public Widget {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Separator enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 区切り線を引く辺.
		enum class Separator : uint8_t {
			Top,
			Bottom
		};

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		using FDrawCallback = std::function<void()>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		ChromeBand(float height, Separator separator, FDrawCallback draw)
			: height_(height), separator_(separator), draw_(std::move(draw)) {
		}

		//* widget option *//

		Vector2f ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const override;

		int32_t OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		float height_;
		Separator separator_;
		FDrawCallback draw_;

		//!< ImGuiウィンドウのID. アドレスから一度だけ作る(毎frame変えると状態が引き継がれない).
		mutable std::string id_;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
