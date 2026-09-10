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
#include <Lib/Math/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTargetTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetTexture final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Options structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Options {
	public:

		//=========================================================================================
		// private methods
		//=========================================================================================

		std::optional<D3D12_CLEAR_VALUE> GetClearValue() const;

		//=========================================================================================
		// private variables
		//=========================================================================================

		DXGI_FORMAT format   = DXGI_FORMAT_UNKNOWN;
		Vector2ui resolution = {};

		Color4f clearColor = {};

		// TODO: Descriptorのカテゴリを指定するオプションを追加.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* transition option *//

	//! @brief リソースの状態を遷移する.
	void Transition(const Graphics::GraphicsCommandContext& context, D3D12_RESOURCE_STATES state);

	//* clear option *//

	//! @brief RenderTargetをクリアする.
	void ClearRenderTarget(const Graphics::GraphicsCommandContext& context);

	//* texture option *//

	void SetName(const std::string_view& name) const;
	void SetName(const std::wstring_view& name) const;

	Graphics::Resource& GetResource();
	const Graphics::Resource& GetResource() const;

	const Graphics::Descriptor::Handle& GetDescriptorRTV() const;

	const Graphics::Descriptor::Handle& GetDescriptorSRV() const;

	const Graphics::Descriptor::Handle& GetDescriptorUAV() const;

	//* options *//

	//! @brief 解像度を取得する.
	const Vector2ui& GetResolution() const { return options_.resolution; }

	//* static methods *//

	static RenderTargetTexture Create(const Options& options);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* options *//

	Options options_;

	//* graphics resource *//

	Graphics::ResourceHandle resource_;
	Graphics::Descriptor descriptorRTV_;
	Graphics::Descriptor descriptorSRV_;
	Graphics::Descriptor descriptorUAV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* static helper methods *//

	static Graphics::ResourceHandle CreateResource(const Options& options);

	static void CreateDescriptorRTV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource);

	static void CreateDescriptorSRV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource);

	static void CreateDescriptorUAV(Graphics::Descriptor& descriptor, const Options& options, const Graphics::Resource& resource);

};

SXAVENGER_ENGINE_NAMESPACE_END
