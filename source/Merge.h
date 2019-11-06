#pragma once
namespace Jde::IO{ struct IDrive; }
namespace Jde::DriveBackup
{
	void Merge( tuple<fs::path,sp<IO::IDrive>> source, tuple<fs::path,sp<IO::IDrive>> destination )noexcept(false);
	int RemoveOrphans( const fs::path& destPath, const fs::path& sourcePath, IO::IDrive& destinationModule, IO::IDrive& souceModule, const map<string,IO::IDirEntryPtr>& sourceEntries, map<string,IO::IDirEntryPtr>& destinationEntries, vector<string>& statuses )noexcept(false);
}