
#include "schedule_logger.h"

schedule_logger::schedule_logger(const configuration& config){
    
    _file_directory = config.get_logDirectory();
    _cur_file_name = _file_directory + _file_name + ".csv";

    if (config.isScheduleLoggerOn()){
        openLogFile();
    } 
    else{
        spdlog::info("The scheduling logger switch is not On!");
    }
}


bool schedule_logger::isFileOpen(){

    if (_log_file.is_open()){
        _open_status = true;
    } else{
        _open_status = false;
    }

    return _open_status;
}


void schedule_logger::openLogFile(){
    
    if (_log_file.is_open()){
        _open_status = true;
        spdlog::info("A log file is already open!");
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
                spdlog::critical("Faild to rename the log file from {0} to {1}!", _cur_file_name.c_str(), _new_file_name.c_str());
            }
        }

        _log_file.open(_cur_file_name);
        if (!_log_file.is_open()){
            spdlog::critical("Could not open a new log file!");
            exit(1);
        } 
        else{
            spdlog::info("A new log file has been created. New log file name: {0}", _cur_file_name);
            _open_status = true;
            _log_file << "Schedule Index, Schedule Timestamp, Vehicle ID, ";
        }

    }

}


void schedule_logger::saveSchedule(std::string schedule_info, bool is_schedule_logger_on){


    if (is_schedule_logger_on){
        if (_log_file.is_open()){
            
            _log_file << schedule_info;
            
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
    // else
    // {
    //     spdlog::info("The scheduling logger switch is not On!");
    // }

}


// void schedule_logger::closeFile(){
//     if (_log_file.is_open()){
//         _log_file.close();
//         _open_status = false;
//     }
//     else{
//         spdlog::info("There is no open log file!");
//     }
// }


/**
 * Returns the current data time as string.
 * @return current time in ddmmyyhhmiss format.
 */
std::string schedule_logger::GetCurDateTimeStr()
{   
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d%m%Y%H%M%S");
	auto str = oss.str();
	return str; 
}

