#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// [Sxavenger Engine / Sxx Engine] version
////////////////////////////////////////////////////////////////////////////////////////////

#define SXAVENGER_ENGINE_VERSION "4.0.0"

////////////////////////////////////////////////////////////////////////////////////////////
// [Sxavenger Engine / Sxx Engine] namespace
////////////////////////////////////////////////////////////////////////////////////////////

#define SXAVENGER_ENGINE_NAMESPACE_BEGIN \
	namespace Sxx {

#define SXAVENGER_ENGINE_NAMESPACE_BEGIN_(category) \
	namespace Sxx::category {

#define SXAVENGER_ENGINE_NAMESPACE_END \
	}

#define SXAVENGER_ENGINE_USING \
	using namespace Sxx;

#define SXAVENGER_ENGINE_USING_(category) \
	SXAVENGER_ENGINE_USING \
	using namespace Sxx::category;

#define SXAVENGER_ENGINE \
	Sxx::

#define SXAVENGER_ENGINE_(category) \
	Sxx::category::

//!< [Sxavenger Engine / Sxx Engine] namespace
namespace Sxx {}
