#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "../System/Tex.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Material namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Material {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Normal structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Normal {

		////////////////////////////////////////////////////////////////////////////////////////////
		// Operation enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Operation : uint32_t {
			Value,
			Texture,
		};

		//=========================================================================================
		// public variables
		//=========================================================================================

		Operation operation;
		uint32_t texture_index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float32_t3 Get(float3 normal, float3 tangent, float3 bitangent, SamplerState sampler, float32_t2 texcoord) {

			[branch]
			switch (operation) {
				case Operation::Value:
					return normal;

				case Operation::Texture:
				{
					float32_t3x3 tbn = float32_t3x3(tangent, bitangent, normal);

					Texture2D<float32_t4> texture = Tex::Get2d<float32_t4>(texture_index);
					float32_t3 sampled_color = Tex::Sample2d(texture, sampler, texcoord).rgb;

					return normalize(mul(sampled_color * 2.0 - 1.0, tbn));
				}
			}

			return 0.0; //!< operatorの未定義.
		}

	};

}
