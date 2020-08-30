cmake_minimum_required(VERSION 3.16)
project(Jde.Drive)
set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../Framework/cmake )
include(SetUpConfigurations)
link_directories( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
#######################################################

add_executable( Jde.Drive main.cpp Scan.cpp Drive.cpp Merge.cpp )
#######################################################
target_link_libraries(Jde.Drive boost_regex)
target_link_libraries(Jde.Drive Threads::Threads)
target_link_libraries(Jde.Drive Jde)
target_link_libraries(Jde.Drive Jde.Ssl)
target_link_libraries(Jde.Drive Jde.Google)
target_link_libraries(Jde.Drive dl)
target_link_libraries(Jde.Drive protobuf)
target_link_libraries(Jde.Drive ssl)
target_link_libraries(Jde.Drive crypto)
#target_link_libraries(Jde.Drive stdc++fs)
target_link_libraries(Jde.Drive stdc++)
#######################################################
target_precompile_headers(Jde.Drive
	PRIVATE
	<boost/crc.hpp>
	<boost/noncopyable.hpp>
	<boost/system/error_code.hpp>
	<spdlog/spdlog.h>
	<spdlog/sinks/basic_file_sink.h>
	<spdlog/fmt/ostr.h>
	<nlohmann/json.hpp>
	"../../Framework/source/TypeDefs.h"
	"../../Framework/source/JdeAssert.h"
	"../../Framework/source/log/Logging.h"
	"../../Framework/source/Settings.h"
	"../../Framework/source/io/DiskWatcher.h"
	"../../Framework/source/io/File.h"
)
