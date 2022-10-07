# Streets Signal Optimization Library

## Introduction
This library is intended to generate a list of candidate desired phase plans for **CARMA Streets**. This library consists of one main class: the **desired_phase_plan_generator** class.

## desired_phase_plan_generator
The **desired_phase_plan_generator** is a library that generates a list of candidate desired phase plans. Each desired phase plan has a list of movement groups with their start times, end times, and the included signal groups (Please refer to the desired phase plan library's documentation for more information about this library). The main method that returns the desired phase plan list is **desired_phase_plan_generator::generate_desire_phase_plan_list.()** which takes a vehicle id to vehicle object (**streets_vehicle_list::vehicle**) mapping, a modified spat object (**signal_phase_and_timing::spat**), and a list of candidate movement groups (**streets_signal_optimization::movement_groups &move_groups**).
In order to succussfully run the **desired_phase_plan_generator::generate_desire_phase_plan_list.()** method, the **intersection_info_ptr** has to be set first. Also, the **desired_phase_plan_generator** has a **desired_phase_plan_generator_configuration** object. If the configurations are not set, they will be set to their default values.


```
An example of the desired phase plan list:

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