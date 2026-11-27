#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightCommon.hlsli"

//* system
#include "../../System/Tex.hlsli"

//* lib
#include "../../Lib/Flag.hlsli"
#include "../../Lib/Mathmatic.hlsli"
#include "../../Lib/Equirectangular.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Component namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Component {

	////////////////////////////////////////////////////////////////////////////////////////////
	// SkyLight structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SkyLight {

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			None            = 0,
			Equirectangular = 1,
			Cube            = 2,
		};

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t3 light_color;
		float32_t light_intensity; //!< [lux]

		uint32_t environment_texture_index;
		Type environment_texture_type;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float32_t3 GetEnvironment(float32_t3 direction, SamplerState sampler) {
			switch (environment_texture_type) {
				case Type::Equirectangular:
					{
						Texture2D<float32_t4> texture = Tex::Get2d<float32_t4>(environment_texture_index);
						float32_t2 texcoord = Equirect::DirectionToTexcoord(direction); //!< Equirectangular texture の場合は, direction を texcoord に変換する必要がある
						return Tex::SampleLevel2d<float32_t4>(texture, sampler, texcoord).rgb;
					}

				case Type::Cube:
					{
						TextureCube<float32_t4> texture = Tex::GetCube<float32_t4>(environment_texture_index);
						return Tex::SampleLevelCube<float32_t4>(texture, sampler, direction).rgb;
					}

				default:
					return float32_t3(1.0, 1.0, 1.0); //!< サンプリングするTextureがない場合は白色を返す
			}
		}

		float32_t3 GetColorMask() {
			return light_color * light_intensity;
		}

	};

}
