#pragma once

#include <bdn/ui/View.h>

#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

#include <bdn/android/wrapper/NativeViewCoreLayoutChangeListener.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

#include <bdn/Context.h>
#include <bdn/android/ContextWrapper.h>
#include <bdn/ui/ViewCoreFactory.h>

#include <utility>

using namespace std::string_literals;

namespace bdn::ui::android
{
    class ViewCore : public View::Core, public std::enable_shared_from_this<ViewCore>
    {
        friend class ViewCoreFactory;

      public:
        ViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, bdn::android::wrapper::View jView)
            : View::Core(viewCoreFactory), _jView(std::move(std::move(jView)))
        {}
        ~ViewCore() override;

        void init() override;

      public:
        bdn::android::wrapper::View &getJView() { return _jView; }
        template <class JSuperType> JSuperType getJViewAS() { return _jView.cast<JSuperType>(); }

        Size sizeForSpace(Size availableSpace = Size::none()) const override;

        virtual void clicked() {}

        virtual void layoutChange(int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight,
                                  int oldBottom);

        virtual double getUIScaleFactor() const;
        virtual void setUIScaleFactor(double scaleFactor);

        void scheduleLayout() override;

        void updateChildren();

        float baseline(Size forSize) const override;
        float pointScaleFactor() const override;

      protected:
        virtual void initTag();

        virtual bool canAdjustWidthToAvailableSpace() const { return false; }
        virtual bool canAdjustHeightToAvailableSpace() const { return false; }

        virtual void updateGeometry();

      private:
        mutable bdn::android::wrapper::View _jView;
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
    bdn::android::wrapper::View createAndroidViewClass(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
    {
        T view(viewCoreFactory->getContextStackTop<bdn::android::Context>()->_contextWrapper->getContext());
        return bdn::android::wrapper::View(view.getRef_());
    }
}
