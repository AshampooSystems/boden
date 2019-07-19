#include <bdn/foundationkit/AttributedString.hh>
#include <bdn/foundationkit/stringUtil.hh>
#include <bdn/log.h>
#include <bdn/platform.h>

#ifdef BDN_PLATFORM_IOS
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

namespace bdn::fk
{
    AttributedString::AttributedString() { _nsAttributedString = [[NSMutableAttributedString alloc] init]; }

    bool AttributedString::fromHTML(const String &str)
    {
        if (NSData *nsData =
                [[NSData alloc] initWithBytesNoCopy:(void *)str.c_str() length:str.length() freeWhenDone:NO]) {
            NSError *nsError = nullptr;
            NSMutableAttributedString *attrStr = nullptr;

            attrStr = [[NSMutableAttributedString alloc]
                      initWithData:nsData
                           options:@{
                               NSDocumentTypeDocumentAttribute : NSHTMLTextDocumentType,
                               NSCharacterEncodingDocumentAttribute : @(NSUTF8StringEncoding)
                           }
                documentAttributes:nullptr
                             error:&nsError];

            if (nsError) {
                return false;
            }

            if (!attrStr) {
                return false;
            }

            _nsAttributedString = attrStr;

            return true;
        }
        return false;
    }

    String AttributedString::toHTML() const
    {
        if (_nsAttributedString) {

            NSError *nsError = nullptr;
            NSData *data = [_nsAttributedString dataFromRange:NSMakeRange(0, _nsAttributedString.length)
                                           documentAttributes:@{
                                               NSDocumentTypeDocumentAttribute : NSHTMLTextDocumentType,
                                               NSCharacterEncodingDocumentAttribute : @(NSUTF8StringEncoding)
                                           }
                                                        error:&nsError];

            if (data && !nsError) {
                String s((const char *)[data bytes], [data length]);
                return s;
            }
        }

        return {};
    }
}

namespace bdn
{
    std::function<std::shared_ptr<AttributedString>()> AttributedString::defaultCreateAttributedString()
    {
        return []() -> std::shared_ptr<AttributedString> { return std::make_shared<bdn::fk::AttributedString>(); };
    }
}
