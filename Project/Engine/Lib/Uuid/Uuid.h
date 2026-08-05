#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Uuid structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Uuid {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Uuid() noexcept = default;
	Uuid(uint32_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint64_t d5) noexcept : data1(d1), data2(d2), data3(d3), data4(d4), data5(d5) {}

	//* operator [copy / move] <Uuid> *//

	Uuid(const Uuid&) noexcept            = default;
	Uuid& operator=(const Uuid&) noexcept = default;

	Uuid(Uuid&&) noexcept            = default;
	Uuid& operator=(Uuid&&) noexcept = default;

	//* operator [comparison] <Uuid> *//

	bool operator==(const Uuid& other) const noexcept { return data1 == other.data1 && data2 == other.data2 && data3 == other.data3 && data4 == other.data4 && data5 == other.data5; }
	bool operator!=(const Uuid& other) const noexcept { return data1 != other.data1 || data2 != other.data2 || data3 != other.data3 || data4 != other.data4 || data5 != other.data5; }

	//* serialization methods *//

	std::string Serialize() const noexcept;

	static Uuid Deserialize(const std::string_view& data);

	//* generation methods *//

	static Uuid Generate();

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint32_t data1 = NULL; //!< first part  - 32 bits
	uint16_t data2 = NULL; //!< second part - 16 bits
	uint16_t data3 = NULL; //!< third part  - 16 bits
	uint16_t data4 = NULL; //!< fourth part - 16 bits
	uint64_t data5 = NULL; //!< fifth part - 64 bits
	//!< "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"の形式で各部分を整数型として保持する.
	//!< "data1" - "data2" - "data3" - "data4" - "data5" と定義する.
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// std::hash <Uuid>
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct std::hash<Uuid> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* operator [hash] <Uuid> *//

	std::size_t operator()(const Uuid& uuid) const noexcept {
		// ハッシュ結合（Boostのhash_combine風）
		std::size_t seed = 0;
		seed ^= std::hash<uint32_t>{}(uuid.data1) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<uint16_t>{}(uuid.data2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<uint16_t>{}(uuid.data3) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<uint16_t>{}(uuid.data4) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<uint64_t>{}(uuid.data5) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		return seed;
	}
};
