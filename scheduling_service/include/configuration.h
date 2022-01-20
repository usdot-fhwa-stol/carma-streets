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

		/* the time point when the last schedule started */
		double last_schedule_start_time = duration<double>(chrono::system_clock::now().time_since_epoch()).count();

		/* the time point when the current schedule started */
		double cur_schedule_start_time;

		/* the index of the current schedule*/
		int cur_schedule_index = 0;

		/* the index of the current schedule*/
		int last_nonempty_schedule_index = 0;

		/* the index of the current schedule*/
		int max_empty_schedule_count = 10;

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

		/* a binary variable indicating whether the scheduling_service logs shall be stored in a CSV file or not */
		bool schedule_logger_switch;

		/* log file directory */
		string file_directory;

	public:

		/* initialization: reading the EXPIRATION_DELTA from ../manifest.json */
		configuration();

		/* get the scheduling time interval */
		double get_schedulingDelta() const;

		/* get the time point when the last schedule started */
		double get_lastSchedulingT() const;

		/* get the time point when the current schedule started */
		double get_curSchedulingT() const;

		/* get the current schedule index */
		int get_curScheduleIndex() const;

		/* get the last nonempty schedule index */
		int get_lastNonemptyScheduleIndex() const;

		/* get the maximum number of empty schedule before reseting the scheduling index to 0 */
		int get_maxEmptyScheduleCount() const;

		/* get the vehicle status and intent update expiration time interval */
		double get_expDelta() const;

		/* get the stopping distance condition */
		double get_stopDistance() const;

		/* get the stopping speed condition */
		double get_stopSpeed() const;

		/* get the max valid speed */
		double get_maxValidSpeed() const;

		/* get the schedule log directory */
		string get_logDirectory() const;

		/* get the message_logger switch status: true = on, false = off */ 
		bool isScheduleLoggerOn() const;

		/* set the last schedule's start time point to t */
		void set_lastSchedulingT(double t);

		/* set the current schedule's start time point to t */
		void set_curSchedulingT(double t);

		/* set the current schedule index */
		void set_curScheduleIndex(int index);

		/* set the last nonempty schedule index */
		void set_lastNonemptyScheduleIndex(int index);

		/* set the maximum number of empty schedule to count */
		void set_maxEmptyScheduleCount(int count);

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
