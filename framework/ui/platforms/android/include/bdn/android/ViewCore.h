#pragma once

#include <bdn/Dip.h>
#include <bdn/View.h>
#include <bdn/ViewCore.h>

#include <bdn/java/wrapper/NativeStrongPointer.h>
#include <bdn/java/wrapper/NativeWeakPointer.h>

#include <bdn/android/IParentViewCore.h>
#include <bdn/android/wrapper/NativeViewCoreLayoutChangeListener.h>
#include <bdn/android/wrapper/NativeViewGroup.h>
#include <bdn/android/wrapper/View.h>

#include <bdn/log.h>
#include <cstdlib>

using namespace std::string_literals;

namespace bdn::android
{
    class UIProvider;

    class ViewCore : virtual public bdn::ViewCore
    {
        friend class bdn::android::UIProvider;

        void init();

      public:
        ViewCore(std::shared_ptr<View> outer, wrapper::View jView) : _outerView(outer), _jView(jView) { init(); }
        virtual ~ViewCore();

      public:
        std::shared_ptr<View> outerView() const;

        virtual void initTag();

        wrapper::View &getJView() { return _jView; }
        template <class JSuperType> JSuperType getJViewAS() { return _jView.cast<JSuperType>(); }

        std::shared_ptr<ViewCore> getParentViewCore();

        Size sizeForSpace(Size availableSpace = Size::none()) const override;

        bool canMoveToParentView(std::shared_ptr<View> newParentView) const override;

        void moveToParentView(std::shared_ptr<View> newParentView) override;

        virtual void dispose() override;

        virtual void clicked() {}

        virtual void layoutChange(int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight,
                                  int oldBottom);

        double getUIScaleFactor() const;
        void setUIScaleFactor(double scaleFactor);

        void scheduleLayout() override;

      protected:
        /** Returns true if the view can adjust its width to fit into
                    a certain size of available space.

                    If this returns false then sizeForSpace will ignore the
                    availableWidth parameter.

                    The default implementation returns false.
                */
        virtual bool canAdjustWidthToAvailableSpace() const { return false; }

        /** Returns true if the view can adjust its height to fit into
                    a certain size of available space.

                    If this returns false then sizeForSpace will ignore the
                    availableHeight parameter.

                    The default implementation returns false.
                */
        virtual bool canAdjustHeightToAvailableSpace() const { return false; }

        virtual double getFontSizeDips() const;

        double getEmSizeDips() const;
        double getSemSizeDips() const;

      private:
        void _addToParent(std::shared_ptr<View> parent);
        void _removeFromParent();

      private:
        std::weak_ptr<View> _outerView;
        mutable wrapper::View _jView;
        double _uiScaleFactor;

        mutable double _emDipsIfInitialized = -1;
        mutable double _semDipsIfInitialized = -1;
    };

    std::shared_ptr<ViewCore> viewCoreFromJavaViewRef(const bdn::java::Reference &javaViewRef);

    template <class T>
    wrapper::View createAndroidViewClass(std::shared_ptr<View> outer,
                                         std::optional<wrapper::Context> context = std::nullopt)
    {
        if (!context) {
            std::shared_ptr<View> parent = outer->getParentView();
            if (parent == nullptr) {
                throw ProgrammingError("ViewCore instance requested for a "s + typeid(T).name() +
                                       " that does not have a parent and no context was given."s);
            }

            std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->viewCore());

            if (parentCore == nullptr) {
                throw ProgrammingError("ViewCore instance requested for a "s + typeid(T).name() +
                                       " with core-less parent."s);
            }

            T view(parentCore->getJView().getContext());
            return wrapper::View(view.getRef_());
        }

        T view(*context);
        return wrapper::View(view.getRef_());
    }
}
