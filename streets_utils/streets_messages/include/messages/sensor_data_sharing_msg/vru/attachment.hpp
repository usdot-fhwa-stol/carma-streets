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

    attachment attachment_from_int( const int i );

}