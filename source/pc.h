
//#include <SDKDDKVer.h>
#pragma warning( disable : 4245) 
#include <boost/crc.hpp> 
#pragma warning( default : 4245) 
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#ifndef __INTELLISENSE__
	#include <spdlog/spdlog.h>
	#include <spdlog/sinks/basic_file_sink.h>
	#include <spdlog/fmt/ostr.h>
#endif

#include <nlohmann/json.hpp>

#include "TypeDefs.h"
#include "JdeAssert.h"
#include "log/Logging.h"
#include "Settings.h"
#include "io/DiskWatcher.h"
#include "io/File.h"
