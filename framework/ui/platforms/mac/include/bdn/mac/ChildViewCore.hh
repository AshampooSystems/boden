#pragma once

#include <Cocoa/Cocoa.h>
#include <bdn/ProgrammingError.h>
#include <bdn/ViewCore.h>
#include <bdn/mac/IParentViewCore.h>

#import <bdn/mac/UIProvider.hh>
#import <bdn/mac/util.hh>

@protocol BdnLayoutable
@end

namespace bdn
{
    namespace mac
    {

        class ChildViewCore : virtual public ViewCore, virtual public IParentViewCore
        {
          public:
            ChildViewCore(std::shared_ptr<View> outerView, NSView *nsView);
            virtual ~ChildViewCore();

          public:
            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override { return true; }

            void moveToParentView(std::shared_ptr<View> newParentView) override;

            void dispose() override;

            std::shared_ptr<View> getOuterViewIfStillAttached() const;

            NSView *getNSView() const;

            void addChildNsView(NSView *childView) override;

            void removeFromNsSuperview();

            void frameChanged();

            virtual void scheduleLayout() override;

            virtual Size sizeForSpace(Size availableSpace) const override;

          protected:
            virtual double getFontSize() const
            {
                // most views do not have a font size attached to them in cocoa.
                // Those should override this function.
                // In the default implementation we simply return the system
                // font size.
                return getSemSizeDips();
            }

            double getEmSizeDips() const
            {
                if (_emDipsIfInitialized == -1)
                    _emDipsIfInitialized = getFontSize();

                return _emDipsIfInitialized;
            }

            double getSemSizeDips() const
            {
                if (_semDipsIfInitialized == -1)
                    _semDipsIfInitialized = UIProvider::get()->getSemSizeDips();

                return _semDipsIfInitialized;
            }

          private:
            void _addToParent(std::shared_ptr<View> parentView)
            {
                if (parentView == nullptr) {
                    // classes derived from ChildViewCore MUST have a parent.
                    // Top level windows do not derive from ChildViewCore.
                    throw ProgrammingError("bdn::mac::ChildViewCore constructed for a view that "
                                           "does not have a parent.");
                }

                std::shared_ptr<ViewCore> parentCore = parentView->getViewCore();
                if (parentCore == nullptr) {
                    // this should not happen. The parent MUST have a core -
                    // otherwise we cannot initialize ourselves.
                    throw ProgrammingError("bdn::mac::ChildViewCore constructed for a view whose "
                                           "parent does not have a core.");
                }

                std::dynamic_pointer_cast<IParentViewCore>(parentCore)->addChildNsView(_nsView);
            }

            std::weak_ptr<View> _outerViewWeak;
            NSView *_nsView;

            mutable double _emDipsIfInitialized = -1;
            mutable double _semDipsIfInitialized = -1;

            NSObject *_eventForwarder;
        };
    }
}
