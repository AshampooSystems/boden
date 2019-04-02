#pragma once

#include <bdn/Label.h>

#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/util.hh>

/** NSTextView subclass that adds some additional capabilities.

    For example, non-symmetrical text-insets can be set.
 */
@interface BdnMacTextView_ : NSTextView

/** A displacement value that is added to textContainerOrigin.
    This can be used to create non-symmetrical insets / padding.*/
@property NSSize textContainerDisplacement;

@end

@implementation BdnMacTextView_

- (NSPoint)textContainerOrigin
{
    NSPoint origin = [super textContainerOrigin];
    NSPoint displacedOrigin =
        NSMakePoint(origin.x + _textContainerDisplacement.width, origin.y + _textContainerDisplacement.height);

    return displacedOrigin;
}

@end

namespace bdn::mac
{
    class LabelCore : public ViewCore, virtual public bdn::Label::Core
    {
      private:
        static BdnMacTextView_ *_createNSTextView();

      public:
        LabelCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        Size sizeForSpace(Size availableSpace) const override;

      private:
        BdnMacTextView_ *_nsTextView;
        bool _wrap;
    };
}
