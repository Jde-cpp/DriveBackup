#include "Drive.h"
#include "../../Framework/source/io/drive/DriveApi.h"
#include "../../Framework/source/io/DiskWatcher.h"

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
	sp<IDrive> LoadDriveModuleInternal( const fs::path& path )noexcept(false)
	{
		if( !fs::exists(path) )
			THROW( IOException("module '{}' does not exist.", path.string()) );

		lock_guard l{ _driveMutex };
		auto pApi = _driveApis.emplace( path.string(), shared_ptr<IDriveApi>{new IDriveApi(path)} ).first->second;

		return _drives.emplace( path.string(), sp<Jde::IO::IDrive>{pApi->LoadDriveFunction()} ).first->second;
	}
	sp<IDrive> LoadDriveModule( const fs::path& path )noexcept(false)
	{
		return path.empty() ? LoadNativeDrive() : LoadDriveModuleInternal( path );
	}
}