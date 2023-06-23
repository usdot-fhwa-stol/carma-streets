# Streets Timing Plan Command library
## Introduction
This CARMA-streets library is meant to handle JSON deserialization for external signal timing plan message generated by [MMITSS Roadside Processor] (https://github.com/mmitss/mmitss-az), and a sample signal timing plan message refers to https://github.com/mmitss/mmitss-az/blob/master/src/mrp/priority-request-solver/test/tci-msg-sender/signalPlan.json.

###  <b>Sample</b>
A schedule with commands:
```
{
    "MsgType": "ActiveTimingPlan",
    "TimingPlan": {
        "NoOfPhase": 8,
        "PhaseNumber": [
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8
        ],
        "PedWalk": [
            0,
            7,
            0,
            7,
            0,
            7,
            0,
            7
        ],
        "PedClear": [
            0,
            33,
            0,
            43,
            0,
            33,
            0,
            33
        ],
        "MinGreen": [
            4,
            15,
            4,
            15,
            4,
            15,
            4,
            15
        ],
        "Passage": [
            2.0,
            5.0,
            2.0,
            5.0,
            2.0,
            5.0,
            2.0,
            5.0
        ],
        "MaxGreen": [
            37,
            35,
            19,
            40,
            32,
            40,
            19,
            29
        ],
        "YellowChange": [
            3.0,
            4.0,
            3.0,
            3.6,
            3.0,
            4.0,
            3.0,
            3.6
        ],
        "RedClear": [
            1.0,
            2.5,
            1.0,
            3.4000000000000004,
            1.0,
            2.5,
            1.0,
            3.4000000000000004
        ],
        "PhaseRing": [
            1,
            1,
            1,
            1,
            2,
            2,
            2,
            2
        ]
    }
}
```
## Including library
```
find_package( streets_timing_plan_lib REQUIRED )
...
target_link_libraries( <target> PUBLIC streets_timing_plan_lib::streets_timing_plan_lib )
```