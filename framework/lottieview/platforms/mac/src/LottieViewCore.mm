#include <bdn/LottieView.h>

#import <bdn/mac/LottieViewCore.hh>
#import <bdn/mac/util.hh>

#include <LOTAnimationView.h>

#import <WebKit/WebKit.h>
#include <bdn/mac/util.hh>

#include <bdn/log.h>

#include <bdn/ViewUtilities.h>

namespace bdn::lottieview::detail
{
    CORE_REGISTER(LottieView, bdn::mac::LottieViewCore, LottieView)
}

namespace bdn::mac
{
    NSView *createView() { return [[NSView alloc] initWithFrame:CGRectZero]; }

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
        geometry.onChange() += [=](auto va) { updateGeometry(); };
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
        for (NSView *oldView in nsView().subviews) {
            [oldView removeFromSuperview];
        }

        if (cpp20::starts_with(url, "file://")) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(url)]) {
                if (auto localPath = nsURL.relativePath) {
                    animationView = [LOTAnimationView animationWithFilePath:localPath];
                }
            }
        } else if (cpp20::starts_with(url, "resource://")) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(url)]) {
                auto server = nsURL.host;
                NSBundle *bundle = [NSBundle mainBundle];
                if (server && [server compare:@"main"] != NSOrderedSame) {
                    bundle = [NSBundle bundleWithIdentifier:server];
                }
                if (bundle) {
                    if (auto localPath = [nsURL.relativePath substringFromIndex:1]) {
                        NSRange lastDot = [localPath rangeOfString:@"." options:NSBackwardsSearch];
                        NSString *ext = [localPath substringFromIndex:lastDot.location];

                        NSString *resourceName = [localPath stringByReplacingOccurrencesOfString:@"/" withString:@"_"];
                        resourceName = [resourceName stringByReplacingOccurrencesOfString:@"." withString:@"_"];

                        resourceName = [NSString stringWithFormat:@"%@%@", resourceName, ext];

                        animationView = [LOTAnimationView animationNamed:resourceName inBundle:bundle];
                    }
                }
            }
        } else {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(url)]) {
                animationView = [[LOTAnimationView alloc] initWithContentsOfURL:nsURL];
            }
        }

        if (animationView != nullptr) {
            [nsView() addSubview:animationView];

            if (running.get()) {
                play();
            }
            if (loop.get()) {
                animationView.loopAnimation = YES;
            }
        }
    }

    void LottieViewCore::updateGeometry()
    {
        for (NSView *view in nsView().subviews) {
            Rect r = Rect(Point(0, 0), geometry->size());

            [view setFrameOrigin:CGPointMake(r.x, r.y)];
            [view setFrameSize:CGSizeMake(r.width, r.height)];
        }
    }

    void LottieViewCore::play()
    {
        std::weak_ptr<LottieViewCore> pThis = shared_from_this<LottieViewCore>();
        [animationView playWithCompletion:^(BOOL completed) {
          if (auto lottieViewCore = pThis.lock()) {
              lottieViewCore->running = false;
          }
        }];
    }
}
