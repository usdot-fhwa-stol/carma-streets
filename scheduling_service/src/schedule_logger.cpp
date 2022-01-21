
#include "schedule_logger.h"

schedule_logger::schedule_logger(const std::string log_directory){
    
    _file_directory = log_directory;
    _cur_file_name = _file_directory + _file_name + ".csv";
    openLogFile();
}




void schedule_logger::openLogFile(){
    
    if (_log_file.is_open()){
        spdlog::debug("A log file is already open!");
    } 
    else{
        
        if (!boost::filesystem::exists(_file_directory + "logs/")){
            boost::filesystem::create_directory(_file_directory + "logs/");   
	    }
        _new_file_name = _file_directory + _file_name + "_" + GetCurDateTimeStr() + ".csv";
        
        int error;
        if (boost::filesystem::exists(_cur_file_name.c_str())){
            error = std::rename(_cur_file_name.c_str(), _new_file_name.c_str() );
            if ( error != 0 ) {
                spdlog::critical("Failed to rename the log file from {0} to {1}!", _cur_file_name.c_str(), _new_file_name.c_str());
            }
        }

        _log_file.open(_cur_file_name);
        if (!_log_file.is_open()){
            spdlog::critical("Could not open a new log file!");
            exit(1);
        } 
        else{
            spdlog::info("A new log file has been created. New log file name: {0}", _cur_file_name);
        }

    }

}


void schedule_logger::log_schedule(std::string schedule_info){
     fileSizeCheck();
    _log_file << schedule_info;
}



std::string schedule_logger::GetCurDateTimeStr()
{   
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d%m%Y%H%M%S");
	auto str = oss.str();
	return str; 
}

void schedule_logger::fileSizeCheck(){
    // Check if there is an open file
    if (_log_file.is_open()){
        // Check
        std::fstream log_file_size(_cur_file_name);
        log_file_size.seekg(0, std::ios::end);
        _log_file_size = log_file_size.tellg();
        _log_file_size_inMB = _log_file_size/1048576;
        if (_log_file_size_inMB >= _log_file_size_inMB_max)
        {
            _log_file.close();
            openLogFile();
        }

    }
    else{
        spdlog::critical("There is no open log file!");
        exit(1);
    }
}

