#include "stdafx.h"
#include "Drive.h"
#include "io/drive/DriveApi.h"
#include "io/DiskWatcher.h"

namespace Jde::IO
{
	class IDriveApi
	{
		DllHelper _dll;
	public:
		IDriveApi( const fs::path& path ):
			_dll{ path },
			LoadDriveFunction{ _dll["LoadDrive"] }
		{}
		decltype(LoadDrive) *LoadDriveFunction;
	};

	map<string,sp<Jde::IO::IDriveApi>> _driveApis; mutex _driveMutex;
	map<string,sp<Jde::IO::IDrive>> _drives; 

	sp<IDrive> LoadDriveModule( const fs::path& path )
	{
		if( !fs::exists(path) )
			THROW( IOException("module '{}' does not exist.", path.string()) );

		lock_guard l{ _driveMutex };
		auto pApi = _driveApis.emplace( path.string(), shared_ptr<IDriveApi>{new IDriveApi(path)} ).first->second;

		return _drives.emplace( path.string(), sp<Jde::IO::IDrive>{pApi->LoadDriveFunction()} ).first->second;
	}
}