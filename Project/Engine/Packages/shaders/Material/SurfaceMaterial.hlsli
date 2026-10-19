#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Albedo.hlsli"
#include "Transparency.hlsli"
#include "Emissive.hlsli"
#include "Normal.hlsli"
#include "Roughness.hlsli"
#include "Metallic.hlsli"
#include "Emissive.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterial structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SurfaceMaterial {

	//=========================================================================================
	// public variables
	//=========================================================================================

	Material::Albedo albedo;
	Material::Transparency transparency;
	Material::Emissive emissive;
	Material::Normal normal;
	Material::Roughness roughness;
	Material::Metallic metallic;

};
