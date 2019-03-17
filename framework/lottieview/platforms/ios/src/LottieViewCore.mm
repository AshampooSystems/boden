#include <bdn/LottieView.h>

#import <bdn/ios/LottieViewCore.hh>

#include <LOTAnimationView.h>

#include <bdn/UIUtil.h>
#import <bdn/foundationkit/stringUtil.hh>
#include <bdn/log.h>

namespace bdn::lottieview::detail
{
    CORE_REGISTER(LottieView, bdn::ios::LottieViewCore, LottieView)
}

@interface BodenLottieViewContainer : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
- (void)updateGeometry;
@end

@implementation BodenLottieViewContainer
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    [self updateGeometry];
}

- (void)updateGeometry
{
    if (auto core = self.viewCore.lock()) {
        if (auto lottieCore = std::dynamic_pointer_cast<bdn::ios::LottieViewCore>(core)) {
            bdn::Rect r = bdn::Rect(bdn::Point(0, 0), bdn::Size(self.frame.size.width, self.frame.size.height));

            for (UIView *subview in self.subviews) {
                [subview setFrame:CGRectMake(r.x, r.y, r.width, r.height)];
            }
        }
    }
}
@end

namespace bdn::ios
{
    BodenLottieViewContainer *createView() { return [[BodenLottieViewContainer alloc] initWithFrame:CGRectZero]; }

    LottieViewCore::LottieViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createView())
    {}

    void LottieViewCore::init()
    {
        ViewCore::init();
        running.onChange() += [=](auto va) {
            if (va->get()) {
                play();
            } else {
                [animationView pause];
            }
        };
        loop.onChange() += [=](auto va) { animationView.loopAnimation = va->get(); };
    }

    void LottieViewCore::loadURL(const String &url)
    {
        for (UIView *subview in uiView().subviews) {
            [subview removeFromSuperview];
        }

        if (cpp20::starts_with(url, "file://")) {
            NSURL *nsurl = [NSURL URLWithString:fk::stringToNSString(url)];
            NSString *localPath = nsurl.relativePath;
            animationView = [LOTAnimationView animationWithFilePath:localPath];
        } else {
            NSURL *nsurl = [NSURL URLWithString:fk::stringToNSString(url)];
            animationView = [[LOTAnimationView alloc] initWithContentsOfURL:nsurl];
        }

        [uiView() addSubview:animationView];

        if (running.get()) {
            play();
        }
        if (loop.get()) {
            animationView.loopAnimation = YES;
        }

        scheduleLayout();
    }

    void LottieViewCore::play()
    {
        [animationView playWithCompletion:^(BOOL completed) {
          running = false;
        }];
    }
}
