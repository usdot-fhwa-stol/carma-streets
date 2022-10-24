# Streets Signal Optimization Library

## Introduction
This library is intended to contain logic for selecting/generating an optimal desired phase plan. This library consists of two main classes: **streets_desired_phase_plan_generator** and **streets_desired_phase_plan_arbitrator**. In summary, **streets_desired_phase_plan_generator** generates a list of candidate desired phase plans where each desired phase plan has one extra fixed future movement group compared with the modified spat. **streets_desired_phase_plan_arbitrator** selects the one that maximizes/minimizes the objective measure which is a function of vehicle delays.


## streets_desired_phase_plan_generator
The **streets_desired_phase_plan_generator** generates a list of candidate desired phase plans. Each desired phase plan has a list of movement groups with their start times, end times, and the included signal groups [streets_desired_phase_plan library documentation](./streets_desired_phase_plan/README.md).

The main method is **streets_desired_phase_plan_generator::generate_desire_phase_plan_list.()** which returns the desired phase plan list and takes the following parameters:
- intersection_info pointer (OpenAPI::OAIIntersection_info), this pointer has the intersection geometry information.
- vehicle id to vehicle object (**streets_vehicle_list::vehicle**) mapping.
- intersection_state object from spat (**signal_phase_and_timing::intersection_state**).
- and a list of candidate movement groups (**streets_signal_optimization::movement_groups**).
If the returned desired phase plan list is empty, either there are no vehicles that can enter the intersection box during any new fixed future movement group, or the number of fixed future movement groups are greater than or equal to the desired number of fixed futre movement groups.

**streets_desired_phase_plan_generator** allows you to configure the **streets_desired_phase_plan_generator_configuration** parameters and **intersection_info_ptrstreets_vehicle_scheduler::signalized_vehicle_scheduler** publicly. Note that if the configuration parameters are not set before **streets_desired_phase_plan_generator::generate_desire_phase_plan_list.()** method, this method will set the configuration parameters to a set of default values.


# streets_desired phase plan arbitrator
The desired phase plan arbitrator include a series of steps to choose the optimzied desired phase plan from a list of orignal candidate desired phase plan.

The sample data for incoming list of candidate desire phase plan in JSON format is below:

