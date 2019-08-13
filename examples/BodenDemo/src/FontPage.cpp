#include <bdn/Json.h>
#include <bdn/platform.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/ImageView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "FontPage.h"

namespace bdn
{
    class FontTestTextField : public CoreLess<TextField>
    {
      public:
        FontTestTextField(NeedsInit init, std::string t, json style = {}, std::shared_ptr<Styler> styler = nullptr)
            : CoreLess<TextField>(init), _style(style), _styler(styler)
        {
            if (styler) {
                _style[0].merge_patch(FlexJsonStringify({"size" : {"width" : "66%"}}));
            } else {
                _style.merge_patch(FlexJsonStringify({"size" : {"width" : "66%"}}));
                text = t;
            }
        }
        void init() override
        {
            if (auto styler = _styler.lock()) {
                styler->setStyleSheet(shared_from_this(), _style);
            } else {
                stylesheet = _style;
            }
        }

      private:
        json _style;
        std::weak_ptr<Styler> _styler;
    };

    void FontPage::init()
    {
        _styler = std::make_shared<ui::Styler>();
        _styler->setCondition("os", std::make_shared<Styler::equals_condition>(BDN_TARGET));

        stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto normalTextField = std::make_shared<FontTestTextField>(needsInit, "no-font");
        addChildView(makeRow("No-Font", normalTextField, 5., 5., 0.62, true));

        auto defaultFontTextField = std::make_shared<FontTestTextField>(needsInit, "default font");
        defaultFontTextField->stylesheet |= bdn::json({{"font", Font{"Tahoma"}}});
        defaultFontTextField->stylesheet |= bdn::json({{"font", nullptr}});

        addChildView(makeRow("Default font", defaultFontTextField, 5., 5., 0.62, true));

        auto boldTextField = std::make_shared<FontTestTextField>(needsInit, "Bold font");
        boldTextField->stylesheet |= json({{"font", Font{"", {}, Font::Style::Inherit, Font::Weight::Bold}}});
        addChildView(makeRow("Bold", boldTextField, 5., 5., 0.62, true));

        auto boldSlider = std::make_shared<Slider>();
        boldSlider->stylesheet = FlexJsonStringify({"size" : {"width" : "66%"}});

        boldSlider->value.onChange() += [=](auto p) {
            Font f = boldTextField->stylesheet.get()["font"];
            f.weight = (Font::Weight)(boldSlider->value.get() * 1000);
            boldTextField->stylesheet |= json({{"font", f}});
        };

        addChildView(makeRow("Boldness", boldSlider));

        auto italicTextField = std::make_shared<FontTestTextField>(needsInit, "Italic font");
        italicTextField->stylesheet |= json({{"font", Font{"", {}, Font::Style::Italic}}});
        addChildView(makeRow("Italic", italicTextField, 5., 5., 0.62, true));

        auto smallTextField = std::make_shared<FontTestTextField>(needsInit, "small font");
        smallTextField->stylesheet |= json({{"font", Font{"", {Font::Size::Type::Small, 10.f}}}});
        addChildView(makeRow("Small", smallTextField, 5., 5., 0.62, true));

        auto handwritingTextField =
            std::make_shared<FontTestTextField>(needsInit, "", JsonStringify([
                                                    {"font" : {"family" : "Bradley Hand"}}, //
                                                    {"if" : {"os" : "android"}, "font" : {"family" : "cursive"}}
                                                ]),
                                                _styler);

#ifdef BDN_PLATFORM_ANDROID
        handwritingTextField->text = "Cursive";
#else
        handwritingTextField->text = "Bradley Hand font";
#endif

        addChildView(makeRow("Handwriting", handwritingTextField, 5., 5., 0.62, true));

        auto bigHandwritingTextField =
            std::make_shared<FontTestTextField>(needsInit, "", JsonStringify([
                                                    {"font" : {"family" : "Bradley Hand", "size" : "xxlarge"}}, //
                                                    {"if" : {"os" : "android"}, "font" : {"family" : "cursive"}}
                                                ]),
                                                _styler);

#ifdef BDN_PLATFORM_ANDROID
        bigHandwritingTextField->text = "Cursive - Big";
#else
        bigHandwritingTextField->text = "Bradley Hand font - Big";
#endif

        addChildView(makeRow("Handwriting Big", bigHandwritingTextField, 5., 5., 0.62, true));
    }
}
