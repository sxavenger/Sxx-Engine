#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../SlateTypes.h"
#include "../Renderer/SlateImGuiRenderer.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Time/TimePoint.h>
#include <Lib/Math/Vector2.h>

//* c++
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class Widget;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Widget class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Widget
		: public std::enable_shared_from_this<Widget> {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Arranged structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 親が子に割り当てた配置
		struct Arranged {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			std::shared_ptr<Widget> widget;
			Geometry geometry;

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// PaintArguments structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief Paint() に渡す引数.
		struct PaintArguments {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			TimePointf<TimeUnit::Second> time;

		};

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		using ArrangedChildren = std::vector<Arranged>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Widget() noexcept          = default;
		virtual ~Widget() noexcept = default;

		//* layout pass *//

		virtual Vector2f ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const = 0;

		virtual void OnArrangeChildren(const Geometry& /*allotted*/, ArrangedChildren& /*out*/) const {}

		//* paint opiton *//

		virtual int32_t OnPaint(const PaintArguments& /*arguments*/, const Geometry& /*geometry*/, ImGuiRenderer* /*renderer*/, int32_t /*layer*/) const = 0;

		//* input option *//

		virtual Reply OnMouseButtonDown(const Geometry& /*geometry*/, const PointerEvent& /*event*/) { return Reply::Unhandled(); }

		virtual Reply OnMouseButtonUp(const Geometry& /*geometry*/, const PointerEvent& /*event*/) { return Reply::Unhandled(); }

		virtual Reply OnMouseMove(const Geometry& /*geometry*/, const PointerEvent& /*event*/) { return Reply::Unhandled(); }

		virtual void OnMouseEnter(const Geometry& /*geometry*/, const PointerEvent& /*event*/) {}

		virtual void OnMouseLeave() {}

		virtual Cursor GetCursor(const Geometry& /*geometry*/, Vector2f /*absolutePos*/) const { return Cursor::Default; }

		//* visibility option *//

		Visibility GetVisibility() const { return visibility_; }

		void SetVisibility(Visibility visibility) { visibility_ = visibility; }

		bool IsHitTestable() const { return visibility_ == Visibility::Visible; }

		//* cache option *//

		Vector2f GetCachedDesiredSize() const { return cachedDesiredSize_; }

		void CacheDesiredSize(Vector2f size) { cachedDesiredSize_ = size; }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		Visibility visibility_ = Visibility::Visible;
		Vector2f cachedDesiredSize_{};

	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using WidgetPointer = std::shared_ptr<Widget>;

}

SXAVENGER_ENGINE_NAMESPACE_END