```
{
  "desired_phase_plan_list": [
    {
      "timestamp": 12121212121,
      "desired_phase_plan": [
        {
          "signal_groups": [1, 5],//MG A
          "start_time": 1660747993,
          "end_time": 1660747998
        },
        {
          "signal_groups": [2, 6],//MG D
          "start_time": 1660747993,
          "end_time": 1660747998
        },
        {
          "signal_groups": [3, 7],//MG E
          "start_time": 1660747993,
          "end_time": 1660747998
        } //Possible option1 //MG E
      ]
    },
    {
        "timestamp": 12121212121,
        "desired_phase_plan": [
          {
            "signal_groups": [1, 5],//MG A
            "start_time": 1660747993,
            "end_time": 1660747998
          },
          {
            "signal_groups": [2, 6],//MG D
            "start_time": 1660747993,
            "end_time": 1660747998
          },
          {
            "signal_groups": [3, 7],//MG E
            "start_time": 1660747993,
            "end_time": 1660748998
          } //Possible option2 MG E
        ]
      },
      {
          "timestamp": 12121212121,
          "desired_phase_plan": [
            {
              "signal_groups": [1, 5],//MG A
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups": [2, 6],//MG D
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups": [4,7],//MG E
              "start_time": 1660747993,
              "end_time": 1660748998
            } //Possible option3 MG E
          ]
        },
      {
          "timestamp": 12121212121,
          "desired_phase_plan": [
            {
              "signal_groups": [1, 5],//MG A
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups": [2, 6],//MG D
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups": [3,7],//MG E
              "start_time": 1660747993,
              "end_time": 1660749998
            } //Possible option4 MG E
          ]
        },
        {
      "timestamp": 12121212121,
      "desired_phase_plan": [
        {
          "signal_groups": [1, 5],//MG A
          "start_time": 1660747993,
          "end_time": 1660747998
        },
        {
          "signal_groups": [2, 6],//MG D
          "start_time": 1660747993,
          "end_time": 1660747998
        },
        {
          "signal_groups": [4, 8],//MG H
          "start_time": 1660747993,
          "end_time": 1660747998
        } //Possible option1 //MG H
      ]
    },
    {
        "timestamp": 12121212121,
        "desired_phase_plan": [
          {
            "signal_groups": [1, 5],//MG A
            "start_time": 1660747993,
            "end_time": 1660747998
          },
          {
            "signal_groups": [2, 6],//MG D
            "start_time": 1660747993,
            "end_time": 1660747998
          },
          {
            "signal_groups":  [4, 8],//MG H
            "start_time": 1660747993,
            "end_time": 1660748998
          } //Possible option2 MG H
        ]
      },
      {
          "timestamp": 12121212121,
          "desired_phase_plan": [
            {
              "signal_groups": [1, 5],//MG A
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups": [2, 6],//MG D
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups":  [4, 8],//MG H
              "start_time": 1660747993,
              "end_time": 1660748998
            } //Possible option3 MG H
          ]
        },
      {
          "timestamp": 12121212121,
          "desired_phase_plan": [
            {
              "signal_groups": [1, 5],//MG A
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups": [2, 6],//MG D
              "start_time": 1660747993,
              "end_time": 1660747998
            },
            {
              "signal_groups":  [4, 8],//MG H
              "start_time": 1660747993,
              "end_time": 1660749998
            } //Possible option4 MG H
          ]
        }
  ]
}
```
- Signal group: A single movement or group of movements in an intersection that are controlled by a single phase. 
![image](https://user-images.githubusercontent.com/62157949/194932912-faca0739-7212-452c-a723-8f446f90ac00.png)



- Step 1: Iterate over all movement groups in each candidate desired phase plan.
  - Step 2: Make a local copy of SPAT and Update SPAT with each candidate desired phase plan iteration
  - Step 3: Given the SPAT and list of vehicles near the intersection, calculating estimated entering time (ET) and get vehicles' schedule
  - Step 4: Given vehicle schedule, calculating delay measure. Keep track of each candidate desired phase plan and delay measure mapping
- Step 5: Find the highest delay measure, its mapping candidate desired phase plan is the chosen desired phase plan.

## Use of desired phase plan arbitrator lib
- streets_desired_phase_plan_arbitrator class provides algorithm or call other library to determine the optimized desired phase plan.

```
streets_desired_phase_plan select_optimal_dpp( 
                  dpp_list, 
                  intersection_info_ptr, 
                  spat_ptr, 
                  tsc_config_state, 
                  veh_list_ptr, 
                  initial_green_buffer, 
                  inal_green_buffer)

Type Parameters:
  dpp_list - std::vector<streets_desired_phase_plan::streets_desired_phase_plan>
  intersection_info_ptr - std::shared_ptr<OpenAPI::OAIIntersection_info> 
  spat_ptr - std::shared_ptr<signal_phase_and_timing::spat>
  tsc_config_state - std::shared_ptr<streets_tsc_configuration::tsc_configuration_state>
  veh_list_ptr - const std::shared_ptr<streets_vehicles::vehicle_list>
  initial_green_buffer - uint64_t
  final_green_buffer - uint64_t

Parameters:
  dpp_list - List of candidate desired phase plan list
  intersection_info_ptr - A pointer to intersection model information 
  spat_ptr - Streets defined signal timing and phasing message type
  tsc_config_state - Traffic signal controller configuration state
  veh_list_ptr - A pointer to a list of vehicles
  initial_green_buffer - Configuration parameter for initial green buffer
  final_green_buffer - Configuration parameter for final green buffer

Returns:
  An instance of desired phase plan.
```
