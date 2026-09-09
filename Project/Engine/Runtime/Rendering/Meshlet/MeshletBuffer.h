#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/DimensionBuffer.h>

//* lib
#include <Lib/Math/Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// MeshletBuffer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshletBuffer final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Meshlet structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Meshlet { //!< [meshopt_Meshlet参照] メッシュレットの頂点/三角形のオフセットと数
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t vertexOffset;
		uint32_t triangleOffset;
		uint32_t vertexCount;
		uint32_t triangleCount;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Triangle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Triangle { //!< メッシュレット内の1三角形を構成するローカル頂点index(各10bit)
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t i0 : 10;
		uint32_t i1 : 10;
		uint32_t i2 : 10;
		//!< 1頂点当たり10bitで表現 計30bit(約uint32_t)

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Bounds structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Bounds {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector3f center; //!< bounding sphereの中心
		float radius;    //!< bounding sphereの半径

		Vector3<int8_t> coneAxis; //!< normal coneの軸 (SNORM8, x / 127.0fでデコード)
		int8_t coneCutoff;        //!< normal coneのcutoff (SNORM8, x / 127.0fでデコード)
		//!< cone apexは保持しない. bounding sphereを使う判定式で代用する.
		
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* buffer option *//

	void SetName(const std::string_view& name) const;
	void SetName(const std::wstring_view& name) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* Buffer *//

	Graphics::DimensionBuffer<Meshlet> meshlets;
	Graphics::DimensionBuffer<Triangle> triangles;
	Graphics::DimensionBuffer<uint32_t> vertexIndices;
	Graphics::DimensionBuffer<Bounds> bounds;

	//* constants *//

	static constexpr const uint32_t kMaxVertices  = 64;
	static constexpr const uint32_t kMaxTriangles = 124;

	static constexpr const float kConeWeight = 0.5f;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
