#include <bdn/Color.h>
#include <bdn/Font.h>
#include <bdn/foundationkit/AttributedString.hh>
#include <bdn/foundationkit/conversionUtil.hh>
#include <bdn/log.h>
#include <bdn/platform.h>
#include <variant>

#ifdef BDN_PLATFORM_IOS
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

namespace bdn::fk
{
    AttributedString::AttributedString() { _nsAttributedString = [[NSMutableAttributedString alloc] init]; }

    NSDictionary *AttributedString::attributeMapToDict(const AttributedString::AttributeMap &attributeMap)
    {
        NSMutableDictionary *dict = [[NSMutableDictionary alloc] init];

        for (auto &attribute : attributeMap) {
            if (attribute.first == "foreground-color") {
                auto color = Color::fromAny(attribute.second);

                [dict setObject:colorToFkColor(color) forKey:NSForegroundColorAttributeName];
            } else if (attribute.first == "font") {
                auto font = Font::fromAny(attribute.second);
                auto fkFont = fontToFkFont(font);

                [dict setObject:fkFont forKey:NSFontAttributeName];
            } else if (attribute.first == "link") {
                auto string = stringToNSString(stringFromAny(attribute.second));
                NSURL *url = [NSURL URLWithString:string];
                [dict setObject:url forKey:NSLinkAttributeName];
            } else if (attribute.first == "baseline-offset") {
                float offset = 0.0f;
                if (attribute.second.type() == typeid(bdn::json)) {
                    offset = (float)std::any_cast<bdn::json>(attribute.second);
                } else if (attribute.second.type() == typeid(float)) {
                    offset = std::any_cast<float>(attribute.second);
                }
                [dict setObject:[NSNumber numberWithInteger:offset] forKey:NSBaselineOffsetAttributeName];
            }
        }

        return dict;
    }

    void AttributedString::setAttributes(AttributeMap attributes, Range range)
    {
        if (range.length == -1) {
            range.length = _nsAttributedString.length - range.start;
        }

        auto nsDict = attributeMapToDict(attributes);
        if (nsDict) {
            [_nsAttributedString setAttributes:nsDict range:NSMakeRange(range.start, range.length)];
        }
    }

    void AttributedString::addAttribute(String attributeName, std::any value, Range range)
    {
        addAttributes({{attributeName, value}}, range);
    }

    void AttributedString::addAttributes(AttributeMap attributes, Range range)
    {
        if (range.length == -1) {
            range.length = _nsAttributedString.length - range.start;
        }

        auto nsDict = attributeMapToDict(attributes);
        if (nsDict) {
            [_nsAttributedString addAttributes:nsDict range:NSMakeRange(range.start, range.length)];
        }
    }

    void AttributedString::fromString(const String &text)
    {
        _nsAttributedString = [[NSMutableAttributedString alloc] initWithString:stringToNSString(text)];
    }

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
