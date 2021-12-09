
#include "configuration.h"

using namespace std;

/* */
double configuration::get_schedulingDelta(){return scheduling_delta;}

/* */
double configuration::get_lastSchedulingT(){return last_schedule_start_time;}

/* */
double configuration::get_curSchedulingT(){return cur_schedule_start_time;}

/* */
double configuration::get_expDelta(){return update_expiration_delta;}

/* */
void configuration::set_lastSchedulingT(double t){last_schedule_start_time = t;}

/* */
void configuration::set_curSchedulingT(double t){cur_schedule_start_time = t;}

/* */
void configuration::set_schedulingDelta(double delta){scheduling_delta = delta;}

/* */
void configuration::set_expDelta(double delta){update_expiration_delta = delta;}

