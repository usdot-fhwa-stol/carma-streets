#pragma once

#include "spdlog/spdlog.h"

/**
 * CARMA-Streets singleton templated class meant for extending
 * to allow for singleton scoped objects in CARMA-Streets services
 * and static access to the objects
 * 
 * @author Paul Bourelly
 * @date 3/14/2022
 */
template <typename T>
class streets_singleton
{
    public:
        /**
         * Method for static singleton instance retrieval
         * @returns T singleton instance
         */ 
        static T& get_singleton() ;
        // Remove copy constructor 
        streets_singleton(streets_singleton const &) = delete;
        // Remove move constructor
        streets_singleton& operator=(streets_singleton const &) = delete;
    protected:
        /**
         * Protected constructor
         */ 
        streets_singleton();
        /**
         * Protected destructor
         */        
        ~streets_singleton();        
};

// Implementation

template <typename T>   
T& streets_singleton<T>::get_singleton() {
    static T instance;  // Guaranteed to be destroyed.
                        // Instantiated on first use.
    return instance;
};
    

/**
 * Protected constructor
 */ 
template <typename T>   
streets_singleton<T>::streets_singleton() {};
/**
 * Protected destructor
 */ 
template <typename T>   
streets_singleton<T>::~streets_singleton() {};        

