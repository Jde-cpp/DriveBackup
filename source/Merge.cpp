#include "stdafx.h"
#include "Merge.h"
#include "../../Framework/source/io/DiskWatcher.h"

#define var const auto

namespace Jde::DriveBackup
{
	void Merge( tuple<fs::path,sp<IO::IDrive>> source, tuple<fs::path,sp<IO::IDrive>> destination )noexcept(false)
	{
		//auto& source2 = *std::get<1>( source );
		//var& sourceHierarchy = source2.Recursive( std::get<0>( source ) );
	}

	int RemoveOrphans( const fs::path& destPath, const fs::path& sourcePath, IO::IDrive& sourceModule, IO::IDrive& destinationModule, const map<string,IO::IDirEntryPtr>& sourceEntries, map<string,IO::IDirEntryPtr>& destinationEntries, vector<string>& statuses )noexcept(false)
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
				DBGN( "Removing:  '{}'", path.string() );
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
	IO::IDirEntryPtr Upload( const IO::IDirEntry& source, const fs::path& destination, IO::IDrive& sourceModule, IO::IDrive& destinationModule )
	{
		//var path = destPath/relativePath;
		DBG( "Uploading:  '{}'", destination.string() );
		return source.IsDirectory()
			? destinationModule.CreateFolder( destination, source )
			: destinationModule.Save( destination, *sourceModule.Load(source) , source );
	}
	void Replace( const IO::IDirEntry& source, const IO::IDirEntry& destination, IO::IDrive& sourceModule, IO::IDrive& destinationModule, string_view relativePath )noexcept(false)
	{
		DBG( "Replacing:  '{}'", relativePath );
		if( source.IsDirectory() )
			THROW( IOException("Replace not implemented on directory '{}'", source.Path.string()) );
		destinationModule.Trash( destination.Path );

		Upload( source, destination.Path, sourceModule, destinationModule );
	}
}