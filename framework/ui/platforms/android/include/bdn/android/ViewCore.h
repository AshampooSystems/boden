#pragma once

#include <bdn/View.h>
#include <bdn/ViewCore.h>

#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

#include <bdn/android/wrapper/NativeViewCoreLayoutChangeListener.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

#include <bdn/UIContext.h>
#include <bdn/ViewCoreFactory.h>
#include <bdn/android/ContextWrapper.h>

#include <utility>

using namespace std::string_literals;

namespace bdn::android
{
    class ViewCore : public bdn::ViewCore
    {
        friend class bdn::ViewCoreFactory;

      public:
        ViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory, wrapper::View jView)
            : bdn::ViewCore(viewCoreFactory), _jView(std::move(std::move(jView)))
        {}
        ~ViewCore() override;

      public:
        wrapper::View &getJView() { return _jView; }
        template <class JSuperType> JSuperType getJViewAS() { return _jView.cast<JSuperType>(); }

        Size sizeForSpace(Size availableSpace = Size::none()) const override;

        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

        virtual void clicked() {}

        virtual void layoutChange(int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight,
                                  int oldBottom);

        virtual double getUIScaleFactor() const;
        virtual void setUIScaleFactor(double scaleFactor);

        void scheduleLayout() override;

        void updateChildren();

      protected:
        void init() override;
        virtual void initTag();

        virtual bool canAdjustWidthToAvailableSpace() const { return false; }
        virtual bool canAdjustHeightToAvailableSpace() const { return false; }

        virtual void updateGeometry();

      private:
        mutable wrapper::View _jView;
        double _uiScaleFactor{};

        mutable double _emDipsIfInitialized = -1;
        mutable double _semDipsIfInitialized = -1;
    };

    std::shared_ptr<ViewCore> viewCoreFromJavaViewRef(const bdn::java::Reference &javaViewRef);

    template <class T> std::shared_ptr<T> viewCoreFromJavaReference(const bdn::java::Reference &javaViewRef)
    {
        return std::dynamic_pointer_cast<T>(viewCoreFromJavaViewRef(javaViewRef));
    }

    template <class T>
    wrapper::View createAndroidViewClass(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
    {
        T view(viewCoreFactory->getContextStackTop<bdn::android::UIContext>()->_contextWrapper->getContext());
        return wrapper::View(view.getRef_());
    }
}
