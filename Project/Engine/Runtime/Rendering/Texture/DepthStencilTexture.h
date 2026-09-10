#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Core/Descriptor.h>
#include <Runtime/Graphics/Core/GraphicsCommandContext.h>
#include <Runtime/Graphics/Buffer/Resource.h>
#include <Runtime/Graphics/Buffer/ResourceHandle.h>

//* lib
#include <Lib/Math/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilTexture final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Options structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Options {
	public:

		//=========================================================================================
		// private methods
		//=========================================================================================

		D3D12_CLEAR_VALUE GetClearValue() const;

		//=========================================================================================
		// private variables
		//=========================================================================================

		DXGI_FORMAT format   = DXGI_FORMAT_UNKNOWN;
		Vector2ui resolution = {};

		float depthClearValue     = 1.0f;
		uint8_t stencilClearValue = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* transition option *//

	//! @brief リソースの状態を遷移する.
	void Transition(const Graphics::GraphicsCommandContext& context, D3D12_RESOURCE_STATES state);

	//* clear option *//

	//! @brief DepthStencilをクリアする.
	void ClearDepthStencil(const Graphics::GraphicsCommandContext& context);

	//* texture option *//

	void SetName(const std::string_view& name) const;
	void SetName(const std::wstring_view& name) const;

	Graphics::Resource& GetResource();
	const Graphics::Resource& GetResource() const;

	const Graphics::Descriptor::Handle& GetDescriptorDSV() const;

	const Graphics::Descriptor::Handle& GetDescriptorSRV() const;

	//* static methods *//

	static DepthStencilTexture Create(const Options& options);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* options *//

	Options options_;

	//* graphics resource *//

	Graphics::ResourceHandle resource_;
	Graphics::Descriptor descriptorDSV_;
	Graphics::Descriptor descriptorSRV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* static helper methods *//

	static Graphics::ResourceHandle CreateResource(const Options& options);

	static void CreateDescriptorDSV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource);

	static void CreateDescriptorSRV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource);

};

SXAVENGER_ENGINE_NAMESPACE_END
