#include "gtest/gtest.h"
#include "csv_logger.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"


TEST(test_file_creation, csv_logger_test)
{
    ASSERT_FALSE(boost::filesystem::exists("../test/schedulingLogs.csv"));
    csv_logger *logger = new csv_logger("../test","testLogs", 1 );
    logger->log_line("Test Schedule");
    delete logger;

    int log_files = 0;
    for (const auto & entry : boost::filesystem::directory_iterator("../test/logs/")){
        log_files++;
        spdlog::info("File name : {0}", entry.path().c_str() );
        boost::filesystem::remove( entry.path() );
    }
    
    ASSERT_EQ(log_files, 1);
}

TEST(test_file_rotation, csv_logger_test ) {

    csv_logger *logger =  new csv_logger("../test","testLogs", 1 );
    std::ifstream file("../test/sample_files/sample.csv");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
           logger->log_line(line);
        }
        file.close();
    }
    delete logger;

    int log_files = 0;
    for (const auto & entry : boost::filesystem::directory_iterator("../test/logs/")){
        log_files++;
        spdlog::info("File name : {0}", entry.path().c_str() );
        boost::filesystem::remove( entry.path() );
    }
    
    ASSERT_EQ(log_files, 2);

}