namespace Jde::DriveBackup
{
	struct EntryItem
	{
		fs::path Path;
		fs::path Driver;
		string Name;
		bool RemoveOrphans;
	};
#define var const auto
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
			try{ j.at("removeOrphans").get_to( boolean ); }catch( nlohmann::detail::type_error& e ){ ERR("Could not parse removeOrphans:  {}"sv, e.what());}
			object.RemoveOrphans = boolean;
		}
	}

	struct Entry
	{
		EntryItem Source;
		EntryItem Destination;
		vector<string> Ignore;
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
			if( item.find("ignore")!=item.end() )
				item.at("ignore").get_to( object.Ignore );
		}
	}
#undef var
}