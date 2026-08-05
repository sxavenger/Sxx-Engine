#include "Uuid.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/NumericConverter.h>

//* windows
#include <objbase.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Uuid structure methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string Uuid::Serialize() const noexcept {
	return std::format("{:08x}-{:04x}-{:04x}-{:04x}-{:012x}", data1, data2, data3, data4, data5);
	//!< "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"の形式で文字列化する.
}

Uuid Uuid::Generate() {

	GUID guid = {};
	auto hr = CoCreateGuid(&guid);
	StreamLogger::Assert(SUCCEEDED(hr), "failed to generate guid.");

	Uuid uuid = {};
	uuid.data1 = guid.Data1;
	uuid.data2 = guid.Data2;
	uuid.data3 = guid.Data3;

	for (uint8_t i = 0; i < 2; ++i) {
		uuid.data4 <<= 8;
		uuid.data4 |= guid.Data4[i];
	}

	for (uint8_t i = 2; i < 8; ++i) {
		uuid.data5 <<= 8;
		uuid.data5 |= guid.Data4[i];
	}

	return uuid;
}

Uuid Uuid::Deserialize(const std::string_view& data) {
	Uuid uuid = {};

	std::optional<uint32_t> data1 = NumericConverter::ConvertIntegral<uint32_t>(data.substr(0, 8), 16);
	std::optional<uint16_t> data2 = NumericConverter::ConvertIntegral<uint16_t>(data.substr(9, 4), 16);
	std::optional<uint16_t> data3 = NumericConverter::ConvertIntegral<uint16_t>(data.substr(14, 4), 16);
	std::optional<uint16_t> data4 = NumericConverter::ConvertIntegral<uint16_t>(data.substr(19, 4), 16);
	std::optional<uint64_t> data5 = NumericConverter::ConvertIntegral<uint64_t>(data.substr(24, 12), 16);
	StreamLogger::Assert(data1.has_value() && data2.has_value() && data3.has_value() && data4.has_value() && data5.has_value(), "failed to deserialize uuid.");

	uuid.data1 = data1.value();
	uuid.data2 = data2.value();
	uuid.data3 = data3.value();
	uuid.data4 = data4.value();
	uuid.data5 = data5.value();

	return uuid;
}
