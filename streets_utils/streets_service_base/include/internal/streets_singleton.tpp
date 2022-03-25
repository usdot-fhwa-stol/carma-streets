
namespace streets_service {  
  // Implementation

    template <typename T>   
    T& streets_singleton<T>::get_singleton() {
        static T instance;  // Guaranteed to be destroyed.
                            // Instantiated on first use.
        char strAddress[] = "0x00000000";
        snprintf(strAddress,sizeof(strAddress) ,"0x%x", std::addressof(instance));
        spdlog::debug("Singleton class : {0}.", typeid(instance).name() );
        spdlog::debug("Singleton address: {0}", strAddress);
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
