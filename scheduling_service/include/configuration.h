#ifndef configuration_H
#define configuration_H

#include <string>
#include <chrono>

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

	public:

		/* get the scheduling time interval */
		double get_schedulingDelta();

		/* get the time point when the last schedule started */
		double get_lastSchedulingT();

		/* get the time point when the current schedule started */
		double get_curSchedulingT();

		/* set the last schedule's start time point to t */
		void set_lastSchedulingT(double t);

		/* set the current schedule's start time point to t */
		void set_curSchedulingT(double t);

		/* set the scheduling time interval to delta */
		void set_schedulingDelta(double delta);
};

#endif