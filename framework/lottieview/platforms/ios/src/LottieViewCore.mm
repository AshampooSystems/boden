#include <bdn/LottieView.h>

#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/LottieViewCore.hh>

#include <LOTAnimationView.h>

#include <bdn/Application.h>
#include <bdn/ViewUtilities.h>
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

    LottieViewCore::LottieViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createView())
    {}

    LottieViewCore::~LottieViewCore()
    {
        [animationView stop];
        animationView = nullptr;
    }

    void LottieViewCore::init()
    {
        ViewCore::init();
        running.onChange() += [=](auto &property) {
            if (property.get()) {
                play();
            } else {
                [animationView pause];
            }
        };
        loop.onChange() += [=](auto &property) { animationView.loopAnimation = property.get(); };
    }

    void LottieViewCore::loadURL(const String &url)
    {
        [[uiView() subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];

        auto uri = App()->uriToBundledFileUri(url);

        if (uri.empty()) {
            uri = url;
        }

        if (cpp20::starts_with(uri, "file:///")) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(uri)]) {
                if (auto localPath = nsURL.relativePath) {
                    animationView = [LOTAnimationView animationWithFilePath:localPath];
                }
            }
        } else {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(uri)]) {
                animationView = [[LOTAnimationView alloc] initWithContentsOfURL:nsURL];
            }
        }

        if (animationView != nullptr) {
            [uiView() addSubview:animationView];

            if (running.get()) {
                play();
            }
            if (loop.get()) {
                animationView.loopAnimation = YES;
            }
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
