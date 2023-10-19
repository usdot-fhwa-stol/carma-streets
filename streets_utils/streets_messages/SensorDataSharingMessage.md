# Sensor Data Sharing Message

## Introduction

This CARMA-Streets library contains the Sensor Data Sharing Object as well as the logic for serailizing and deserializing this object to and from JSON. Below is an example JSON payload. This message will be produced by the **Sensor Data Sharing** CARMA Streets micro-service. The data in it will represent the incoming **Sensor Detected Object** detection. This message will be generated inline with the SAE J3224 document.
> [!IMPORTANT]\
> The first implementation of the SDSM generator will not include functionality to filter out detections that are likely self reporting actors publishing BSMs like described in the J3224 202208 document.
## Example JSON payload
```
{
  "msg_cnt": 1,
  "source_id": "00000001",
  "equipment_type": 2,
  "sdsm_time_stamp": {
    "second": 0,
    "minute": 0,
    "hour": 0,
    "day": 0,
    "month": 0,
    "year": 0,
    "offset": -32767
  },
  "ref_pos": {
    "long": -1800000000,
    "lat": -90000000
  },
  "ref_pos_xy_conf": {
    "semi_major": 0,
    "semi_minor": 0,
    "orientation": 0
  },
  "objects": [
    {
      "detected_object_common_data": {
        "obj_type": 1,
        "object_id": 0,
        "obj_type_cfd": 0,
        "measurement_time": -1500,
        "time_confidence": 39,
        "pos": {
          "offset_x": -32767,
          "offset_y": -32767,
          "offset_z": -32767
        },
        "pos_confidence": {
          "pos": 1,
          "elavation": 15
        },
        "speed": 0,
        "speed_confidence": 21845,
        "heading": 0,
        "heading_conf": 7
      },
      "detected_object_optional_data": {
        "detected_vehicle_data": {
          "lights": "11110000",
          "veh_attitude": {
            "pitch": 72000,
            "roll": 14400,
            "yaw": 14400
          },
          "veh_attitude_confidence": {
            "pitch_confidence": 5,
            "roll_confidence": 1,
            "yaw_confidence": 3
          },
          "veh_ang_vel": {
            "pitch_rate": 32767,
            "roll_rate": 32767
          },
          "veh_ang_vel_confidence": {
            "pitch_rate_confidence": 4
          },
          "size": {
            "width": 4095,
            "length": 4095
          },
          "vehicle_size_confidence": {
            "vehicle_width_confidence": 13,
            "vehicle_length_confidence": 13,
            "vehicle_height_confidence": 13
          },
          "height": 127,
          "vehicle_class": 23,
          "class_conf": 101
        }
      }
    }
  ]
}
```

 
