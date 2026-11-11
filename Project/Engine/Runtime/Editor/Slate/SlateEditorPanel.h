#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateTypes.h"
#include "Docking/SlateDockPanel.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Color4.h>

//* c++
#include <cstdint>
#include <memory>
#include <optional>
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

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class EditorBase; //!< EditorBaseがこのheaderをincludeするため前方宣言に留める.

	////////////////////////////////////////////////////////////////////////////////////////////
	// EditorPanel class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ImGuiでpanelの中身を描くための基底クラス.
	//! @note 継承してOnDraw()にImGuiの呼び出しを書く. Begin / Endは基底が面倒を見るので,
	//!       OnDraw()の中はwidgetを並べるだけでよい.
	//!
	//! ex.
	//! @code
	//! class TestEditor final
	//! 	: public Editor::Slate::EditorPanel {
	//! public:
	//! 	TestEditor() { SetTitle(std::format("{} Test", Editor::Slate::Icon::Home)); }
	//!
	//! 	void OnDraw() override {
	//! 		ImGui::Text("count: %d", count_);
	//! 		if (ImGui::Button("increment")) { ++count_; }
	//! 	}
	//!
	//! private:
	//! 	int32_t count_ = 0;
	//! };
	//! @endcode
	//!
	//! 登録は SlateEditorUnit::AddPanel<TestEditor>(State::Global) で行う.
	//! 他panelのOnDraw / OnUpdateの中からも GetEditor()->AddPanel<T>(state) で追加できる.
	//!
	//! @note このクラスは必ず std::shared_ptr で保持すること.
	//!       widgetへ渡すcallbackは weak_ptr を握るため, 破棄後に呼ばれても安全になる.
	//!       生ポインタやスタック上のインスタンスで使うと dangling する.
	class EditorPanel
		: public std::enable_shared_from_this<EditorPanel> {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// State enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief panelのinstanceの持ち方. 登録時(EditorBase::AddPanel / RegisterPanel)に引数で決める.
		enum class State : uint8_t {
			Global, //!< 型ごとにinstanceを1つしか持たない. EditorBase::GetPanel<T>()で取得可能.
			Pinned, //!< instanceを複数持つ. 取得不可. tabを閉じると破棄される.
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		EditorPanel() noexcept          = default;
		virtual ~EditorPanel() noexcept = default;

		//* panel methods *//

		//! @brief panelの中身を描く.
		//! @note ImGuiのframe内かつ描画領域(BeginRegion)の内側で呼ばれる.
		//!       ImGui::Begin / End は呼ばないこと.
		virtual void OnDraw() = 0;

		//! @brief 毎frame呼ばれる. 描画に依存しない更新に使う.
		//! @note tabが裏に隠れていても呼ばれる. 描画は OnDraw() 側で行うこと.
		virtual void OnUpdate() {}

		//! @brief tabが生成された直後に一度呼ばれる.
		virtual void OnOpen() {}

		//! @brief tabが閉じられる直前に一度呼ばれる.
		virtual void OnClose() {}

		//* title option *//

		//! @brief tabに表示する名前を設定する.
		//! @note UTF-8として扱う. iconは文字列の一部として埋め込める.
		//!       ex. SetTitle(std::format("{} Outliner", Slate::Icon::Hierarchy))
		void SetTitle(const std::string_view& title);

		const std::string& GetTitle() const { return title_; }

		//* tab option *//

		//! @brief tabに × を出すかどうか.
		void SetClosable(bool isClosable);

		bool IsClosable() const { return closable_; }

		//! @brief tabの種別色.
		void SetTabColor(const Color4f& color);

		//* size option *//

		//! @brief 希望サイズ. layoutがAutoのときに使われる.
		void SetDesiredSize(const Vector2f& size);

		//* dock panel option *//

		//! @brief このpanelに対応するDockPanelを取得する. 無ければ生成する.
		//! @note treeへ組み込むのはこれ. 何度呼んでも同じ実体を返す.
		const DockPanelPointer& GetDockPanel();

		//! @brief DockPanelが既に生成されているか.
		bool HasDockPanel() const { return dockPanel_ != nullptr; }

		//* editor option *//

		//! @brief このpanelを所有しているeditor.
		//! @note 登録時に設定されるため, constructorの中ではnullptr.
		EditorBase* GetEditor() const { return editor_; }

		//! @brief 登録時に指定されたinstanceの持ち方.
		State GetState() const { return state_; }

		//* operator [copy] <EditorPanel> (delete) *//

		EditorPanel(const EditorPanel&)            = delete;
		EditorPanel& operator=(const EditorPanel&) = delete;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		friend class EditorBase; //!< editor_ / state_ は登録時にEditorBaseが設定する.

		EditorBase* editor_ = nullptr; //!< 所有しているeditor. (所有はeditor側)

		State state_ = State::Global;

		DockPanelPointer dockPanel_ = nullptr; //!< 遅延生成. GetDockPanel()で作る.

		std::string title_ = "Panel";

		bool closable_ = true;

		//!< 明示的に設定されたときだけDockPanelの既定を上書きする.
		//!< note: constructorはGetDockPanel()より先に走るため, ここで保持しておかないと
		//!<       生成前に設定した色が捨てられる.
		std::optional<Color4f> tabColor_ = std::nullopt;

		Vector2f desiredSize_ = { 0.0f, 0.0f }; //!< 0ならImGuiWidgetの既定に任せる.

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief DockPanelとImGuiWidgetを組み立てる.
		void CreateDockPanel();

	};

	using EditorPanelPointer = std::shared_ptr<EditorPanel>;

}

SXAVENGER_ENGINE_NAMESPACE_END
