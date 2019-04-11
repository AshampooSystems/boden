#pragma once

#include <bdn/Label.h>

#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/util.hh>

namespace bdn::mac
{
    class LabelCore : public ViewCore, virtual public bdn::Label::Core
    {
      public:
        LabelCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        Size sizeForSpace(Size availableSpace) const override;

      private:
        NSTextView *_nsTextView;
        bool _wrap;
    };
}
