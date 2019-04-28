#include <bdn/ios/SliderCore.hh>

namespace bdn::detail
{
    CORE_REGISTER(Slider, bdn::ios::SliderCore, Slider)
}

@implementation BodenUISlider
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}

- (void)sliderAction:(id)sender
{
    if (auto sliderCore = std::dynamic_pointer_cast<bdn::ios::SliderCore>(self.viewCore.lock())) {
        sliderCore->value = self.value;
    }
}
@end

namespace bdn::ios
{
    BodenUISlider *SliderCore::createUISlider()
    {
        BodenUISlider *slider = [[BodenUISlider alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

        [slider addTarget:slider action:@selector(sliderAction:) forControlEvents:UIControlEventValueChanged];

        return slider;
    }

    SliderCore::SliderCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createUISlider())
    {
        _uiSlider = (UISlider *)uiView();

        value.onChange() += [=](const auto &p) { _uiSlider.value = p.get(); };
    }

    UISlider *SliderCore::getUISlider() { return _uiSlider; }
}
