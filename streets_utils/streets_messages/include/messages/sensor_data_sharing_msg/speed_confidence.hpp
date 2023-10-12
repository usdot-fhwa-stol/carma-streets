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

namespace streets_utils::messages{
    enum class speed_confidence {
        UNAVAILABLE = 0,    // Not available
        PREC_100ms = 1,     // 100 m/s
        PREC_10ms = 2,      // 10 m/s
        PREC_5ms = 3,       // 5 m/s
        PREC_1ms = 4,       // 1 m/s
        PREC_0_1ms = 5,     // 0.1 m/s
        PREC_0_05ms = 6,    // 0.05 m/s
        PREC_0_01ms = 7     // 0.01 m/s
    };

    speed_confidence speed_confidence_from_int( const int i );

}