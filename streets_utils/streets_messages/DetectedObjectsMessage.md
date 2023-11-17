# Detected Objects Message

## Introduction

This CARMA-Streets library contains the Detected Objects Message as well as the logic for deserializing this object from JSON. Below is an example JSON payload. This message will be produced by an external Sensor capable of detecting objects. The data in it will represent the incoming **Sensor Detected Object** detection. This is a custom message definition for the detected objects.

## Example JSON payload
```
{
  "type": "CAR",
  "confidence": 0.7,
  "sensorId": "sensor1",
  "projString": "projection String2",
  "objectId": "Object7",
  "position": {
    "x": -1.1,
    "y": -2,
    "z": -3.2
  },
  "positionCovariance": [
    [
      1,
      0,
      0
    ],
    [
      1,
      0,
      0
    ],
    [
      1,
      0,
      0
    ]
  ],
  "velocity": {
    "x": 1,
    "y": 1,
    "z": 1
  },
  "velocityCovariance": [
    [
      1,
      0,
      0
    ],
    [
      1,
      0,
      0
    ],
    [
      1,
      0,
      0
    ]
  ],
  "angularVelocity": {
    "x": 0.1,
    "y": 0.2,
    "z": 0.3
  },
  "angularVelocityCovariance": [
    [
      1,
      0,
      0
    ],
    [
      1,
      0,
      0
    ],
    [
      1,
      0,
      0
    ]
  ],
  "size": {
    "length": 2,
    "height": 1,
    "width": 0.5
  }
}
```
