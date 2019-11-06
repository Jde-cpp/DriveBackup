#pragma once
#include "io/DiskWatcher.h"

namespace Jde::IO
{
	struct Scanner : public IDriveChange, public std::enable_shared_from_this<IDriveChange>
	{
		void Run();
	};
}