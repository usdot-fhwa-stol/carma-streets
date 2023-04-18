
namespace streets_service {  
  // Implementation

    template <typename T,typename... Args>   
    T& streets_singleton<T,Args...>::get_singleton() {
        if ( !instance ) {
          throw streets_singleton_exception("Singleton has not been created");
        } // Guaranteed to be destroyed.
                            // Instantiated on first use.
        char strAddress[20];
        snprintf(strAddress,sizeof(strAddress) ,"%p",std::addressof(instance) );
        SPDLOG_TRACE("Singleton class : {0}.", typeid(instance).name() );
        SPDLOG_TRACE("Singleton address: {0}", strAddress);
        return *instance;
    };

    template <typename T, typename... Args>
    T& streets_singleton<T,Args...>::create(Args...args ){
      if (instance != nullptr){
        throw streets_singleton_exception("Singleton has already been created!");
      }
      instance = new T(args...);
      return *instance;
    }
        

    /**
     * Protected constructor
     */ 
    template <typename T,typename... Args>   
    streets_singleton<T,Args...>::streets_singleton() = default;
    /**
     * Protected destructor
     */ 
    template <typename T, typename... Args>   
    streets_singleton<T,Args...>::~streets_singleton() {
      delete instance;
    }; 
}
