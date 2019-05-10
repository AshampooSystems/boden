#include <bdn/mac/SliderCore.hh>

namespace bdn::ui::detail
{
    CORE_REGISTER(Slider, bdn::ui::mac::SliderCore, Slider)
}

@interface BdnSliderHandler : NSObject
@property(nonatomic, assign) std::weak_ptr<bdn::ui::mac::SliderCore> sliderCore;
@end

@implementation BdnSliderHandler
- (void)changed
{
    if (auto core = self.sliderCore.lock()) {
        core->valueChanged();
    }
}
@end

namespace bdn::ui::mac
{
    NSSlider *createNSSlider()
    {
        NSSlider *slider = [[NSSlider alloc] init];

        return slider;
    }

    SliderCore::SliderCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : mac::ViewCore(viewCoreFactory, createNSSlider())
    {
        _nsSlider = (NSSlider *)nsView();
    }

    void SliderCore::init()
    {
        _sliderHandler = [[BdnSliderHandler alloc] init];
        _sliderHandler.sliderCore = shared_from_this<SliderCore>();

        [_nsSlider setTarget:_sliderHandler];
        [_nsSlider setAction:@selector(changed)];

        value.onChange() += [slider = _nsSlider](const auto &p) { slider.doubleValue = p.get(); };
    }

    void SliderCore::valueChanged() { value = _nsSlider.doubleValue; }
}
