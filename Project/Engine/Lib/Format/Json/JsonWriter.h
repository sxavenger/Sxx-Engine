#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* json
#include "JsonNode.h"

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class JsonWriter final {
public:
	static_assert(false, "unsupported json type.");
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class <Concept::Integral>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Integral T>
class JsonWriter<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Integral型の作成.
	static json::node Create(const T& value) {
		return JsonNode::CreateIntegral<T>(value);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class <Concept::FloatingPoint>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::FloatingPoint T>
class JsonWriter<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief FloatingPoint型の作成.
	static json::node Create(const T& value) {
		return JsonNode::CreateFloatingPoint<T>(value);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class <Concept::Boolean>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Boolean T>
class JsonWriter<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Boolean型の作成.
	static json::node Create(const T& value) {
		return JsonNode::CreateBoolean(value);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class <Concept::StringA>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::StringA T>
class JsonWriter<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief StringA型の作成.
	static json::node Create(const T& value) {
		return JsonNode::CreateString(value);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class <Concept::Enum>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Enum T>
class JsonWriter<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Enum型の作成.
	static json::node Create(const T& value) {
		std::string_view name = EnumUtil<T>::GetName(value);
		return JsonNode::CreateString(name);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonWriter class <std::filesystem::path>
////////////////////////////////////////////////////////////////////////////////////////////
template <>
class JsonWriter<std::filesystem::path> final {
public:

	//! @brief std::filesystem::pathの作成.
	static json::node Create(const std::filesystem::path& value) {
		return JsonNode::CreateString(value.generic_string());
	}

};



