#pragma once

#include <iostream>
#include <math.h>
#include <string>
#include <spdlog/spdlog.h>

namespace intersection_model
{
    /* MAP Enumerations */
    enum map_type
    {
        intersection = 1,
        roadway = 2
    };
    enum map_message_attributes
    {
        elevation = 1,
        decimeter = 2,
        geometric = 4,
        navigational = 8
    };
    enum map_node_attributes
    {
        width = 1,
        packed = 2
    };
    enum map_group_direction
    {
        approach = 1,
        egress = 2
    };
    enum map_lane_type
    {
        vehicle = 1,
        computed = 2,
        pedestrian = 3,
        special = 4,
        crosswalk = 10,
        bike = 11,
        sidewalk = 12,
        barrier = 13,
        striping = 14,
        trackedVehicle = 15,
        parking = 16
    };

    /* MAP Structures */
    struct map_referencelane
    {
        unsigned char lanenumber;
        signed short lateraloffset;
        signed short xoffset;
        signed short yoffset;
    };
    struct map_connection
    {
        long lane_id;
        unsigned char connectionID;
        int32_t signalGroup;
        unsigned short maneuver;
    };
    struct map_node
    {
        long x;
        long y;
    };
    struct map_lane
    {
        long lane_id;
        std::string laneName;
        unsigned char type;
        unsigned short attributes;
        unsigned short width;
        std::vector<map_node> nodes;
        map_referencelane referencelane;
        std::vector<map_connection> connection;
    };
    struct map_group
    {
        long width;
        std::vector<map_lane> lanes;
    };
    struct map_referencepoint
    {
        signed long latitude;
        signed long longitude;
        signed long elevation;
    };
    struct map_geometry
    {
        map_referencepoint refpoint;
        map_group approach; // ingress
        map_group egress;
    };

    /*!
     * Map structure containing the GID information for an intersection
     * \ingroup MAPPlugin
     */
    struct intersection_map
    {
        map_type mapType;
        unsigned long intersectionid;
        std::vector<map_geometry> geometries;
    };
}