#include "BlendState.h"
SXAVENGER_ENGINE_USING_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// BlendState class static variables
////////////////////////////////////////////////////////////////////////////////////////////

const std::array<BlendState::BlendDesc, EnumUtil<BlendModeColor>::GetCount()> BlendState::kBlendDescColor = {
	BlendState::BlendDesc{ false },                                                                               //!< None
	BlendState::BlendDesc{ true, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD },          //!< Normal
	BlendState::BlendDesc{ true, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_ONE,           D3D12_BLEND_OP_ADD },          //!< Additive
	BlendState::BlendDesc{ true, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_COLOR, D3D12_BLEND_OP_REV_SUBTRACT }, //!< Subtractive
	BlendState::BlendDesc{ true, D3D12_BLEND_ZERO,       D3D12_BLEND_ONE,          D3D12_BLEND_OP_ADD },          //!< Multiply
	BlendState::BlendDesc{ true,  D3D12_BLEND_INV_DEST_COLOR, D3D12_BLEND_ONE,     D3D12_BLEND_OP_ADD },          //!< Screen
};

const std::array<BlendState::BlendDesc, EnumUtil<BlendModeTransparent>::GetCount()> BlendState::kBlendDescTransparent = {
	BlendState::BlendDesc{ false },                                                      //!< None
	BlendState::BlendDesc{ true, D3D12_BLEND_ONE, D3D12_BLEND_ONE, D3D12_BLEND_OP_ADD }, //!< Additive
	BlendState::BlendDesc{ true, D3D12_BLEND_ONE, D3D12_BLEND_ONE, D3D12_BLEND_OP_MAX }, //!< Max
};

////////////////////////////////////////////////////////////////////////////////////////////
// BlendState class methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_RENDER_TARGET_BLEND_DESC BlendState::CreateBlendDesc(BlendModeColor color, BlendModeTransparent transparent, D3D12_COLOR_WRITE_ENABLE writeMask) {

	const BlendDesc& colorDesc       = kBlendDescColor[EnumUtil<BlendModeColor>::Cast(color)];
	const BlendDesc& transparentDesc = kBlendDescTransparent[EnumUtil<BlendModeTransparent>::Cast(transparent)];

	D3D12_RENDER_TARGET_BLEND_DESC desc = {};
	desc.BlendEnable = colorDesc.enable || transparentDesc.enable; //!< カラーと透明度のいずれかが有効であれば有効とする.

	//* color側のblendの設定
	desc.SrcBlend  = colorDesc.srcBlend;
	desc.DestBlend = colorDesc.destBlend;
	desc.BlendOp   = colorDesc.blendOp;

	//* alpha側のblendの設定
	desc.SrcBlendAlpha  = transparentDesc.srcBlend;
	desc.DestBlendAlpha = transparentDesc.destBlend;
	desc.BlendOpAlpha   = transparentDesc.blendOp;

	desc.RenderTargetWriteMask = static_cast<UINT8>(writeMask);
	return desc;
}
