#pragma once
namespace Jde::IO{ struct IDrive; struct IDirEntry; }
namespace Jde::DriveBackup
{
	void Merge( tuple<fs::path,sp<IO::IDrive>> source, tuple<fs::path,sp<IO::IDrive>> destination )noexcept(false);
	int RemoveOrphans( const fs::path& destPath, const fs::path& sourcePath, IO::IDrive& destinationModule, IO::IDrive& souceModule, const map<string,IO::IDirEntryPtr>& sourceEntries, map<string,IO::IDirEntryPtr>& destinationEntries, vector<string>& statuses )noexcept(false);
	void Replace( const IO::IDirEntry& source, const IO::IDirEntry& destination, IO::IDrive& souceModule, IO::IDrive& destinationModule, string_view relativePath )noexcept(false);
	IO::IDirEntryPtr Upload( const IO::IDirEntry& source, const fs::path& destination, IO::IDrive& souceModule, IO::IDrive& destinationModule )noexcept(false);
}