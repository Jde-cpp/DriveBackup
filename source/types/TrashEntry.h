namespace Jde::DriveBackup
{
	struct TrashEntry
	{
		fs::path Driver;
		string Name;
		Duration Expiration;
	};
	inline void to_json( nlohmann::json& j, const TrashEntry& object )noexcept{ j = { {"name", object.Name}, {"driver", object.Driver} }; }//needs expiration
	inline void from_json( const nlohmann::json& j, TrashEntry& object )noexcept(false)
	{
		string value;
		ostringstream os;
		os << j;
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
		if( j.find("expiration")!=j.end() )
		{
			j.at("expiration").get_to( value );
			object.Expiration = Chrono::ToDuration( value );
		}
	}
}