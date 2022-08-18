# Streets Desired phase plan

## Introduction

This CARMA-Streets library is meant to handle JSON serialization and deserialization for the CARMA-Streets internal Desired Phase Plan JSON message.Below is a sample Desired Phase Plan JSON message created by this library.

```
{
	"timestamp": 12121212121,
	"desired_phase_plan": [
        {
            "signal_groups": [1, 5],
            "start_time": 1660747993,
            "end_time": 1660747998
	    },
        {
            "signal_groups": [2, 6],
            "start_time": 1660747993,
            "end_time": 1660747998
	    }
    ]
}
```
