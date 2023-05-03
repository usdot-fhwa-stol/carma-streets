
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
        SPDLOG_WARN("Recreating Singleton of type {0}!", typeid(instance).name());
        // Remove old instance
        
        delete instance;
      
      }
      instance = new T(args...);
      return *instance;
    }
        

    /**
     * Protected constructor
     */ 
    template <typename T,typename... Args>   
    streets_singleton<T,Args...>::streets_singleton(){
      SPDLOG_INFO("Creating Singleton of type {0}!", typeid(instance).name());
    };
    /**
     * Protected destructor
     */ 
    template <typename T, typename... Args>   
    streets_singleton<T,Args...>::~streets_singleton() {
      SPDLOG_WARN("Deleting Singleton of type {0}!", typeid(instance).name());
      // Zero out first to avoid recursive call to desctructor
      // https://www.codeproject.com/Articles/987473/Unit-Testing-a-Singleton-in-Cplusplus
      instance = 0;
      delete instance;
    }; 
}
