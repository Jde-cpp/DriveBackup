#include "stdafx.h"
#include "Merge.h"
#include "io/DiskWatcher.h"

#define var const auto

namespace Jde::DriveBackup
{
	void Merge( tuple<fs::path,sp<IO::IDrive>> source, tuple<fs::path,sp<IO::IDrive>> destination )noexcept(false)
	{
		//auto& source2 = *std::get<1>( source );
		//var& sourceHierarchy = source2.Recursive( std::get<0>( source ) );
	}

	int RemoveOrphans( const fs::path& destPath, const fs::path& sourcePath, IO::IDrive& souceModule, IO::IDrive& destinationModule, const map<string,IO::IDirEntryPtr>& sourceEntries, map<string,IO::IDirEntryPtr>& destinationEntries, vector<string>& statuses )noexcept(false)
	{
		var startCount = destinationEntries.size();
		var format = fmt::format( "Verifying Destination {{}}/{}", startCount );
		int i=0;
		for( var& [relativePath,pDirEntry] : destinationEntries )
		{
			*statuses.rbegin() = fmt::format( format, ++i );
			Logging::SetStatus( statuses );
			var pSource = sourceEntries.find( relativePath );
			if( pSource==sourceEntries.end() )
			{
				var path = sourcePath/relativePath;
				DBG( "Removing:  '{}'", path.string() );
				try
				{
					destinationModule.Trash( path );
					destinationEntries.erase( relativePath );
				}
				catch( const Exception& e )
				{}
				catch( const fs::filesystem_error& e )
				{
					DBG( "Could not erase '{}' - '{}'.", path.string(), e.what() );
				}
			}
		}
		return destinationEntries.size()-startCount;
	}
}