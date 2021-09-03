#ifndef configuration_H
#define configuration_H

#include <string>
#include <fstream>
#include <iostream>
#include <chrono> 
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
using chrono::duration;

class configuration{
	private:
		double consumer_delta;
		double scheduling_delta;
		string shutdown_switch;

		double last_schedule_start_time = duration<double>(chrono::system_clock::now().time_since_epoch()).count();
		double cur_schedule_start_time;

	public:

		configuration(string json_file_name);

		double get_consumerDelta(){return consumer_delta;}
		double get_schedulingDelta(){return scheduling_delta;}
		string get_shutdownSwitch(){return shutdown_switch;}
		double get_lastSchedulingT(){return last_schedule_start_time;}
		double get_curSchedulingT(){return cur_schedule_start_time;}

		void start(){shutdown_switch = "on";}
		void shutDown(){shutdown_switch = "off";}
		void set_lastSchedulingT(double t){last_schedule_start_time = t;}
		void set_curSchedulingT(double t){cur_schedule_start_time = t;}

		void set_consumerDelta(double delta){consumer_delta = delta;}
		void set_schedulingDelta(double delta){scheduling_delta = delta;}
};

configuration::configuration(string json_file_name){
	ifstream file_input(json_file_name);
	if (file_input){
		
		json json_config;
		file_input >> json_config;

		consumer_delta = json_config["consumer_delta"];
		scheduling_delta = json_config["scheduling_delta"];
		shutdown_switch = "on";

		cout << "successful" << endl;

	}else{
		cout << "error - configuration file " + json_file_name + " deos not exist!" << endl;
	}
	file_input.close();
}


#endif