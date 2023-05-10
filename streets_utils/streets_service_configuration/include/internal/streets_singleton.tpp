
namespace streets_service {  
  // Implementation

    template <typename T,typename... Args>   
    T& streets_singleton<T,Args...>::get_singleton() {
        if ( !instance ) {
          throw streets_singleton_exception("Singleton has not been created");
        } 
        char strAddress[20];
        snprintf(strAddress,sizeof(strAddress) ,"%p",std::addressof(instance) );
        SPDLOG_TRACE("Singleton class : {0}.", typeid(instance.get()).name() );
        SPDLOG_TRACE("Singleton address: {0}", strAddress);
        return *instance;
    };

    template <typename T, typename... Args>
    T& streets_singleton<T,Args...>::create(Args...args ){
      if (instance != nullptr){
        SPDLOG_WARN("Recreating Singleton of type {0}!", typeid(instance.get()).name());
        // Reset unique ptr
        
        instance.reset( new T(args...) );
      
      }
      else {
        SPDLOG_INFO("Initializing Singleton of type {0}!", typeid(instance.get()).name());
        instance = std::unique_ptr<T>( new T(args...) );
      }
      
      return *instance;
    }
        

}
