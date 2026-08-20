#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Flag/Flag.h>

//* assimp
#include <assimp/postprocess.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpOption enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class AssimpOption : uint32_t {
	CalcTangentSpace         = aiProcess_CalcTangentSpace,          //!< タンジェント空間の計算を行う
	JoinIdenticalVertices    = aiProcess_JoinIdenticalVertices,     //!< 同一の頂点データを統合する
	MakeLeftHanded           = aiProcess_MakeLeftHanded,            //!< 左手座標系へ変換する
	Triangulate              = aiProcess_Triangulate,               //!< 全ての面を三角形へ変換する
	RemoveComponent          = aiProcess_RemoveComponent,           //!< 指定された不要な頂点・マテリアル等の要素を削除する
	GenNormals               = aiProcess_GenNormals,                //!< 法線が存在しない場合に法線を生成する
	GenSmoothNormals         = aiProcess_GenSmoothNormals,          //!< スムース法線を生成する
	SplitLargeMeshes         = aiProcess_SplitLargeMeshes,          //!< 大きなメッシュを分割する
	PreTransformVertices     = aiProcess_PreTransformVertices,      //!< ノード変換を頂点へ適用して階層を除去する
	LimitBoneWeights         = aiProcess_LimitBoneWeights,          //!< 1頂点あたりのボーンウェイト数を制限する
	ValidateDataStructure    = aiProcess_ValidateDataStructure,     //!< シーンデータ構造を検証する
	ImproveCacheLocality     = aiProcess_ImproveCacheLocality,      //!< GPUの頂点キャッシュ効率を改善する
	RemoveRedundantMaterials = aiProcess_RemoveRedundantMaterials,  //!< 重複するマテリアルを削除する
	FixInfacingNormals       = aiProcess_FixInfacingNormals,        //!< 内向きになっている法線を修正する
	SortByPrimitiveType      = aiProcess_SortByPType,               //!< プリミティブタイプごとにメッシュを分類する
	FindDegenerates          = aiProcess_FindDegenerates,           //!< 退化したプリミティブを検出する
	FindInvalidData          = aiProcess_FindInvalidData,           //!< 不正なデータを検出・修正する
	GenUVCoords              = aiProcess_GenUVCoords,               //!< UV座標を生成する
	TransformUVCoords        = aiProcess_TransformUVCoords,         //!< テクスチャのUV変換を頂点UVへ適用する
	FindInstances            = aiProcess_FindInstances,             //!< 重複したメッシュを検出してインスタンス参照へ置換する
	OptimizeMeshes           = aiProcess_OptimizeMeshes,            //!< メッシュ数を削減して描画効率を改善する
	OptimizeGraph            = aiProcess_OptimizeGraph,             //!< シーン階層を最適化する
	FlipUVs                  = aiProcess_FlipUVs,                   //!< UV座標のY軸方向を反転する
	FlipWindingOrder         = aiProcess_FlipWindingOrder,          //!< 面の頂点順序を反転する
	SplitByBoneCount         = aiProcess_SplitByBoneCount,          //!< ボーン数に応じてメッシュを分割する
	Debone                   = aiProcess_Debone,                    //!< 不要なボーンを除去する
	GlobalScale              = aiProcess_GlobalScale,               //!< モデル全体へスケールを適用する
	EmbedTextures            = aiProcess_EmbedTextures,             //!< 外部テクスチャをシーンへ埋め込む
	ForceGenNormals          = aiProcess_ForceGenNormals,           //!< 既存の法線を無視して法線を強制生成する
	DropNormals              = aiProcess_DropNormals,               //!< 全ての法線を削除する
	// GenBoundingBoxes         = aiProcess_GenBoundingBoxes,          //!< メッシュのバウンディングボックスを生成する
	
	ConvertToLeftHanded //!< 左手座標系へ変換する (aiProcess_ConvertToLeftHanded参照)
		= MakeLeftHanded
		| FlipUVs
		| FlipWindingOrder, 

	RealTimeFast //!< リアルタイムレンダリング向けの高速設定 (aiProcessPreset_TargetRealtime_Fast参照)
		= CalcTangentSpace
		| JoinIdenticalVertices
		| Triangulate
		| SortByPrimitiveType,

	RealTimeQuality //!< リアルタイムレンダリング向けの高品質設定 (aiProcessPreset_TargetRealtime_Quality参照)
		= CalcTangentSpace
		| JoinIdenticalVertices
		| ImproveCacheLocality
		| LimitBoneWeights
		| RemoveRedundantMaterials
		| SplitLargeMeshes
		| Triangulate
		| SortByPrimitiveType
		| FindDegenerates
		| FindInvalidData,

};
ENUM_FLAG_OPERATORS(AssimpOption);

SXAVENGER_ENGINE_NAMESPACE_END
