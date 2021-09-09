
#include <string>
#include <fstream>
#include <iostream>
#include <chrono> 
#include "json.hpp"

#include "configuration.h"

using namespace std;
using json = nlohmann::json;
using chrono::duration;

/* */
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

/* */
double configuration::get_consumerDelta(){return consumer_delta;}

/* */
double configuration::get_schedulingDelta(){return scheduling_delta;}

/* */
string configuration::get_shutdownSwitch(){return shutdown_switch;}

/* */
double configuration::get_lastSchedulingT(){return last_schedule_start_time;}

/* */
double configuration::get_curSchedulingT(){return cur_schedule_start_time;}

/* */
void configuration::start(){shutdown_switch = "on";}

/* */
void configuration::shutDown(){shutdown_switch = "off";}

/* */
void configuration::set_lastSchedulingT(double t){last_schedule_start_time = t;}

/* */
void configuration::set_curSchedulingT(double t){cur_schedule_start_time = t;}

/* */
void configuration::set_consumerDelta(double delta){consumer_delta = delta;}

/* */
void configuration::set_schedulingDelta(double delta){scheduling_delta = delta;}



