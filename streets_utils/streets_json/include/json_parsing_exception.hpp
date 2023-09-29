#pragma once

#include <stdexcept>



namespace streets_utils::json_utils {
    /**
     * @brief Runtime error related to processing signal_phase_and_timing JSON updates.
     * 
     * @author Paul Bourelly 
     */ 
    class json_parsing_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~json_parsing_exception() = default;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit json_parsing_exception(const std::string &msg ) : std::runtime_error(msg){};

    };
}