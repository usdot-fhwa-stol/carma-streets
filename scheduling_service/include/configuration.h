#ifndef configuration_H
#define configuration_H

#include <string>
#include <chrono>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace std;
using chrono::duration;

class configuration{
	private:

		/* the scheduling time interval */
		double scheduling_delta = 1.0; // sec

		/* If the last update of the vehicle is update_expiration_delta seconds older than the current time, the vehicle will not be included in the schedule
		*  Unit: second
		 */
		double update_expiration_delta;

		/* stopping distance paramter 
		*  unit: meter 
		*/
		double stopping_distance;

		/* stopping speed parameter 
		*  unit: meter/sec
		*/
		double stopping_speed;

		/* maximum valid speed (a speed higher than this parameter will count as an invalid speed)
		*  unit: meter/sec
		*/
		double max_valid_speed;

		/* Flag to indicate whether schedule logs are enabled */
		bool schedule_log_enabled;

		/* String schedule log directory path*/
		string schedule_log_path;
		/* String schedule log filename prefix */
		string schedule_log_filename;
		/* Schedule log max size in megabytes */
		int schedule_log_maxsize;

	public:

		/* initialization: reading the EXPIRATION_DELTA from ../manifest.json */
		configuration();

		/* get the scheduling time interval */
		double get_schedulingDelta() const;

		/* get the vehicle status and intent update expiration time interval */
		double get_expDelta() const;

		/* get the stopping distance condition */
		double get_stopDistance() const;

		/* get the stopping speed condition */
		double get_stopSpeed() const;

		/* get the max valid speed */
		double get_maxValidSpeed() const;

		/* get the schedule log path */
		string get_scheduleLogPath() const;

		/* is schedule log enabled */ 
		bool isScheduleLoggerEnabled() const;

		/**
		 * @return schedule log file name prefix.
		 **/
		string get_scheduleLogFilename() const;

		/**
		 * @return schedule log file max size in megabytes.
		 **/
		int get_scheduleLogMaxsize() const;

		/* set the scheduling time interval to delta */
		void set_schedulingDelta(double delta);

		/* set the vehicle status and intent update expiration time interval to delta*/
		void set_expDelta(double delta);
		
		/* set the stopping distance condition to ds */
		void set_stopDistance(double ds);

		/* set the stopping speed condition to speed */
		void set_stopSpeed(double speed);

		/* set the max valid speed to speed */
		void set_maxValidSpeed(double speed);

};

#endif
