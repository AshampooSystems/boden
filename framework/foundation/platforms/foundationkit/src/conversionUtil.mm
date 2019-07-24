#import <bdn/foundationkit/conversionUtil.hh>

#ifdef BDN_PLATFORM_IOS

namespace bdn::fk
{
    UIColor *colorToFkColor(const bdn::Color &color, UIColor *defaultColor)
    {
        return [UIColor colorWithRed:color.red() green:color.green() blue:color.blue() alpha:color.alpha()];
    }

    UIFont *fontToFkFont(const bdn::Font &font, UIFont *defaultFont)
    {
        if (!defaultFont) {
            defaultFont = [UIFont systemFontOfSize:[UIFont systemFontSize]];
        }

        UIFontDescriptor *defaultFontDescriptor = defaultFont.fontDescriptor;
        float defaultSize = defaultFont.pointSize;

        float size = defaultSize;

        switch (font.size.type) {
        case Font::Size::Type::Inherit:
        case Font::Size::Type::Medium:
            break;
        case Font::Size::Type::Small:
            size *= 0.75;
            break;
        case Font::Size::Type::XSmall:
            size *= 0.5;
            break;
        case Font::Size::Type::XXSmall:
            size *= 0.25;
            break;
        case Font::Size::Type::Large:
            size *= 1.25;
            break;
        case Font::Size::Type::XLarge:
            size *= 1.5;
            break;
        case Font::Size::Type::XXLarge:
            size *= 1.75;
            break;
        case Font::Size::Type::Percent:
            size *= font.size.value;
            break;
        case Font::Size::Type::Points:
        case Font::Size::Type::Pixels:
            size = font.size.value;
            break;
        }

        UIFontDescriptor *descriptor = nullptr;

        if (!font.family.empty()) {
            descriptor = [[UIFontDescriptor alloc] init];
            descriptor = [descriptor fontDescriptorWithFamily:fk::stringToNSString(font.family)];
        } else {
            descriptor = [defaultFontDescriptor copy];
        }

        descriptor = [descriptor fontDescriptorWithSize:size];
        UIFontDescriptorSymbolicTraits traits = 0x0;

        if (font.style == Font::Style::Italic) {
            traits |= UIFontDescriptorTraitItalic;
        }

        if (font.weight != Font::Weight::Inherit && font.weight != Font::Weight::Normal) {
            if (font.weight == Font::Weight::Bold) {
                traits |= UIFontDescriptorTraitBold;
            } else {
                std::array<UIFontWeight, 9> weights = {UIFontWeightUltraLight, UIFontWeightThin,   UIFontWeightLight,
                                                       UIFontWeightRegular,    UIFontWeightMedium, UIFontWeightSemibold,
                                                       UIFontWeightBold,       UIFontWeightHeavy,  UIFontWeightBlack};

                UIFontWeight w = weights[(font.weight / 1000.0) * 8];

                descriptor = [descriptor fontDescriptorByAddingAttributes:@{
                    UIFontDescriptorTraitsAttribute : @{UIFontWeightTrait : @(w)}
                }];
            }
        }

        if (traits != 0x0) {
            descriptor = [descriptor fontDescriptorWithSymbolicTraits:traits];
        }

        return [UIFont fontWithDescriptor:descriptor size:size];
    }
}

#else

namespace bdn::fk
{
    NSColor *colorToFkColor(const bdn::Color &color, NSColor *defaultColor)
    {
        return [NSColor colorWithRed:color.red() green:color.green() blue:color.blue() alpha:color.alpha()];
    }

    NSFont *fontToFkFont(const bdn::Font &font, NSFont *defaultFont)
    {
        if (!defaultFont) {
            defaultFont = [NSFont systemFontOfSize:[NSFont systemFontSize]];
        }

        float size = defaultFont.pointSize;

        switch (font.size.type) {
        case Font::Size::Type::Inherit:
        case Font::Size::Type::Medium:
            break;
        case Font::Size::Type::Small:
            size *= 0.75;
            break;
        case Font::Size::Type::XSmall:
            size *= 0.5;
            break;
        case Font::Size::Type::XXSmall:
            size *= 0.25;
            break;
        case Font::Size::Type::Large:
            size *= 1.25;
            break;
        case Font::Size::Type::XLarge:
            size *= 1.5;
            break;
        case Font::Size::Type::XXLarge:
            size *= 1.75;
            break;
        case Font::Size::Type::Percent:
            size *= font.size.value;
            break;
        case Font::Size::Type::Points:
        case Font::Size::Type::Pixels:
            size = font.size.value;
            break;
        }

        NSFont *nsFont = nullptr;

        if (font.family.empty()) {
            nsFont = [defaultFont copy];
        } else {
            nsFont = [NSFont fontWithName:fk::stringToNSString(font.family) size:size];
        }

        NSString *fontFamily = nsFont.familyName;

        auto fontManager = [NSFontManager sharedFontManager];

        NSFontTraitMask fontTraits = 0;
        int fontWeight = NSFontWeightRegular;

        if (font.weight == Font::Weight::Inherit) {
            fontWeight = [fontManager weightOfFont:defaultFont];
            fontTraits = [fontManager traitsOfFont:defaultFont];
        } else if (font.weight == Font::Weight::Bold) {
            fontWeight = NSFontWeightBold;
            fontTraits |= NSBoldFontMask;
        } else if (font.weight == Font::Weight::Normal) {
            fontWeight = NSFontWeightRegular;
        } else {
            fontWeight = ((double)font.weight / 1000.0) * 15;
        }

        if (font.style == Font::Style::Italic) {
            fontTraits |= NSItalicFontMask;
        }

        if (font.variant == Font::Variant::SmallCaps) {
            fontTraits |= NSSmallCapsFontMask;
        }

        nsFont = [fontManager fontWithFamily:fontFamily traits:fontTraits weight:fontWeight size:size];

        return nsFont;
    }
}
#endif
