#pragma once
#include "Dll.h"

namespace Jde::IO
{
	struct IDrive;

	sp<IDrive> LoadDriveModule( const fs::path& libraryName );

}