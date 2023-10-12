// Copyright 2023 Leidos
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
#pragma once

namespace streets_utils::messages {
    enum class attachment {
        UNAVAILABLE = 0 ,
        STROLLER = 1,
        BICYLE_TRAILER = 2,
        CART = 3,
        WHEEL_CHAIR = 4,
        OTHER_WALK_ASSIST_ATTACHMENTS = 5,
        PET = 6
    };

    inline attachment attachment_from_int( const int i ){
        switch (i)
        {
        case 0:
            return attachment::UNAVAILABLE;
        case 1:
            return attachment::STROLLER;
        case 2:           
            return attachment::BICYLE_TRAILER;
        case 3:
            return attachment::CART;
        case 4:
            return attachment::WHEEL_CHAIR;
        case 5:
            return attachment::OTHER_WALK_ASSIST_ATTACHMENTS;
        case 6:
            return attachment::PET;
        default:           
            return attachment::UNAVAILABLE;
        }
    }

}