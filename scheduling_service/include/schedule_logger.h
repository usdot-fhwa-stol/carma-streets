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
		
		/*** ??
		* @brief ??
		*/
		std::ofstream _log_file;

		std::string _file_name = "schedulingLogs";

		std::string _cur_file_name;

		std::string _new_file_name;

		std::string _file_directory;

		int _log_file_size;
		
		int _log_file_size_inMB;

		int _log_file_size_inMB_max = 50;

		bool _open_status;

		std::string GetCurDateTimeStr();

	public:

		schedule_logger(const configuration& config);

		void openLogFile();

		void saveSchedule(std::string schedule_info, bool is_schedule_logger_on);

		bool isFileOpen();

		// void closeFile();

};


#endif
