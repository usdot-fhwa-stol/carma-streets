
namespace streets_service {  
  // Implementation

    template <typename T>   
    T& streets_singleton<T>::get_singleton() {
        static T instance;  // Guaranteed to be destroyed.
                            // Instantiated on first use.
        char strAddress[20];
        snprintf(strAddress,sizeof(strAddress) ,"%p",std::addressof(instance) );
        SPDLOG_DEBUG("Singleton class : {0}.", typeid(instance).name() );
        SPDLOG_DEBUG("Singleton address: {0}", strAddress);
        return instance;
    };
        

    /**
     * Protected constructor
     */ 
    template <typename T>   
    streets_singleton<T>::streets_singleton() = default;
    /**
     * Protected destructor
     */ 
    template <typename T>   
    streets_singleton<T>::~streets_singleton() = default; 
}
