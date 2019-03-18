#pragma once

#include <bdn/UIProvider.h>

#define VIEW_CORE_REGISTRY_DECLARATION_STATIC(NAME)                                                                    \
    void NAME##registerViewCore(const std::shared_ptr<UIProvider> &uiProvider);

#define VIEW_CORE_REGISTRY_DECLARATION_SHARED(NAME)                                                                    \
    class NAME##RegisterMe                                                                                             \
    {                                                                                                                  \
      public:                                                                                                          \
        NAME##RegisterMe(std::function<void(std::shared_ptr<UIProvider>)> callMe);                                     \
    };                                                                                                                 \
    class NAME##Registrar                                                                                              \
    {                                                                                                                  \
        friend class NAME##RegisterMe;                                                                                 \
                                                                                                                       \
      public:                                                                                                          \
        static NAME##Registrar &get();                                                                                 \
        void call(std::shared_ptr<UIProvider> uiProvider);                                                             \
                                                                                                                       \
      protected:                                                                                                       \
        void setRegisterMe(std::function<void(std::shared_ptr<UIProvider>)> function);                                 \
                                                                                                                       \
      private:                                                                                                         \
        std::function<void(std::shared_ptr<UIProvider>)> _registerFunction;                                            \
    };

#define VIEW_CORE_REGISTRY_IMPLEMENTATION_SHARED(NAME)                                                                 \
    NAME##RegisterMe::NAME##RegisterMe(std::function<void(std::shared_ptr<UIProvider>)> callMe)                        \
    {                                                                                                                  \
        NAME##Registrar::get().setRegisterMe(std::move(callMe));                                                       \
    }                                                                                                                  \
    NAME##Registrar &NAME##Registrar::get()                                                                            \
    {                                                                                                                  \
        static NAME##Registrar s_registrar{};                                                                          \
        return s_registrar;                                                                                            \
    }                                                                                                                  \
    void NAME##Registrar::call(std::shared_ptr<UIProvider> uiProvider)                                                 \
    {                                                                                                                  \
        if (_registerFunction) {                                                                                       \
            _registerFunction(std::move(uiProvider));                                                                  \
        } else {                                                                                                       \
            throw std::runtime_error("Nothing registered!");                                                           \
        }                                                                                                              \
    }                                                                                                                  \
    void NAME##Registrar::setRegisterMe(std::function<void(std::shared_ptr<UIProvider>)> function)                     \
    {                                                                                                                  \
        _registerFunction = std::move(function);                                                                       \
    }

#define VIEW_CORE_REGISTER_STATIC(NAME, UIPROVIDER) NAME##registerViewCore(UIPROVIDER);
#define VIEW_CORE_REGISTER_SHARED(NAME, UIPROVIDER) NAME##Registrar::get().call(UIPROVIDER);

#define CORE_REGISTER_STATIC(NAME, CORETYPE, VIEWTYPE)                                                                 \
    void NAME##registerViewCore(const std::shared_ptr<UIProvider> &uiProvider)                                         \
    {                                                                                                                  \
        uiProvider->registerCoreType<CORETYPE, VIEWTYPE>();                                                            \
    }

#define CORE_REGISTER_SHARED(NAME, CORETYPE, VIEWTYPE)                                                                 \
    CORE_REGISTER_STATIC(NAME, CORETYPE, VIEWTYPE)                                                                     \
    static NAME##RegisterMe registerMe = {&NAME##registerViewCore};

#if BDN_SHARED_LIB == 1
#define VIEW_CORE_REGISTRY_DECLARATION(NAME) VIEW_CORE_REGISTRY_DECLARATION_SHARED(NAME)
#define VIEW_CORE_REGISTRY_IMPLEMENTATION(NAME) VIEW_CORE_REGISTRY_IMPLEMENTATION_SHARED(NAME)
#define VIEW_CORE_REGISTER(NAME, UIPROVIDER) VIEW_CORE_REGISTER_SHARED(NAME, UIPROVIDER)
#define CORE_REGISTER(NAME, CORETYPE, VIEWTYPE) CORE_REGISTER_SHARED(NAME, CORETYPE, VIEWTYPE)
#else
#define VIEW_CORE_REGISTRY_DECLARATION(NAME) VIEW_CORE_REGISTRY_DECLARATION_STATIC(NAME)
#define VIEW_CORE_REGISTRY_IMPLEMENTATION(NAME)
#define VIEW_CORE_REGISTER(NAME, UIPROVIDER) VIEW_CORE_REGISTER_STATIC(NAME, UIPROVIDER)
#define CORE_REGISTER(NAME, CORETYPE, VIEWTYPE) CORE_REGISTER_STATIC(NAME, CORETYPE, VIEWTYPE)
#endif
