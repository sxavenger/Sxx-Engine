#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// GBuffer namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace GBuffer {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Surface structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Surface {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t depth;
		float32_t3 position;
		float32_t3 normal;
		float32_t3 albedo;
		float32_t metallic;
		float32_t roughness;
		float32_t ambient_occlusion;

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool HasGeometry() {
			return depth < 1.0; //!< depthが1.0の場合は描画されていないのでfalseを返す
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Textures structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Textures { //!< [Rendering::GBuffer 参照]

		//=========================================================================================
		// public variables
		//=========================================================================================

		Texture2D<float32_t> depth;
		Texture2D<float32_t4> bufferA;
		Texture2D<float32_t4> bufferB;
		Texture2D<float32_t4> bufferC;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* depth methods *//

		float32_t LoadDepth(uint32_t2 pixel) {
			return depth[pixel];
		}

		float32_t SampleDepth(float32_t2 texcoord, SamplerState point_sampler) {
			return depth.SampleLevel(point_sampler, texcoord, 0);
		}

		//* albedo methods *//

		float32_t3 LoadAlbedo(uint32_t2 pixel) {
			return bufferA[pixel].rgb;
		}

		float32_t3 SampleAlbedo(float32_t2 texcoord, SamplerState point_sampler) {
			return bufferA.SampleLevel(point_sampler, texcoord, 0).rgb;
		}

		//* normal methods *//

		static float32_t3 DecodeNormal(float32_t3 n) {
			return normalize(n * 2.0 - 1.0);
		}

		float32_t3 LoadNormal(uint32_t2 pixel) {
			return DecodeNormal(bufferB[pixel].rgb);
		}

		float32_t3 SampleNormal(float32_t2 texcoord, SamplerState point_sampler) {
			return DecodeNormal(bufferB.SampleLevel(point_sampler, texcoord, 0).rgb);
		}

		//* material_arm methods *//

		float32_t3 LoadMaterialARM(uint32_t2 pixel) {
			return bufferC[pixel].rgb;
		}

		float32_t3 SampleMaterialARM(float32_t2 texcoord, SamplerState point_sampler) {
			return bufferC.SampleLevel(point_sampler, texcoord, 0).rgb;
		}

		//* position methods *//

		float32_t3 ReconstructWorldPosition(uint32_t2 pixel, uint32_t2 resolution, float32_t4x4 proj_inv, float32_t4x4 view_inv) {
			float32_t depth = LoadDepth(pixel);

			float32_t2 ndc = (float32_t2(pixel) + 0.5) / float32_t2(resolution) * 2.0 - 1.0; //!< [-1, 1]
			ndc.y = -ndc.y; //!< flip y

			float32_t4 clip_space = float32_t4(ndc, depth, 1.0);
			float32_t4 position = mul(clip_space, mul(proj_inv, view_inv));
			return position.xyz / position.w;
		}

		//* static methods *//

		static Textures Create(Texture2D<float32_t> depth, Texture2D<float32_t4> bufferA, Texture2D<float32_t4> bufferB, Texture2D<float32_t4> bufferC) {
			Textures textures;
			textures.depth        = depth;
			textures.bufferA      = bufferA;
			textures.bufferB      = bufferB;
			textures.bufferC      = bufferC;

			return textures;
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reconstructor structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Reconstructor {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t2 pixel;
		uint32_t2 resolution;

		float32_t4x4 projection_inverse;
		float32_t4x4 view_inverse;

		//=========================================================================================
		// public methods
		//=========================================================================================

		Surface RecnstructSurface(Textures textures) {
			Surface surface;

			//!< Depthの取得
			surface.depth = textures.LoadDepth(pixel);
			if (surface.depth == 1.0f) {
				return surface; //!< Depthが1.0fの場合は描画されていないのでdepthのみを返す
			}

			//!< Positionの取得
			surface.position = textures.ReconstructWorldPosition(pixel, resolution, projection_inverse, view_inverse);

			//!< Normalの取得
			surface.normal = textures.LoadNormal(pixel);

			//!< Albedoの取得
			surface.albedo = textures.LoadAlbedo(pixel); // TODO: ACEScgに変換する

			//!< Materialの取得
			float32_t3 material_arm = textures.LoadMaterialARM(pixel);
			surface.ambient_occlusion = material_arm.r;
			surface.roughness         = material_arm.g;
			surface.metallic          = material_arm.b;

			return surface;
		}

		//* static methods *//

		static Reconstructor Create(uint32_t2 pixel, uint32_t2 resolution, float32_t4x4 projection_inverse, float32_t4x4 view_inverse) {
			Reconstructor reconstructor;
			reconstructor.pixel              = pixel;
			reconstructor.resolution         = resolution;
			reconstructor.projection_inverse = projection_inverse;
			reconstructor.view_inverse       = view_inverse;

			return reconstructor;
		}

	};

}
