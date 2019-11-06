#include "stdafx.h"
#include "Application.h"
#include "Scan.h"
#include "Drive.h"
#include "Merge.h"

//#include "io/drive/google/GoogleDrive.h"

#define var const auto

namespace Jde::DriveBackup
{
	std::shared_ptr<Jde::Settings::Container> SettingsPtr;
	int Run()noexcept(false);
}

int main()
{
	int result = EXIT_FAILURE;
	try
	{
		std::filesystem::path settingsPath = std::filesystem::path( "DriveBackup.json" );
		Jde::Settings::SetGlobal( std::make_shared<Jde::Settings::Container>(settingsPath) );
		Jde::InitializeLogger( "DriveBackup" );
		Jde::DriveBackup::SettingsPtr = Jde::Settings::Global().SubContainer( "DriveBackup" );
		std::thread{ []{Jde::DriveBackup::Run();} }.detach();
		Jde::Application::Pause();
	}
	catch( const Jde::EnvironmentException& e )
	{
		CRITICAL0( e.what() );
	}
	return result;
}

namespace Jde::DriveBackup
{
	struct EntryItem
	{
		fs::path Path;
		fs::path Driver;
		string Name;
		bool RemoveOrphans;
	};
	inline void to_json( nlohmann::json& j, const EntryItem& object ){ j = { {"name", object.Name}, {"path", object.Path}, {"driver", object.Driver}, {"removeOrphans", object.RemoveOrphans} }; }
	inline void from_json( const nlohmann::json& j, EntryItem& object )
	{
		string value;
		ostringstream os;
		os << j;
		DBG0( os.str() );
		if( j.find("path")!=j.end() )
		{ 
			j.at("path").get_to( value ); 
			object.Path = value; 
		}
		if( j.find("driver")!=j.end() )
		{ 
			j.at("driver").get_to( value ); 
			object.Driver = value;
		}
		if( j.find("name")!=j.end() )
		{ 
			j.at("name").get_to( value ); 
			object.Name = value;
		}
		if( j.find("removeOrphans")!=j.end() )
		{
			bool boolean=false;
			try{ j.at("removeOrphans").get_to( boolean ); }catch( nlohmann::detail::type_error& e ){ ERR("Could not parse removeOrphans:  {}", e.what());}
			object.RemoveOrphans = boolean;
		}
	}
// {"destination":    {"driver":"/home/duffyj/code/jde/7.0/native/bin/debug/libDriveGoogle.so","name":"Google","path":"dbs/market/securities","removeOrphans":"true"},
// "source":          {"driver":"/home/duffyj/code/jde/7.0/native/bin/debug/libDriveLinux.so","name":"Linux","path":"/mnt/2TB/securities2"}}
	struct Entry
	{
		EntryItem Source;
		EntryItem Destination;
	};
	inline void to_json( nlohmann::json& j, const Entry& object ){ j = { {"source", object.Source}, {"destination", object.Destination} }; }
	inline void from_json( const nlohmann::json& j, Entry& object )
	{
		string value;
		ostringstream os;
		os << j;
		DBG0( os.str() );
		{
			var item = j;
			if( item.find("source")!=item.end() )
				item.at("source").get_to( object.Source ); 
			if( item.find("destination")!=item.end() )
				item.at("destination").get_to( object.Destination ); 
		}
	}
	int Run()noexcept(false)
	{
		var entries = Jde::DriveBackup::SettingsPtr->Array<Entry>( "Entries" );
		vector<string> statuses( entries.size()+1 );
		for( uint i=0; i<entries.size(); ++i )
			statuses[i] = fmt::format( "{}->{}", entries[i].Source.Name, entries[i].Destination.Name );

		//IO::IDrive* pDrive = LoadDrive();
		//var contents = pDrive->Recursive( "/tmp" );
		//TODO:  trash needs to work, double check TSLA/options uploads.
		for( var entry : entries )
		{
			try
			{
				DBG( "({}[{}])=>({}[{}])", entry.Source.Path.string(), entry.Source.Driver.string(), entry.Destination.Path.string(), entry.Destination.Driver.string() );
				auto pSource = IO::LoadDriveModule( entry.Source.Driver );
				auto pDestModule = IO::LoadDriveModule( entry.Destination.Driver );

				*statuses.rbegin() = fmt::format( "Loading {}", entry.Source.Name );
				Logging::SetStatus( statuses );
				auto sourceEntries = pSource->Recursive( entry.Source.Path );

				*statuses.rbegin() = fmt::format( "Loading {}", entry.Destination.Name );
				Logging::SetStatus( statuses );
				auto destinationEntries = pDestModule->Recursive( entry.Destination.Path );
				string format = fmt::format( "{}->{} {{}}/{}", entry.Source.Name, entry.Destination.Name, sourceEntries.size() );

				auto sync = [&entry, &format,&statuses]( const fs::path& destPath, const fs::path& /*sourcePath*/, IO::IDrive& destinationModule, IO::IDrive& souceModule, const map<string,IO::IDirEntryPtr>& sourceEntries, map<string,IO::IDirEntryPtr>& destinationEntries )
				{
					uint i=0; var startCount = destinationEntries.size();
					for( var& [relativePath,pDirEntry] : sourceEntries )
					{
						*statuses.rbegin() = fmt::format( format, ++i );
						Logging::SetStatus( statuses );
						var pDestination = destinationEntries.find( relativePath );
						if( pDestination==destinationEntries.end() )
						{
							var path = destPath/relativePath;
							DBG( "Uploading:  '{}'", path.string() );
							try
							{
								IO::IDirEntryPtr pNewItem = pDirEntry->IsDirectory()
									? destinationModule.CreateFolder( path, *pDirEntry )
									: destinationModule.Save( path, *souceModule.Load(*pDirEntry) , *pDirEntry );
								destinationEntries.emplace( relativePath, pNewItem );
							}
							catch( const Exception& )
							{}
							catch( const fs::filesystem_error& e )
							{
								DBG( "Could not upload '{}' - '{}'.", path.string(), e.what() );
							}
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
			}
			catch( const Exception& e )
			{
				ERR0( e.what() );
				//Cleanup?
			}
				//Linux
				//Fix tsla vs. TSLA.
				//Watch for changes
				//see if there are differences.
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
		return EXIT_SUCCESS;
	}
}

//
//int main()
//{
//	InitializeLogger( Jde::LogLevel::Debug, "/tmp/DriveBackup.txt" );

//	return Jde::Application::Main( [&pScan](){pScan->Run();} );
//}