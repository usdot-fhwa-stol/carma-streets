// Copyright 2026 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "detection_offset_from_ref_converter.hpp"

#include <iostream>

namespace sensor_data_sharing_service {
    streets_utils::messages::detected_objects_msg::detected_objects_msg detected_object_local_to_ref(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg, const std::string reference_proj_string) {
        
        streets_utils::messages::detected_objects_msg::detected_objects_msg ref_detection(msg);

        PJ_CONTEXT *ctx = proj_context_create();
        if(!ctx){
            ref_detection._position._x = 0.0;
            ref_detection._position._y = 0.0;
            return ref_detection;
        }

        // Convert proj strings to char arr
        const char* detection_proj = msg._proj_string.c_str();
        const char* reference_proj = reference_proj_string.c_str();

        // Creation of proj transformation object between the two proj strings
        PJ *detection_to_ref = proj_create_crs_to_crs(ctx, detection_proj, reference_proj, NULL);
        if(!detection_to_ref){
            proj_context_destroy(ctx);
            ref_detection._position._x = 0.0;
            ref_detection._position._y = 0.0;
            return ref_detection;
        }

        // Relate values to PJ_COORD in cartesian
        PJ_COORD detection_cartesian = proj_coord(msg._position._x, msg._position._y, 0.0, 0.0);

        // Transform cartesian points into the shared reference crs
        PJ_COORD detection_transformed = proj_trans(detection_to_ref, PJ_FWD, detection_cartesian);

        // Calculate cartesian offset
        ref_detection._position._x = detection_transformed.xy.x;
        ref_detection._position._y = detection_transformed.xy.y;

        // Update the detection to use the reference proj string
        ref_detection._proj_string = reference_proj_string;

        proj_destroy(detection_to_ref);
        proj_context_destroy(ctx);

        return ref_detection;   
    }
}

