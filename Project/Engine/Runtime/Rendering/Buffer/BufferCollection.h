#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "BaseBuffer.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Container/InstanceRegistry.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// BufferCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class BufferCollection final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* instance option *//

	template <Buffer T>
	T* EnsureInstance(const Vector2u& resolution);

	template <Buffer T>
	bool HasInstance() const;

	template <Buffer T>
	T& GetInstance() const;

	//* buffer option *//

	void Resize(const Vector2u& resolution);

	void ClearTarget(Graphics::GraphicsCommandContext& command);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	InstanceRegistry<BaseBuffer> instances_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// BufferCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Buffer T>
T* BufferCollection::EnsureInstance(const Vector2u& resolution) {
	if (!instances_.Contains<T>()) {
		instances_.Insert<T>(); //!< instanceが存在しない場合は生成する.

		T* instance = instances_.Get<T>();
		instance->Create(resolution); //!< instanceを生成する.
	}

	T* instance = instances_.Get<T>();
	instance->Resize(resolution); //!< instanceをリサイズする.

	return instance;
}

template <Buffer T>
bool BufferCollection::HasInstance() const {
	return instances_.Contains<T>();
}

template <Buffer T>
T& BufferCollection::GetInstance() const {
	return *instances_.Get<T>();
}

SXAVENGER_ENGINE_NAMESPACE_END
