#pragma once

#include <memory>
#include <functional>

#include "class/registry/registry.hpp"
#include "class/process/process.hpp"
#include "class/credentials/credentials.hpp"
#include "class/network/network.hpp"
#include "class/libraries/libraries.hpp"

#include "data/data.hpp"
#include "../utils/utils.hpp"

namespace ctx
{
	/* data */
	inline const std::unique_ptr<data> data = std::make_unique<class data>();

	/* functions */
	inline const std::unique_ptr<registry> registry = std::make_unique<class registry>();
	inline const std::unique_ptr<process> process = std::make_unique<class process>();
	inline const std::unique_ptr<credentials> credentials = std::make_unique<class credentials>();
	inline const std::unique_ptr<network> network = std::make_unique<class network>();
	inline const std::unique_ptr<libraries> libraries = std::make_unique<class libraries>();
}
