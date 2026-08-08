#include "SlateDockLayout.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <fstream>
#include <iterator>
#include <memory>
#include <system_error>
#include <utility>

////////////////////////////////////////////////////////////////////////////////////////////
// anonymous namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	using Json = Slate::DockLayout::Json;

	//! @brief objectからkeyのnodeを探す. 無ければnullptr.
	const Json* FindMember(const Json& node, const char* key) {
		if (!node.is_object()) {
			return nullptr;
		}

		const auto itr = node.find(key);
		if (itr == node.end()) {
			return nullptr;
		}

		return std::addressof(*itr);
	}

	//!< 壊れたjsonでも落ちないように, 型が違えば既定値を返す(例外を投げない).

	std::string GetString(const Json& node, const char* key, const std::string& defaultValue) {
		const Json* member = FindMember(node, key);
		return (member != nullptr && member->is_string()) ? member->get<std::string>() : defaultValue;
	}

	bool GetBoolean(const Json& node, const char* key, bool defaultValue) {
		const Json* member = FindMember(node, key);
		return (member != nullptr && member->is_boolean()) ? member->get<bool>() : defaultValue;
	}

	int32_t GetIntegral(const Json& node, const char* key, int32_t defaultValue) {
		const Json* member = FindMember(node, key);
		return (member != nullptr && member->is_number_integer()) ? member->get<int32_t>() : defaultValue;
	}

	//! @brief nodeからfloatを取得する.
	//! @note 判定は is_number_float() ではなく is_number() を使う.
	//!       前者だと "ratio": 1 のような整数リテラルを弾いてしまうため(SlateStyleIOと同じ流儀).
	float GetFloatingPoint(const Json& node, const char* key, float defaultValue) {
		const Json* member = FindMember(node, key);
		return (member != nullptr && member->is_number()) ? member->get<float>() : defaultValue;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// DockLayout class methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::DockLayout::Json Slate::DockLayout::NodeToJson(const WidgetPointer& node) {
	if (!node) {
		return Json(nullptr);
	}

	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(node)) {
		Json panels = Json::array();

		for (const auto& panel : stack->Panels()) {
			if (panel) {
				panels.push_back(panel->GetTitle());
			}
		}

		return Json{
			{ "type",   "tabs" },
			{ "panels", std::move(panels) },
			{ "active", stack->GetActiveIndex() < 0 ? 0 : stack->GetActiveIndex() },
		};
	}

	if (auto split = std::dynamic_pointer_cast<Splitter>(node)) {
		return Json{
			{ "type",        "splitter" },
			{ "orientation", split->GetOrientation() == Splitter::Orientation::Horizontal ? "horizontal" : "vertical" },
			{ "ratio",       split->GetRatio() },
			{ "first",       NodeToJson(split->First()) },
			{ "second",      NodeToJson(split->Second()) },
		};
	}

	return Json(nullptr); //!< ドックノード以外は保存しない.
}

Slate::WidgetPointer Slate::DockLayout::NodeFromJson(const Json& value, const PanelFactory& factory) {
	if (!value.is_object()) {
		return nullptr;
	}

	const std::string type = GetString(value, "type", "");

	if (type == "tabs") {
		auto stack = std::make_shared<DockTabStack>();

		if (const Json* panels = FindMember(value, "panels"); panels != nullptr && panels->is_array()) {
			for (const auto& item : *panels) {
				if (!item.is_string()) {
					continue;
				}

				//!< 未知の ID は作れないので飛ばす(レイアウトは壊さない).
				if (DockPanelPointer panel = factory ? factory(item.get<std::string>()) : nullptr) {
					stack->AddPanel(std::move(panel));
				}
			}
		}

		if (stack->IsEmpty()) {
			return nullptr;
		}

		const int32_t active = GetIntegral(value, "active", 0);
		if (active > 0 && active < static_cast<int32_t>(stack->Count())) {
			stack->SetActiveIndex(active);
		}

		return stack;
	}

	if (type == "splitter") {
		const Json* firstValue  = FindMember(value, "first");
		const Json* secondValue = FindMember(value, "second");

		WidgetPointer first  = (firstValue  != nullptr) ? NodeFromJson(*firstValue,  factory) : nullptr;
		WidgetPointer second = (secondValue != nullptr) ? NodeFromJson(*secondValue, factory) : nullptr;

		//!< 片側が復元できなかった場合は分割せず残った方を返す.
		if (!first && !second) {
			return nullptr;
		}
		if (!first) {
			return second;
		}
		if (!second) {
			return first;
		}

		const auto orientation =
			(GetString(value, "orientation", "horizontal") == "vertical")
				? Splitter::Orientation::Vertical
				: Splitter::Orientation::Horizontal;

		auto split = std::make_shared<Splitter>(orientation, std::move(first), std::move(second));
		//!< ratio は ctor ではなく SetRatio を通す. ctor は clamp しないため,
		//!< 壊れた layout file の 0 / 1 / 負 がそのまま入って片側が潰れる.
		split->SetRatio(GetFloatingPoint(value, "ratio", 0.5f));

		return split;
	}

	return nullptr;
}

