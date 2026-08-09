#include "SlateStyleEditor.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate [style]
#include "SlateImGuiStyleIO.h"
#include "SlateStyleIO.h"

//* imgui
#include <imgui.h>

//* c++
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// anonymous namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Snapshot structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 編集前の状態(Revert の戻り先).
	//! @note 項目表の並び順で値だけを控える. 名前で引き直さないので, 控えたあとに
	//!       StyleIO の表を編集した場合は「小さい方の要素数」までしか復元しない.
	struct Snapshot {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 現在の値を控える.
		void Capture() {
			const std::span<const Slate::StyleIO::ColorEntry> colorEntries   = Slate::StyleIO::GetColorEntries();
			const std::span<const Slate::StyleIO::MetricEntry> metricEntries = Slate::StyleIO::GetMetricEntries();

			colors.clear();
			colors.reserve(colorEntries.size());
			for (const Slate::StyleIO::ColorEntry& entry : colorEntries) {
				colors.emplace_back(*entry.value);
			}

			metrics.clear();
			metrics.reserve(metricEntries.size());
			for (const Slate::StyleIO::MetricEntry& entry : metricEntries) {
				metrics.emplace_back(*entry.value);
			}

			isValid = true;
		}

		//! @brief 控えた値へ戻す.
		//! @return 書き換えたかどうか(未 Capture なら false).
		bool Restore() const {
			if (!isValid) {
				return false;
			}

			const std::span<const Slate::StyleIO::ColorEntry> colorEntries   = Slate::StyleIO::GetColorEntries();
			const std::span<const Slate::StyleIO::MetricEntry> metricEntries = Slate::StyleIO::GetMetricEntries();

			const size_t colorCount  = std::min(colors.size(), colorEntries.size());
			const size_t metricCount = std::min(metrics.size(), metricEntries.size());

			for (size_t i = 0; i < colorCount; ++i) {
				*colorEntries[i].value = colors[i];
			}

			for (size_t i = 0; i < metricCount; ++i) {
				*metricEntries[i].value = metrics[i];
			}

			return true;
		}

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<Color4f> colors  = {};
		std::vector<float> metrics   = {};

		bool isValid = false;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	Snapshot& GetSnapshot() {
		static Snapshot snapshot = {};
		return snapshot;
	}

	//!< nullptr でも落ちない文字列を返す(項目表の group / description は非 nullptr のはずだが,
	//!< SeparatorText / TextUnformatted は nullptr を受け取れない).
	const char* ToText(const char* text) {
		return (text != nullptr) ? text : "";
	}

	//!< フィルタに一致するか(名前・分類・説明のいずれか).
	//!< ImGuiTextFilter::PassFilter は空フィルタなら常に true, nullptr も安全に扱う.
	bool PassFilter(const ImGuiTextFilter& filter, const char* name, const char* group, const char* description) {
		return filter.PassFilter(name) || filter.PassFilter(group) || filter.PassFilter(description);
	}

	//!< 直前の項目に "(?)" と tooltip を付ける.
	void HelpMarker(const char* description) {
		if (description == nullptr) {
			return;
		}

		ImGui::SameLine();
		ImGui::TextDisabled("(?)");

		if (ImGui::BeginItemTooltip()) {
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 22.0f);
			ImGui::TextUnformatted(description);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//! @brief Colors タブの中身.
	//! @return 値を書き換えたかどうか.
	bool DrawColors(const ImGuiTextFilter& filter, ImGuiColorEditFlags alphaFlags, bool isShowHex) {
		bool isChanged = false;

		//!< BeginChild は戻り値によらず EndChild と対で呼ぶ.
		if (ImGui::BeginChild("##colors", ImVec2(0.0f, 0.0f), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.55f);

			const char* currentGroup = nullptr;

			int32_t index = 0;
			for (const Slate::StyleIO::ColorEntry& entry : Slate::StyleIO::GetColorEntries()) {
				const int32_t id = index++; //!< フィルタで飛ばしても ID が動かないよう, 表の位置で振る.

				if (!PassFilter(filter, entry.name, entry.group, entry.description)) {
					continue;
				}

				//!< 分類の見出し(フィルタ中でもどの分類か分かるように出す).
				if (currentGroup == nullptr || std::strcmp(currentGroup, ToText(entry.group)) != 0) {
					currentGroup = ToText(entry.group);
					ImGui::SeparatorText(currentGroup);
				}

				ImGui::PushID(id);

				//!< Color4f は { r, g, b, a } と std::array<float, 4> の union なので,
				//!< 先頭から 4 つの float がそのまま RGBA として並ぶ. ColorEdit4 には Ptr() で
				//!< 実体を直接渡し, 編集結果をその場へ書き込ませる.
				//!< (Color4floating は単項 operator& を float* を返すよう多重定義しているため
				//!<  &(*entry.value) でも同じ float* になるが, 意図が読めないので Ptr() を使う.
				//!<  Color4f* が必要な場面では std::addressof を使うこと.)
				//!< Style::Colors は sRGB のバイト値をそのまま持っている. linear への変換は
				//!< 描画側(ImGuiStyleIO::SetCurrentStyle / ImGuiRenderer)の役割なので,
				//!< エディタは生の値を触る(変換すると編集値と ToHex() の保存値がずれる).
				if (ImGui::ColorEdit4("##color", entry.value->Ptr(), ImGuiColorEditFlags_AlphaBar | alphaFlags)) {
					isChanged = true;
				}

				//!< 既定値と違えば戻すボタンを出す.
				//!< Color4floating は operator== を持たないので成分ごとに比べる.
				const Color4f current = *entry.value;
				const bool isModified
					= (current.r != entry.def.r)
					|| (current.g != entry.def.g)
					|| (current.b != entry.def.b)
					|| (current.a != entry.def.a);

				if (isModified) {
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::SmallButton("R")) {
						*entry.value = entry.def;
						isChanged    = true;
					}

					if (ImGui::BeginItemTooltip()) {
						ImGui::TextUnformatted("既定値へ戻す");
						ImGui::EndTooltip();
					}
				}

				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::TextUnformatted(ToText(entry.name));

				if (isShowHex) {
					ImGui::SameLine();
					ImGui::TextDisabled("%s", Slate::StyleIO::ToHex(*entry.value).c_str());
				}

				HelpMarker(entry.description);

				ImGui::PopID();
			}

			ImGui::PopItemWidth();
		}

		ImGui::EndChild();

		return isChanged;
	}

	//! @brief Sizes タブの中身.
	//! @return 値を書き換えたかどうか.
	bool DrawMetrics(const ImGuiTextFilter& filter) {
		bool isChanged = false;

		if (ImGui::BeginChild("##sizes", ImVec2(0.0f, 0.0f), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.55f);

			const char* currentGroup = nullptr;

			int32_t index = 0;
			for (const Slate::StyleIO::MetricEntry& entry : Slate::StyleIO::GetMetricEntries()) {
				const int32_t id = index++;

				if (!PassFilter(filter, entry.name, entry.group, entry.description)) {
					continue;
				}

				if (currentGroup == nullptr || std::strcmp(currentGroup, ToText(entry.group)) != 0) {
					currentGroup = ToText(entry.group);
					ImGui::SeparatorText(currentGroup);
				}

				ImGui::PushID(id);

				//!< 秒の項目(TooltipDelay)だけ小数を見せる. それ以外は px なので整数表示でよい.
				const bool isSeconds = (std::strstr(ToText(entry.name), "Delay") != nullptr);

				//!< entry.min / entry.max は windows.h の min / max マクロと同名だが,
				//!< .vcxproj の全構成で NOMINMAX が定義済みなのでメンバとして安全に読める.
				//!< スライダは min / max の範囲でクランプするため, Load() のクランプと同じ範囲になる.
				if (ImGui::SliderFloat("##value", entry.value, entry.min, entry.max, isSeconds ? "%.2f s" : "%.0f px")) {
					isChanged = true;
				}

				if (*entry.value != entry.def) {
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::SmallButton("R")) {
						*entry.value = entry.def;
						isChanged    = true;
					}

					if (ImGui::BeginItemTooltip()) {
						ImGui::Text("既定値 %.2f へ戻す", entry.def);
						ImGui::EndTooltip();
					}
				}

				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::TextUnformatted(ToText(entry.name));

				HelpMarker(entry.description);

				ImGui::PopID();
			}

			ImGui::PopItemWidth();
		}

		ImGui::EndChild();

		return isChanged;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// StyleEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::StyleEditor::Draw() {
	//!< 初回に現在の状態を控える(Revert の戻り先).
	if (!GetSnapshot().isValid) {
		GetSnapshot().Capture();
	}

	static ImGuiTextFilter filter;
	static ImGuiColorEditFlags alphaFlags = ImGuiColorEditFlags_AlphaPreviewHalf;
	static bool isShowHex                 = true;

	//!< Style を書き換えたか. 書き換えた場合はこの関数の最後で ImGui 側の style へ写し直す.
	bool isChanged = false;

	//* commands *//

	if (ImGui::Button("Reset to default")) {
		StyleIO::ResetToDefault();
		isChanged = true;
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::TextUnformatted("すべての項目を既定値へ戻す");
		ImGui::EndTooltip();
	}

	ImGui::SameLine();
	if (ImGui::Button("Save Ref")) {
		GetSnapshot().Capture();
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::TextUnformatted("現在の状態を控える(Revert の戻り先)");
		ImGui::EndTooltip();
	}

	ImGui::SameLine();
	if (ImGui::Button("Revert")) {
		if (GetSnapshot().Restore()) {
			isChanged = true;
		}
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::TextUnformatted("控えた状態へ戻す");
		ImGui::EndTooltip();
	}

	ImGui::SameLine();
	if (ImGui::Button("Copy JSON")) {
		//!< StyleIO::Save() / dump() の返す文字列は式の終わりまで生きるのでそのまま渡してよい.
		ImGui::SetClipboardText(StyleIO::Save().dump(2).c_str());
	}
	if (ImGui::BeginItemTooltip()) {
		ImGui::TextUnformatted("現在のスタイルを JSON でクリップボードへ");
		ImGui::EndTooltip();
	}

	//* filter *//

	filter.Draw("Filter", ImGui::GetFontSize() * 12.0f);
	ImGui::SameLine();
	ImGui::Checkbox("Hex", &isShowHex);
	HelpMarker("色の 16 進表記(StyleIO::ToHex の保存形式)を並べて表示する");

	ImGui::Separator();

	//* tabs *//

	if (ImGui::BeginTabBar("##style_editor_tabs")) {
		if (ImGui::BeginTabItem("Colors")) {
			//!< アルファの見せ方(ImGui のスタイルエディタと同じ選択肢).
			if (ImGui::RadioButton("Opaque", alphaFlags == ImGuiColorEditFlags_AlphaOpaque)) {
				alphaFlags = ImGuiColorEditFlags_AlphaOpaque;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Alpha", alphaFlags == 0)) {
				alphaFlags = 0;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Half", alphaFlags == ImGuiColorEditFlags_AlphaPreviewHalf)) {
				alphaFlags = ImGuiColorEditFlags_AlphaPreviewHalf;
			}
			HelpMarker("半透明色(ActiveSoft / TooltipBg)の見え方を切り替える");

			if (DrawColors(filter, alphaFlags, isShowHex)) {
				isChanged = true;
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Sizes")) {
			//!< FontBody を変えても font atlas は作り直されない.
			//!< ImGuiRenderer::LoadFont() は Init() でしか走らないため, atlas の基準サイズは
			//!< 起動時の FontBody のまま. Slate 側の DrawTextA / MeasureTextA は毎回サイズを
			//!< 渡すので寸法自体は追従するが, 基準から離すほど拡縮されて字がぼやける.
			ImGui::TextDisabled("変更は即座に反映されます(FontBody は font atlas の基準サイズを変えません)");

			if (DrawMetrics(filter)) {
				isChanged = true;
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	//* apply to imgui *//

	//!< ImGui の style は Slate の style から作った写しなので, 書き換えたら写し直す.
	//!< これを呼ばないと ImGui のウィジェット(ボタン・スライダ・ポップアップ)の色と
	//!< 余白が古いまま残る. Reset / Revert / 個別の編集すべてがここを通る.
	//!< ImGuiRenderer::BeginRegion は WindowRounding / WindowBorderSize / WindowMinSize を
	//!< push しているが, 前 2 つは SetCurrentStyle が書く値と同じ 0 で, WindowMinSize は
	//!< SetCurrentStyle が触らないため, EndRegion の pop でここでの書き換えは壊れない.
	if (isChanged) {
		ImGuiStyleIO::SetCurrentStyle(ImGui::GetStyle());
	}
}
