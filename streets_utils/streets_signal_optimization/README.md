# Streets Desired phase plan arbitrator

## Introduction

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



```