std::string Slate::DockLayout::Serialize(const std::vector<WindowLayout>& windows) {
	Json array = Json::array();

	for (const WindowLayout& window : windows) {
		array.push_back(Json{
			{ "title",     window.title     },
			{ "isMain",    window.isMain    },
			{ "x",         window.x         },
			{ "y",         window.y         },
			{ "width",     window.width     },
			{ "height",    window.height    },
			{ "maximized", window.maximized },
			{ "root",      NodeToJson(window.root) },
		});
	}

	const Json root{
		{ "version", kFormatVersion   },
		{ "windows", std::move(array) },
	};

	return root.dump(1, '\t'); //!< engineのjson出力と同じくtab整形.
}

bool Slate::DockLayout::Deserialize(const std::string_view& jsonText, const PanelFactory& factory, std::vector<WindowLayout>& out) {
	//!< 壊れたファイルでも例外を投げずに false を返す.
	const Json root = Json::parse(jsonText.begin(), jsonText.end(), nullptr, false);

	if (root.is_discarded() || !root.is_object()) {
		return false;
	}
	if (GetIntegral(root, "version", 0) != kFormatVersion) {
		return false;
	}

	const Json* windows = FindMember(root, "windows");
	if (windows == nullptr || !windows->is_array()) {
		return false;
	}

	out.clear();

	for (const auto& item : *windows) {
		if (!item.is_object()) {
			continue;
		}

		WindowLayout window;
		window.title     = GetString(item, "title", "");
		window.isMain    = GetBoolean(item, "isMain", false);
		window.x         = GetIntegral(item, "x", 0);
		window.y         = GetIntegral(item, "y", 0);
		window.width     = GetIntegral(item, "width", 1280);
		window.height    = GetIntegral(item, "height", 720);
		window.maximized = GetBoolean(item, "maximized", false);

		if (const Json* rootValue = FindMember(item, "root"); rootValue != nullptr) {
			window.root = NodeFromJson(*rootValue, factory);
		}

		//!< 中身が1枚も復元できなかったウィンドウは作らない(mainだけは残す).
		if (!window.root && !window.isMain) {
			continue;
		}

		out.push_back(std::move(window));
	}

	return !out.empty();
}

bool Slate::DockLayout::SaveToFile(const std::filesystem::path& filepath, const std::vector<WindowLayout>& windows) {
	std::error_code ec;
	if (filepath.has_parent_path()) {
		std::filesystem::create_directories(filepath.parent_path(), ec); //!< 無ければ作る.
	}

	//!< binaryで開いて改行変換を避ける(UTF-8をそのまま書く).
	std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
	if (!file) {
		return false;
	}

	const std::string text = Serialize(windows);
	file.write(text.data(), static_cast<std::streamsize>(text.size()));

	return file.good();
}

bool Slate::DockLayout::LoadFromFile(const std::filesystem::path& filepath, const PanelFactory& factory, std::vector<WindowLayout>& out) {
	std::ifstream file(filepath, std::ios::binary);
	if (!file) {
		return false;
	}

	const std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return Deserialize(text, factory, out);
}
