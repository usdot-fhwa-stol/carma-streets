
#include "csv_logger.h"

csv_logger::csv_logger( std::string log_directory, std::string filename, int filesize_max) : 
_file_directory(log_directory), _file_name(filename), _log_file_size_inMB_max(filesize_max) ,_cur_file_name(_file_directory + _file_name + ".csv") 
{
    openLogFile();
}

csv_logger::~csv_logger() {
    spdlog::info("Logger object deconstructor called");
    // Close file and rename with date
    if (_log_file.is_open()) {
        _log_file.close();
    }
    renameAndMoveLogfile();
}




void csv_logger::openLogFile(){
    
    if (_log_file.is_open()){
        spdlog::debug("A log file is already open!");
    } 
    else{
        spdlog::info("Opening a Log file.");
        if (!boost::filesystem::exists(_file_directory + "logs/")){
            boost::filesystem::create_directory(_file_directory + "logs/");   
	    }

        renameAndMoveLogfile();

        _log_file.open(_cur_file_name);
        if (!_log_file.is_open()){
            spdlog::critical("Could not open a new log file!");
            exit(1);
        } 

    }

}

void csv_logger::renameAndMoveLogfile(){
    std::string _new_file_name = _file_directory + "/logs/" + _file_name + "_" + GetCurDateTimeStr() + ".csv";

    if ( boost::filesystem::exists(_new_file_name.c_str( ) ) ) {
            _new_file_name.append("(1)");
    }
    
    int error;
    if (boost::filesystem::exists(_cur_file_name.c_str())){
        error = std::rename(_cur_file_name.c_str(), _new_file_name.c_str() );
        if ( error != 0 ) {
            spdlog::critical("Failed to rename the log file from {0} to {1}!", _cur_file_name.c_str(), _new_file_name.c_str());
        }
        else{
            spdlog::info("Create csv log file {0} !", _new_file_name.c_str() );
        }
    }
}


void csv_logger::log_line(std::string new_line){
    // File size check
     fileSizeCheck();
    // Write log and append endline character
    _log_file << new_line + "\n";
}



std::string csv_logger::GetCurDateTimeStr() const
{   
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d%m%Y%H%M%S");
	auto str = oss.str();
	return str; 
}

void csv_logger::fileSizeCheck(){
    // Check if there is an open file
    if (_log_file.is_open()){
        // Check file size in bytes
        int _log_file_size = boost::filesystem::file_size( _cur_file_name );
        // Convert to Mb
        long _log_file_size_inMB = _log_file_size/1048576;
        // Close file if larger that Mb limit and open new file
        if (_log_file_size_inMB >= _log_file_size_inMB_max)
        {
            spdlog::info("Current file size in bytes {0}", _log_file_size );
            _log_file.close();
            openLogFile();
        }

    }
    else{
        spdlog::critical("There is no open log file!");
        exit(1);
    }
}

