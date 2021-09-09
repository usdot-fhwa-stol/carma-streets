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

		double get_consumerDelta();
		double get_schedulingDelta();
		string get_shutdownSwitch();
		double get_lastSchedulingT();
		double get_curSchedulingT();

		void start();
		void shutDown();
		void set_lastSchedulingT(double t);
		void set_curSchedulingT(double t);

		void set_consumerDelta(double delta);
		void set_schedulingDelta(double delta);
};

#endif