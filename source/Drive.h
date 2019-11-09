#pragma once
#include "../../Framework/source/Dll.h"

namespace Jde::IO
{
	struct IDrive;

	sp<IDrive> LoadDriveModule( const fs::path& libraryName );

}