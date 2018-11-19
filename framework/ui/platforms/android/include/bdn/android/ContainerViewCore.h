#ifndef BDN_ANDROID_ContainerViewCore_H_
#define BDN_ANDROID_ContainerViewCore_H_

#include <bdn/ContainerView.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/IParentViewCore.h>
#include <bdn/android/JNativeViewGroup.h>

namespace bdn
{
    namespace android
    {

        class ContainerViewCore : public ViewCore, BDN_IMPLEMENTS IParentViewCore
        {
          private:
            static P<JNativeViewGroup> _createJNativeViewGroup(ContainerView *outer)
            {
                // we need to know the context to create the view.
                // If we have a parent then we can get that from the parent's
                // core.
                P<View> parent = outer->getParentView();
                if (parent == nullptr)
                    throw ProgrammingError("ContainerViewCore instance requested for a "
                                           "ContainerView that does not have a parent.");

                P<ViewCore> parentCore = cast<ViewCore>(parent->getViewCore());
                if (parentCore == nullptr)
                    throw ProgrammingError("ContainerViewCore instance requested for a "
                                           "ContainerView with core-less parent.");

                JContext context = parentCore->getJView().getContext();

                return newObj<JNativeViewGroup>(context);
            }

          public:
            ContainerViewCore(ContainerView *outer) : ViewCore(outer, _createJNativeViewGroup(outer)) {}

            Size calcPreferredSize(const Size &availableSpace) const override
            {
                // call the outer container's preferred size calculation

                P<ContainerView> outerView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr)
                    return outerView->calcContainerPreferredSize(availableSpace);
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                // call the outer container's layout function

                P<ContainerView> outerView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr) {
                    P<ViewLayout> layout = outerView->calcContainerLayout(outerView->size());
                    layout->applyTo(outerView);
                }
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                Rect adjustedBounds = ViewCore::adjustAndSetBounds(requestedBounds);

                JNativeViewGroup thisGroup(getJView().getRef_());

                double scaleFactor = getUiScaleFactor();

                thisGroup.setSize(std::lround(adjustedBounds.width * scaleFactor),
                                  std::lround(adjustedBounds.height * scaleFactor));

                return adjustedBounds;
            }

            double getUiScaleFactor() const override { return ViewCore::getUiScaleFactor(); }

            void addChildJView(JView childJView) override
            {
                JNativeViewGroup parentGroup(getJView().getRef_());

                parentGroup.addView(childJView);
            }

            void removeChildJView(JView childJView) override
            {
                JNativeViewGroup parentGroup(getJView().getRef_());
                parentGroup.removeView(childJView);
            }
        };
    }
}

#endif
