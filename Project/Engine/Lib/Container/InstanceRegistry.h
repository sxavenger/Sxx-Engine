#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/TypeInfo.h>

//* c++
#include <unordered_map>
#include <memory>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// InstanceRegistry class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename Base>
class InstanceRegistry final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <typename Derived> requires std::derived_from<Derived, Base>
	void Insert();

	template <typename Derived> requires std::derived_from<Derived, Base>
	bool Contains() const;

	template <typename Derived> requires std::derived_from<Derived, Base>
	Derived* Get() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<TypeInfo, std::unique_ptr<Base>> container_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <typename Derived> requires std::derived_from<Derived, Base>
	static Derived* Cast(Base* ptr) { return static_cast<Derived*>(ptr); }

};

////////////////////////////////////////////////////////////////////////////////////////////
// InstanceRegistry class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename Base>
template <typename Derived> requires std::derived_from<Derived, Base>
inline void InstanceRegistry<Base>::Insert() {
	container_.emplace(TypeInfo::GetType<Derived>(), std::make_unique<Derived>());
}

template <typename Base>
template <typename Derived> requires std::derived_from<Derived, Base>
inline bool InstanceRegistry<Base>::Contains() const {
	return container_.contains(TypeInfo::GetType<Derived>());
}

template<typename Base>
template<typename Derived> requires std::derived_from<Derived, Base>
inline Derived* InstanceRegistry<Base>::Get() const {

	constexpr TypeInfo info = TypeInfo::GetType<Derived>();

	if (!container_.contains(info)) {
		return nullptr;
	}

	return Cast<Derived>(container_.at(info).get());
}
