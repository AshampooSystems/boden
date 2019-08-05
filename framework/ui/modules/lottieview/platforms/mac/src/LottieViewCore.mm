#include <LOTAnimationView.h>

#include <bdn/ui/lottie/View.h>

#import <WebKit/WebKit.h>
#import <bdn/mac/LottieViewCore.hh>
#import <bdn/mac/util.hh>

#include <bdn/Application.h>
#include <bdn/log.h>
#include <bdn/mac/util.hh>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui::lottie::detail
{
    CORE_REGISTER(View, bdn::ui::mac::LottieViewCore, View)
}

namespace bdn::ui::mac
{
    NSView *createView() { return [[NSView alloc] initWithFrame:CGRectZero]; }

    LottieViewCore::LottieViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
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
        geometry.onChange() += [=](auto &property) { updateGeometry(); };
        running.onChange() += [=](auto &property) {
            if (property.get()) {
                play();
            } else {
                [animationView pause];
            }
        };
        loop.onChange() += [=](auto &property) { animationView.loopAnimation = property.get(); };
    }

    void LottieViewCore::loadURL(const std::string &url)
    {
        for (NSView *oldView in nsView().subviews) {
            [oldView removeFromSuperview];
        }

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
