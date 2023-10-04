# Sensor Data Sharing Message

## Introduction

This CARMA-Streets library contains the Sensor Data Sharing Object as well as the logic for serailizing and deserializing this object to and from JSON. Below is an example JSON payload. This message will be produced by the **Sensor Data Sharing** CARMA Streets micro-service. The data in it will represent the incoming **Sensor Detected Object** detection. This message will be generated inline with the SAE J3224 document.
> [!IMPORTANT]\
> The first implementation of the SDSM generator will not include functionality to filter out detections that are likely self reporting actors publishing BSMs like described in the J3224 202208 document.
```
{
  "equipment_type": 1,
  "msg_cnt": 1,
  "ref_pos": {
    "long": 600000000,
    "elevation": 30,
    "lat": 400000000
  },
  "ref_pos_el_conf": 10,
  "ref_pos_xy_conf": {
    "orientation": 25000,
    "semi_major": 235,
    "semi_minor": 200
  },
  "sdsm_time_stamp": {
    "day": 4,
    "hour": 19,
    "minute": 15,
    "month": 7,
    "offset": 400,
    "second": 5000,
    "year": 2007
  },
  "source_id": "01020304",
  "objects": [
    {
      "detected_object_data": {
        "detected_object_common_data": {
          "acc_cfd_x": 4,
          "acc_cfd_y": 5,
          "acc_cfd_yaw": 3,
          "acc_cfd_z": 6,
          "accel_4_way": {
            "lat": -500,
            "long": 200,
            "vert": 1,
            "yaw": 400
          },
          "heading": 16000,
          "heading_conf": 4,
          "measurement_time": -1100,
          "object_id": 12200,
          "obj_type": 1,
          "obj_type_cfd": 65,
          "pos": {
            "offset_x": 4000,
            "offset_y": -720,
            "offset_z": 20
          },
          "pos_confidence": {
            "elevation": 5,
            "pos": 2
          },
          "speed": 2100,
          "speed_confidence": 3,
          "speed_confidence_z": 4,
          "speed_z": 1000,
          "time_confidence": 2
        }
      }
    }
  ]
}
```

### 
