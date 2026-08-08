#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* framework
#include "Pipeline.h"
#include "IUnit.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/TypeInfo.h>
#include <Lib/Pointer/ReferencePointer.h>

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Framework)

////////////////////////////////////////////////////////////////////////////////////////////
// Context class
////////////////////////////////////////////////////////////////////////////////////////////
class Context final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// UnitPool structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct UnitPool {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		template <Unit T>
		void Push();

		template <Unit T>
		RefPtr<T> Get() const;

		template <Unit T>
		bool Contains() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::list<std::unique_ptr<IUnit>> units_; //!< Unitを格納するリスト
		std::unordered_map<TypeInfo, IUnit*> pointers_; 

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* framework option *//

	//! @brief Unitを登録する
	template <Unit T>
	static void Push() { pool_.Push<T>(); }

	//! @brief Pipelineを実行する
	static void Run();

	//* unit option *//

	//! @brief Unitを取得する
	//! @retval T* Unitが存在する場合はUnitのポインタを返却.
	//! @retval nullptr Unitが存在しない場合はnullptrを返却.
	template <Unit T>
	static RefPtr<T> GetUnit() { return pool_.Get<T>(); }

	//! @brief Unitが登録されているかどうかを取得する
	template <Unit T>
	static bool HasUnit() { return pool_.Contains<T>(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline UnitPool pool_; //!< Unitを格納するリスト
	static inline Pipeline pipeline_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// [Context] UnitPool structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Unit T>
void Context::UnitPool::Push() {

	TypeInfo type = TypeInfo::GetType<T>();

	if (pointers_.contains(type)) {
		StreamLogger::Warning("Framework::Context | unit pool already contains unit. type: {}", type.GetName());
		return; //!< すでに登録されている場合は何もしない
	}

	std::unique_ptr<T> unit = std::make_unique<T>();
	pointers_.emplace(type, unit.get());
	units_.push_back(std::move(unit));
	StreamLogger::Info("Framework::Context | unit pool push unit. type: {}", type.GetName());
}

template <Unit T>
RefPtr<T> Context::UnitPool::Get() const {

	TypeInfo type = TypeInfo::GetType<T>();

	if (!pointers_.contains(type)) {
		StreamLogger::Warning("Framework::Context | unit pool does not contain unit. type: {}", type.GetName());
		return nullptr; //!< 登録されていない場合はnullptrを返す
	}

	return static_cast<T*>(pointers_.at(type));
}

template <Unit T>
bool Context::UnitPool::Contains() const {
	return pointers_.contains(TypeInfo::GetType<T>());
}

SXAVENGER_ENGINE_NAMESPACE_END
