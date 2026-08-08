#include "SlateApplication.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Application class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::Application::Tick(Window& window, ImGuiRenderer* renderer, TimePointf<TimeUnit::Second> time) {
	if (renderer == nullptr || !renderer->IsActiveFrame()) {
		return; //!< フレーム外では ImGui の DrawList が取れないため描画しない.
	}

	const Geometry root = MakeRootGeometry(window);

	if (root.localSize.x <= 0.0f || root.localSize.y <= 0.0f) {
		return; //!< 最小化などでクライアント領域が無い場合.
	}

	//!< pass 1. 希望サイズを再帰的に計算してキャッシュする(ボトムアップ).
	//!< 各ウィジェットの ComputeDesiredSize() が自身の子の CacheDesiredSize() を埋めるため, ルートの一回で全段に伝播する.
	window.CacheDesiredSize(window.ComputeDesiredSize(root.scale, renderer));

	//!< pass 2. ルートジオメトリ全体を割り当てて描画する(トップダウン).
	const Widget::PaintArguments arguments = { time };

	window.OnPaint(arguments, root, renderer, 0);
}

void Slate::Application::ProcessMouseMove(const Window& window, const PointerEvent& event) {
	//!< キャプチャ中はヒットテストを行わず, キャプチャ時のジオメトリでキャプチャ先へ直接送る.
	if (const WidgetPointer captor = mouseCaptor_.lock()) {
		const Geometry geometry = captorGeometry_;

		//!< ドラッグ中はカーソル形状を維持する.(仕切りから外れても矢印のまま)
		currentCursor_ = captor->GetCursor(geometry, event.screenPosition);

		//!< ドラッグ中にキャプチャ解放を要求されることがある.(例: タブをタブバーの外へ引き出して切り離すとき)
		const Reply reply = captor->OnMouseMove(geometry, event);
		ApplyReply(reply, geometry);
		return;
	}

	Widget::ArrangedChildren path = {};
	BuildHitPath(window.GetPointer(), MakeRootGeometry(window), event.screenPosition, path);

	const WidgetPointer topmost = path.empty() ? nullptr : path.back().widget;
	const Geometry topGeometry  = path.empty() ? Geometry{} : path.back().geometry;

	//!< ホバー対象が変わったウィジェットへ Leave / Enter を送る.
	if (const WidgetPointer previous = hoveredWidget_.lock(); previous != topmost) {
		if (previous != nullptr) {
			previous->OnMouseLeave();
		}

		if (topmost != nullptr) {
			topmost->OnMouseEnter(topGeometry, event);
		}

		hoveredWidget_ = topmost;
	}

	//!< カーソル形状を更新する.(ヒットパスの葉から根へ辿り, 既定でない最初のものを採用する)
	currentCursor_ = Cursor::Default;

	for (auto itr = path.rbegin(); itr != path.rend(); ++itr) {
		const Cursor cursor = itr->widget->GetCursor(itr->geometry, event.screenPosition);

		if (cursor != Cursor::Default) {
			currentCursor_ = cursor;
			break;
		}
	}

	if (topmost != nullptr) {
		topmost->OnMouseMove(topGeometry, event);
	}
}

void Slate::Application::ProcessMouseButtonDown(const Window& window, const PointerEvent& event) {
	Widget::ArrangedChildren path = {};
	BuildHitPath(window.GetPointer(), MakeRootGeometry(window), event.screenPosition, path);

	//!< 葉から根へバブリングし, 処理されたところで止める.
	for (auto itr = path.rbegin(); itr != path.rend(); ++itr) {
		const Reply reply = itr->widget->OnMouseButtonDown(itr->geometry, event);

		if (reply.IsHandled()) {
			ApplyReply(reply, itr->geometry);
			break;
		}
	}
}

void Slate::Application::ProcessMouseButtonUp(const Window& window, const PointerEvent& event) {
	//!< キャプチャ中はキャプチャ先へ直接送る.
	if (const WidgetPointer captor = mouseCaptor_.lock()) {
		const Geometry geometry = captorGeometry_;

		const Reply reply = captor->OnMouseButtonUp(geometry, event);
		ApplyReply(reply, geometry);
		return;
	}

	Widget::ArrangedChildren path = {};
	BuildHitPath(window.GetPointer(), MakeRootGeometry(window), event.screenPosition, path);

	for (auto itr = path.rbegin(); itr != path.rend(); ++itr) {
		const Reply reply = itr->widget->OnMouseButtonUp(itr->geometry, event);

		if (reply.IsHandled()) {
			ApplyReply(reply, itr->geometry);
			break;
		}
	}
}

void Slate::Application::BuildHitPath(const WidgetPointer& widget, const Geometry& geometry, const Vector2f& point, Widget::ArrangedChildren& out) const {
	if (widget == nullptr || !widget->IsHitTestable()) {
		return;
	}

	if (!geometry.ContainsAbsolute(point)) {
		return;
	}

	out.emplace_back(widget, geometry);

	Widget::ArrangedChildren children = {};
	widget->OnArrangeChildren(geometry, children);

	for (const auto& child : children) {
		BuildHitPath(child.widget, child.geometry, point, out);
	}
}

void Slate::Application::ApplyReply(const Reply& reply, const Geometry& capturedGeometry) {
	if (reply.GetMouseCaptor() != nullptr) {
		mouseCaptor_ = reply.GetMouseCaptor();

		//!< ドラッグ中は配置が変わり得るため, キャプチャ時点のジオメトリを保持して以降のイベントに渡す.
		captorGeometry_ = capturedGeometry;
	}

	if (reply.ShouldReleaseCapture()) {
		mouseCaptor_.reset();
		captorGeometry_ = Geometry{};
	}
}

Slate::Geometry Slate::Application::MakeRootGeometry(const Window& window) {
	Geometry geometry = {};

	geometry.absolutePosition = { 0.0f, 0.0f };
	geometry.localSize        = window.GetClientSize();
	geometry.scale            = window.GetDpiScale();

	return geometry;
}
