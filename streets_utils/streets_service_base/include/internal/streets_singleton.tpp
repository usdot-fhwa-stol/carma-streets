#ifdef streets_singleton_h

namespace streets_service {  
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
    streets_singleton<T>::streets_singleton() = default;
    /**
     * Protected destructor
     */ 
    template <typename T>   
    streets_singleton<T>::~streets_singleton() = default; 
}
#endif