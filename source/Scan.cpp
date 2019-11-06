#include "stdafx.h"
#include "Scan.h"
#include "io/DiskWatcher.h"

namespace Jde::IO
{
	void Scanner::Run()
	{
		auto pCallback = shared_from_this();
		//DiskWatcher::Add( fs::path("/mnt/2TB/securities"), pCallback, EDiskWatcherEvents::AllEvents );
	}
}