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
### Message specs
| Parameter Name | Description |
| -------------- | ----------- |
| timestamp | The unix timestamp in milliseconds when the desired phase plan is publised by signal optimization service.| 
| desired_phase_plan | The content of desired phase plan publised by signal optimization service.|
| signal_groups | An array of signal group ids that are in the same barrier but different rings. The arary length should be 2. |
| start_time | The unix timestamp in milliseconds when event state assigned to the signal groups starts to turn green.  |
| end_time | The unix timestamp in milliseconds when green event state assigned to the signal groups ends. |
