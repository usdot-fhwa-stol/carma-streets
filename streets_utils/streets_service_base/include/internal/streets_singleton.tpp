#ifdef streets_singleton_H

namespace streets_service {  
  // Implementation

    template <typename T>   
    T& streets_singleton<T>::get_singleton() {
        static T instance;  // Guaranteed to be destroyed.
                            // Instantiated on first use.
        char strAddress[] = "0x00000000";
        snprintf(strAddress,sizeof(strAddress) ,"0x%x", &instance);
        spdlog::debug("Singleton {0} memory address : {1}", typeid(instance).name() , strAddress);
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
#endif