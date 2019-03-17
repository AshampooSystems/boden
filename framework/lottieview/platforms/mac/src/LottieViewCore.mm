#include <bdn/LottieView.h>

#import <bdn/mac/LottieViewCore.hh>
#import <bdn/mac/util.hh>

#include <LOTAnimationView.h>

#import <WebKit/WebKit.h>
#include <bdn/mac/util.hh>

#include <bdn/log.h>

#include <bdn/UIUtil.h>

namespace bdn::lottieview::detail
{
    CORE_REGISTER(LottieView, bdn::mac::LottieViewCore, LottieView)
}

namespace bdn::mac
{
    NSView *createView() { return [[NSView alloc] initWithFrame:CGRectZero]; }

    LottieViewCore::LottieViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createView())
    {}

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
            NSURL *nsurl = [NSURL URLWithString:fk::stringToNSString(url)];
            NSString *localPath = nsurl.relativePath;
            animationView = [LOTAnimationView animationWithFilePath:localPath];
        } else {
            NSURL *nsurl = [NSURL URLWithString:fk::stringToNSString(url)];
            animationView = [[LOTAnimationView alloc] initWithContentsOfURL:nsurl];
        }

        [nsView() addSubview:animationView];

        if (running.get()) {
            play();
        }
        if (loop.get()) {
            animationView.loopAnimation = YES;
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
        [animationView playWithCompletion:^(BOOL completed) {
          running = false;
        }];
    }
}
