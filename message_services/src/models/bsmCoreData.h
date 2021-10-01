#ifndef BSM_CORE_DATA_H
#define BSM_CORE_DATA_H

#include <spdlog/spdlog.h>
#include <ctime>
#include "baseMessage.h"
#include "header.h"

namespace message_services
{

    namespace models
    {
        typedef enum BrakeAppliedStatus
        {
            BrakeAppliedStatus_unavailable = 0,
            BrakeAppliedStatus_leftFront = 1,
            BrakeAppliedStatus_leftRear = 2,
            BrakeAppliedStatus_rightFront = 3,
            BrakeAppliedStatus_rightRear = 4
        } BrakeAppliedStatus_t;

        typedef struct AccelerationSet4Way
        {
            float Long = 0.0;
            float lat = 0.0;
            float vert = 0.0;
            float yaw = 0.0;
        } AccelerationSet4Way_t;

        typedef struct PositionalAccuracy
        {
            float semiMajor;
            float semiMinor;
            float orientation;
        } PositionalAccuracy_t;

        typedef struct BrakeSystemStatus
        {
            BrakeAppliedStatus_t wheelBrakes;
            long traction = 0;
            long abs = 0;
            long scs = 0;
            long brakeBoost = 0;
            long auxBrakes = 0;
        } BrakeSystemStatus_t;

        typedef struct VehicleSize
        {
            long width = 0;
            long length = 0;
        } VehicleSize_t;

        typedef struct bsmCoreData
        {
            std::string temprary_id = "";
            long msg_count = 0;
            long sec_mark = 0;
            double latitude = 0.0;
            double longitude = 0.0;
            float elev = 0.0;
            PositionalAccuracy_t accuracy;
            std::string transmission;
            float speed = 0.0;
            float heading = 0.0;
            float angle = 0.0;
            AccelerationSet4Way accelSet;
            BrakeSystemStatus_t brakes;
            VehicleSize size;
        } bsmCoreData_t;
    }

}

#endif