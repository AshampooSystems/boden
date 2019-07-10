#include <bdn/foundationkit/AttributedString.hh>
#include <bdn/foundationkit/stringUtil.hh>
#include <bdn/platform.h>

#ifdef BDN_PLATFORM_IOS
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

namespace bdn::fk
{
    AttributedString::AttributedString() { _nsAttributedString = [[NSAttributedString alloc] init]; }

    bool AttributedString::fromHtml(const String &str)
    {
        if (NSData *nsData =
                [[NSData alloc] initWithBytesNoCopy:(void *)str.c_str() length:str.length() freeWhenDone:NO]) {
            NSError *nsError = nullptr;
            NSAttributedString *attrStr = nullptr;

            attrStr =
                [[NSAttributedString alloc] initWithData:nsData
                                                 options:@{
                                                     NSDocumentTypeDocumentAttribute : NSHTMLTextDocumentType,
                                                     NSCharacterEncodingDocumentAttribute : @(NSUTF8StringEncoding)
                                                 }
                                      documentAttributes:nil
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
}

namespace bdn
{
    std::function<std::shared_ptr<AttributedString>()> AttributedString::defaultCreateAttributedString()
    {
        return []() -> std::shared_ptr<AttributedString> { return std::make_shared<bdn::fk::AttributedString>(); };
    }
}
