#ifndef BDN_PropertyNotifier_H_
#define BDN_PropertyNotifier_H_

#include <bdn/DummyMutex.h>
#include <bdn/IPropertyNotifier.h>
#include <bdn/NotifierBase.h>

namespace bdn
{

    /** The default implementation for IPropertyNotifier (see \ref
       IPropertyNotifier decription for more info).
    */
    template <typename PROPERTY_VALUE_TYPE>
    class PropertyNotifier
        : public NotifierBase<DummyMutex, const PROPERTY_VALUE_TYPE &>
        // note that we use DummyMutex (i.e. no actual mutex operations will
        // happen)
        ,
          BDN_IMPLEMENTS IPropertyNotifier<PROPERTY_VALUE_TYPE>
    {
      private:
        using BASE = NotifierBase<DummyMutex, const PROPERTY_VALUE_TYPE &>;

      public:
        PropertyNotifier() {}

        ~PropertyNotifier() {}

        void notify(const IPropertyReadAccessor<PROPERTY_VALUE_TYPE>
                        &propertyAccessor) override
        {
            BASE::template notifyImpl<
                decltype(&PropertyNotifier::callPropertySubscriber),
                const IPropertyReadAccessor<PROPERTY_VALUE_TYPE> &>(
                &PropertyNotifier::callPropertySubscriber, propertyAccessor);
        }

      private:
        /** Makes the notification call to a single subscriber.
            Call maker ensures that the current value of a property is provided
           to subscribers even if a property is set recursively from within a
           subscriber method.*/
        static void callPropertySubscriber(
            const std::function<void(const PROPERTY_VALUE_TYPE &)>
                &subscribedFunc,
            const IPropertyReadAccessor<PROPERTY_VALUE_TYPE> &propertyAccessor)
        {
            subscribedFunc(propertyAccessor.get());
        }
    };
}

#endif
