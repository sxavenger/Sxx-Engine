#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Tex namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Tex {
	//!< graphics pipeline <GRAPHICS_PIPELINE> での shader の場合は Sample(...) を使用し, compute pipeline <COMPUTE_PIPELINE> での shader の場合は SampleLevel(...) を使用する

	////////////////////////////////////////////////////////////////////////////////////////////
	// Channel enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Channel : uint32_t {
		R = 0,
		G = 1,
		B = 2,
		A = 3,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	Texture1D<T> Get1d(uint32_t index) {
		return ResourceDescriptorHeap[index];
	}

	template <typename T>
	T Sample1d(Texture1D<T> texture, SamplerState sampler, float32_t texcoord) {
#ifdef GRAPHICS_PIPELINE
		return texture.Sample(sampler, texcoord);
#else
		return texture.SampleLevel(sampler, texcoord, 0);
#endif
	}

	template <typename T>
	Texture2D<T> Get2d(uint32_t index) {
		return ResourceDescriptorHeap[index];
	}

	template <typename T>
	T Sample2d(Texture2D<T> texture, SamplerState sampler, float32_t2 texcoord) {
#ifdef GRAPHICS_PIPELINE
		return texture.Sample(sampler, texcoord);
#else
		return texture.SampleLevel(sampler, texcoord, 0);
#endif
	}

	template <typename T>
	Texture3D<T> Get3d(uint32_t index) {
		return ResourceDescriptorHeap[index];
	}

	template <typename T>
	T Sample3d(Texture3D<T> texture, SamplerState sampler, float32_t3 texcoord) {
#ifdef GRAPHICS_PIPELINE
		return texture.Sample(sampler, texcoord);
#else
		return texture.SampleLevel(sampler, texcoord, 0);
#endif
	}

}
