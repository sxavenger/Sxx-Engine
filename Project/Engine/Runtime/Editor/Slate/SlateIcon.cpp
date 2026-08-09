#include "SlateIcon.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// IconGlyph structure
////////////////////////////////////////////////////////////////////////////////////////////

Slate::IconGlyph::Data Slate::IconGlyph::Encode(Icon icon) noexcept {

	uint32_t codepoint = static_cast<uint32_t>(icon);

	Data data{};

	if (codepoint < 0x80) {
		data[0] = static_cast<char>(codepoint);

	} else if (codepoint < 0x800) {
		data[0] = static_cast<char>(0xC0 | (codepoint >> 6));
		data[1] = IconGlyph::Continuation(codepoint);

	} else if (codepoint < 0x10000) {
		data[0] = static_cast<char>(0xE0 | (codepoint >> 12));
		data[1] = IconGlyph::Continuation(codepoint >> 6);
		data[2] = IconGlyph::Continuation(codepoint);

	} else {
		data[0] = static_cast<char>(0xF0 | (codepoint >> 18));
		data[1] = IconGlyph::Continuation(codepoint >> 12);
		data[2] = IconGlyph::Continuation(codepoint >> 6);
		data[3] = IconGlyph::Continuation(codepoint);
	}

	return data;
}

char Slate::IconGlyph::Continuation(uint32_t value) noexcept {
	return static_cast<char>(0x80 | (value & 0x3F));
}
