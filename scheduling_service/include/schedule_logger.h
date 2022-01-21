#ifndef schedule_logger_H
#define schedule_logger_H

#include <string>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <sstream>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#include "configuration.h"

using namespace std;
using namespace boost::filesystem;


class schedule_logger{
	private:
		
		
		std::ofstream _log_file;

		std::string _file_name = "schedulingLogs";

		std::string _cur_file_name;

		std::string _new_file_name;

		std::string _file_directory;

		int _log_file_size;
		
		int _log_file_size_inMB;

		int _log_file_size_inMB_max = 50;

		std::string GetCurDateTimeStr();

		void openLogFile();


		void fileSizeCheck();

	public:

		schedule_logger( std::string log_directory );
		void log_schedule( std::string schedule_info );

		

};


#endif
