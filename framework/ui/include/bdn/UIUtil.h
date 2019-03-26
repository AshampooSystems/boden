#pragma once

#include <bdn/ViewCoreFactory.h>

#define VIEW_CORE_REGISTRY_DECLARATION_STATIC(NAME)                                                                    \
    void NAME##_registerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

#define VIEW_CORE_REGISTRY_DECLARATION_SHARED(NAME)                                                                    \
    class NAME##_RegisterMe                                                                                            \
    {                                                                                                                  \
      public:                                                                                                          \
        NAME##_RegisterMe(std::function<void(std::shared_ptr<ViewCoreFactory>)> callMe);                               \
    };                                                                                                                 \
    class NAME##_Registrar                                                                                             \
    {                                                                                                                  \
        friend class NAME##_RegisterMe;                                                                                \
                                                                                                                       \
      public:                                                                                                          \
        static NAME##_Registrar &get();                                                                                \
        void call(std::shared_ptr<ViewCoreFactory> viewCoreFactory);                                                   \
                                                                                                                       \
      protected:                                                                                                       \
        void setRegisterMe(std::function<void(std::shared_ptr<ViewCoreFactory>)> function);                            \
                                                                                                                       \
      private:                                                                                                         \
        std::function<void(std::shared_ptr<ViewCoreFactory>)> _registerFunction;                                       \
    };

#define VIEW_CORE_REGISTRY_IMPLEMENTATION_SHARED(NAME)                                                                 \
    NAME##_RegisterMe::NAME##_RegisterMe(std::function<void(std::shared_ptr<ViewCoreFactory>)> callMe)                 \
    {                                                                                                                  \
        NAME##_Registrar::get().setRegisterMe(std::move(callMe));                                                      \
    }                                                                                                                  \
    NAME##_Registrar &NAME##_Registrar::get()                                                                          \
    {                                                                                                                  \
        static NAME##_Registrar s_registrar{};                                                                         \
        return s_registrar;                                                                                            \
    }                                                                                                                  \
    void NAME##_Registrar::call(std::shared_ptr<ViewCoreFactory> viewCoreFactory)                                      \
    {                                                                                                                  \
        if (_registerFunction) {                                                                                       \
            _registerFunction(std::move(viewCoreFactory));                                                             \
        } else {                                                                                                       \
            throw std::runtime_error("Nothing registered!");                                                           \
        }                                                                                                              \
    }                                                                                                                  \
    void NAME##_Registrar::setRegisterMe(std::function<void(std::shared_ptr<ViewCoreFactory>)> function)               \
    {                                                                                                                  \
        _registerFunction = std::move(function);                                                                       \
    }

#define VIEW_CORE_REGISTER_STATIC(NAME, VIEWCORE_FACTORY) NAME##_registerViewCore(VIEWCORE_FACTORY);
#define VIEW_CORE_REGISTER_SHARED(NAME, VIEWCORE_FACTORY) NAME##_Registrar::get().call(VIEWCORE_FACTORY);

#define CORE_REGISTER_STATIC(NAME, CORETYPE, VIEWTYPE)                                                                 \
    void NAME##_registerViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)                              \
    {                                                                                                                  \
        viewCoreFactory->registerCoreType<CORETYPE, VIEWTYPE>();                                                       \
    }

#define CORE_REGISTER_SHARED(NAME, CORETYPE, VIEWTYPE)                                                                 \
    CORE_REGISTER_STATIC(NAME, CORETYPE, VIEWTYPE)                                                                     \
    static NAME##_RegisterMe registerMe = {&NAME##_registerViewCore};

#if BDN_SHARED_LIB == 1
#define VIEW_CORE_REGISTRY_DECLARATION(NAME) VIEW_CORE_REGISTRY_DECLARATION_SHARED(NAME)
#define VIEW_CORE_REGISTRY_IMPLEMENTATION(NAME) VIEW_CORE_REGISTRY_IMPLEMENTATION_SHARED(NAME)
#define VIEW_CORE_REGISTER(NAME, VIEWCORE_FACTORY) VIEW_CORE_REGISTER_SHARED(NAME, VIEWCORE_FACTORY)
#define CORE_REGISTER(NAME, CORETYPE, VIEWTYPE) CORE_REGISTER_SHARED(NAME, CORETYPE, VIEWTYPE)
#else
#define VIEW_CORE_REGISTRY_DECLARATION(NAME) VIEW_CORE_REGISTRY_DECLARATION_STATIC(NAME)
#define VIEW_CORE_REGISTRY_IMPLEMENTATION(NAME)
#define VIEW_CORE_REGISTER(NAME, VIEWCORE_FACTORY) VIEW_CORE_REGISTER_STATIC(NAME, VIEWCORE_FACTORY)
#define CORE_REGISTER(NAME, CORETYPE, VIEWTYPE) CORE_REGISTER_STATIC(NAME, CORETYPE, VIEWTYPE)
#endif
