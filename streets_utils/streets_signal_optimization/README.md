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