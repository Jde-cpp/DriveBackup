// #ifndef _MSC_VER
// 	#include "../../Framework/source/application/ApplicationLinux.h"
// #endif
#include <boost/regex.hpp>
#include "Scan.h"
#include "Drive.h"
#include "Merge.h"
#include "types/Entry.h"
#include "types/TrashEntry.h"

//#include "io/drive/google/GoogleDrive.h"

#define var const auto

namespace Jde::DriveBackup
{
	std::shared_ptr<Jde::Settings::Container> SettingsPtr;
	int Run()noexcept(false);
}

int main( int argc, char** argv )
{
	int result = EXIT_FAILURE;
	try
	{
		Jde::OSApp::Startup( argc, argv, "DriveBackup" );
		// std::filesystem::path settingsPath = std::filesystem::path( "DriveBackup.json" );
		// Jde::Settings::SetGlobal( std::make_shared<Jde::Settings::Container>(settingsPath) );
		// Jde::InitializeLogger( "DriveBackup" );
		Jde::DriveBackup::SettingsPtr = Jde::Settings::Global().SubContainer( "DriveBackup" );
		std::thread{ []{Jde::DriveBackup::Run();} }.detach();
		Jde::IApplication::Pause();
	}
	catch( const Jde::EnvironmentException& e )
	{
		CRITICAL0( std::string(e.what()) );
	}
	return result;
}

namespace Jde::DriveBackup
{
// {"destination":    {"driver":"/home/duffyj/code/jde/7.0/native/bin/debug/libDriveGoogle.so","name":"Google","path":"dbs/market/securities","removeOrphans":"true"},
// "source":          {"driver":"/home/duffyj/code/jde/7.0/native/bin/debug/libDriveLinux.so","name":"Linux","path":"/mnt/2TB/securities2"}}
	int Run()noexcept(false)
	{
		var trashEntries = Jde::DriveBackup::SettingsPtr->Array<TrashEntry>( "TrashDisposal" );
		for( var& entry : trashEntries )
		{
			auto pDriver = IO::LoadDriveModule( entry.Driver );
			var latestDate = Clock::now()-entry.Expiration;
			DBG( "{} - removing trash upto '{}'"sv, entry.Name, ToIsoString(latestDate) );
			pDriver->TrashDisposal( latestDate );
		}

		var entries = Jde::DriveBackup::SettingsPtr->Array<Entry>( "Entries" );
		vector<string> statuses( entries.size()+1 );
		for( uint i=0; i<entries.size(); ++i )
			statuses[i] = fmt::format( "{}->{}", entries[i].Source.Name, entries[i].Destination.Name );

		for( var& entry : entries )
		{
			try
			{
				//double check TSLA/options uploads.
				//for google get earliest history.
				//for drive start watcher.
				//
				DBG( "({}[{}])=>({}[{}])"sv, entry.Source.Path.string(), entry.Source.Driver.string(), entry.Destination.Path.string(), entry.Destination.Driver.string() );
				auto pSource = IO::LoadDriveModule( entry.Source.Driver );
				auto pDestModule = IO::LoadDriveModule( entry.Destination.Driver );

				//Upload( *pSource->Get(fs::path{"/mnt/2TB/securities/arca/IAI/2019-12-10.dat.xz"}), fs::path{"dbs/market/securities/arca/IAI/2019-12-10.dat.xz"}, *pSource, *pDestModule );
//		ostringstream osDebug;""
//		osDebug << nlohmann::json{result}; ""

				*statuses.rbegin() = fmt::format( "Loading {}", entry.Source.Name );
				Logging::SetStatus( statuses );
				auto sourceEntries = pSource->Recursive( entry.Source.Path );
				*statuses.rbegin() = fmt::format( "Loading {}", entry.Destination.Name );
				Logging::SetStatus( statuses );
				auto destinationEntries = pDestModule->Recursive( entry.Destination.Path );
				auto format = fmt::format( "{}->{} {{}}/{}", entry.Source.Name, entry.Destination.Name, sourceEntries.size() );

				auto sync = [&entry, &format,&statuses]( const fs::path& destPath, const fs::path& /*sourcePath*/, IO::IDrive& destinationModule, IO::IDrive& sourceModule, const map<string,IO::IDirEntryPtr>& sourceEntries, map<string,IO::IDirEntryPtr>& destinationEntries )
				{
					uint i=0; var startCount = destinationEntries.size();
					for( var& [relativePath,pSource] : sourceEntries )
					{
						var stemString = fs::path( relativePath ).stem().string();
						if( std::find_if(entry.Ignore.begin(), entry.Ignore.end(), [&stemString](var& expression){return boost::regex_search(stemString.c_str(), boost::regex{expression});} )!=entry.Ignore.end() )
							continue;
						*statuses.rbegin() = fmt::format( format, ++i );
						Logging::SetStatus( statuses );
						var pDestination = destinationEntries.find( relativePath );
						if( pDestination==destinationEntries.end() )
						{
							var path = destPath/relativePath;
							try
							{
								//DBG( "Upload '{}'", relativePath );
								destinationEntries.emplace( relativePath, Upload(*pSource, path, sourceModule, destinationModule) );
								// IO::IDirEntryPtr pNewItem = pSource->IsDirectory()
								// 	? destinationModule.CreateFolder( path, *pSource )
								// 	: destinationModule.Save( path, *sourceModule.Load(*pSource) , *pSource );
								// destinationEntries.emplace( relativePath, pNewItem );
							}
							catch( const Exception& )
							{}
							catch( const fs::filesystem_error& e )
							{
								DBG( "Could not upload '{}' - '{}'."sv, path.string(), e.what() );
							}
						}
						else if( !pSource->IsDirectory() && pDestination->second->Size!=pSource->Size )
						{
							DBG( "Replace '{}'"sv, relativePath );
							try
							{
								Replace( *pSource, *pDestination->second, sourceModule, destinationModule, relativePath );
							}
							catch( const Exception& )
							{}
						}
					}
					return destinationEntries.size()-startCount;
				};
				while( sync(entry.Destination.Path, entry.Source.Path, *pDestModule, *pSource, sourceEntries, destinationEntries) );
				format = fmt::format( "{}->{} {{}}/{}", entry.Source.Name, entry.Destination.Name, destinationEntries.size() );
				if( entry.Destination.RemoveOrphans )
					while( RemoveOrphans(entry.Source.Path, entry.Destination.Path, *pSource, *pDestModule, sourceEntries, destinationEntries, statuses) );
				else
					while( sync(entry.Source.Path, entry.Destination.Path, *pSource, *pDestModule, destinationEntries, sourceEntries) );
				INFO0( "Complete!"sv );
			}
			catch( const Exception& e )
			{
				ERR0( string(e.what()) );
				//Cleanup?
			}
				//Linux
				//Fix tsla vs. TSLA.
				//Watch for changes
				//deal with trash.
				//for each source entry, write to destination.
				//for each destination entry, write to source.
		}
			//var source = entry["source"];
			//var destination = entry["destination"];
		//if no data, create data.
		//merge.
		//auto pScan = std::make_shared<Jde::IO::Scanner>();
		//pScan->Run();
		DBG0( "Run() complete"sv );
		return EXIT_SUCCESS;
	}
}

//
//int main()
//{
//	InitializeLogger( Jde::LogLevel::Debug, "/tmp/DriveBackup.txt" );

//	return Jde::Application::Main( [&pScan](){pScan->Run();} );
//}