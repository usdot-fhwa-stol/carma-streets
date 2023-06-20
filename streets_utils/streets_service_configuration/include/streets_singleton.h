#pragma once
#include <spdlog/spdlog.h>
#include <typeinfo>
#include "streets_singleton_exception.h"


namespace streets_service {
    /**
     * CARMA-Streets singleton templated class meant for extending
     * to allow for singleton scoped objects in CARMA-Streets services
     * and static access to the objects
     * 
     * @author Paul Bourelly
     * @date 3/14/2022
     */
    template <typename T, typename... Args>
    class streets_singleton
    {
        public:
            /**
             * Method for static singleton instance retrieval
             * @returns T singleton instance
             */ 
            static T& get_singleton() ;
            // Remove copy constructor 
            streets_singleton(const streets_singleton &) = delete;
            // Remove move constructor
            streets_singleton(streets_singleton&& ) = delete;
            // Remove copy assignment operator
            streets_singleton& operator=(const streets_singleton &) = delete;
            // Remove move assignment operator
            streets_singleton& operator=(const streets_singleton &&) = delete;
            /**
             * @brief Method to intialize streets_singleton with constructor params
             * ...Args of templated class.
             * 
             * @param ...args constructor parameters of templated class.
             * @return returns reference to singleton instance.
             */
            static T& create(Args...args );


        protected:
            /**
             * Protected constructor
             */ 
            streets_singleton() = default;
            /**
             * Protected destructor
             */        
            ~streets_singleton() = default;
              
            static std::unique_ptr<T> instance;        
    };

    template <typename T, typename ...Args>
    std::unique_ptr<T> streets_singleton<T, Args...>::instance = nullptr;
         
};

#include "internal/streets_singleton.tpp"
