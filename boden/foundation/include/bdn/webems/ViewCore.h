#ifndef BDN_WEBEMS_ViewCore_H_
#define BDN_WEBEMS_ViewCore_H_

#include <bdn/IViewCore.h>
#include <bdn/IdGen.h>
#include <bdn/Dip.h>

#include <bdn/webems/UiProvider.h>

#include <emscripten/val.h>
#include <emscripten/html5.h>

#include <iomanip>

namespace bdn
{
    namespace webems
    {

        class ViewCore : public Base,
                         BDN_IMPLEMENTS IViewCore,
                         BDN_IMPLEMENTS LayoutCoordinator::IViewCoreExtension
        {
          public:
            ViewCore(View *pOuterView, const String &elementName,
                     const std::map<String, String> &styleMap =
                         std::map<String, String>())
                : _domObject(emscripten::val::null())
            {
                _outerViewWeak = pOuterView;

                _elementId = _nextElementId();

                emscripten::val docVal = emscripten::val::global("document");

                _domObject = docVal.call<emscripten::val>("createElement",
                                                          elementName.asUtf8());

                // we always use absolute positioning. Set this here.
                emscripten::val styleObj = _domObject["style"];
                styleObj.set("position", "absolute");

                // we want the padding to be included in the reported box size.
                styleObj.set("box-sizing", "border-box");

                for (auto stylePair : styleMap)
                    styleObj.set(stylePair.first.asUtf8(),
                                 stylePair.second.asUtf8());

                _domObject.set("id", _elementId.asUtf8());

                setVisible(pOuterView->visible());
                setPadding(pOuterView->padding());

                _addToParent(pOuterView->getParentView());
            }

            /** Returns the DOM object that corresponds to this view.*/
            emscripten::val getDomObject() { return _domObject; }

            /** Returns the id of the DOM element that corresponds to the
             * view.*/
            String getHtmlElementId() const { return _elementId; }

            P<View> getOuterViewIfStillAttached() const
            {
                return _outerViewWeak.toStrong();
            }

            void setVisible(const bool &visible) override
            {
                _domObject["style"].set("visibility",
                                        visible ? "visible" : "hidden");
            }

            void setPadding(const Nullable<UiMargin> &padding) override
            {
                // we need to set the padding in the DOM element, so that it can
                // adjust its display and the minimum size accordingly.

                if (padding.isNull()) {
                    setDefaultPadding();
                } else {
                    Margin pad = uiMarginToDipMargin(padding.get());

                    setNonDefaultPadding(pad);
                }
            }

            Rect adjustAndSetBounds(const Rect &bounds) override
            {
                Rect adjustedBounds = adjustBounds(bounds, RoundType::nearest,
                                                   RoundType::nearest);

                emscripten::val styleObj = _domObject["style"];

                // the adjusted bounds are rounded to full integers. So we can
                // just use std::to_string to convert them to a string.

                styleObj.set(
                    "left",
                    std::to_string((int)std::lround(adjustedBounds.x)) + "px");
                styleObj.set(
                    "top",
                    std::to_string((int)std::lround(adjustedBounds.y)) + "px");

                styleObj.set("width", std::to_string((int)std::lround(
                                          adjustedBounds.width)) +
                                          "px");
                styleObj.set("height", std::to_string((int)std::lround(
                                           adjustedBounds.height)) +
                                           "px");

                return adjustedBounds;
            }

            Rect adjustBounds(const Rect &requestedBounds,
                              RoundType positionRoundType,
                              RoundType sizeRoundType) const override
            {
                // we need to round to physical pixels. Unfortunately there is
                // apparently no reliable mechanism to find our the physical
                // pixel resolution at the moment. There is a proposed draft for
                // a Window.devicePixelRatio property, but that is currently not
                // supported by any browsers. There is the
                // -webkit-device-pixel-ratio media property, but that is webkit
                // only and not on the standards track. There is also a
                // "resolution" property, which apparently specifies the pixel
                // density in "DPI". Since the term "DPI" is misused everywhere,
                // it is unclear if we can derive anything useful from it. It
                // might be that resolution/96 gives us the number of physical
                // pixels per "CSS pixel"/DIP, but that is not clear.
                // https://drafts.csswg.org/mediaqueries-3/#resolution

                // So with all this we simply align our stuff on integer DIP
                // boundaries. It is likely that the browser chooses the DIP
                // unit so that it covers an integer number of pixels, otherwise
                // most CSS positioning will cause unaligned controls, which the
                // browser will try to avoid.

                // While doing it this way makes it likely that we actually
                // align on physical pixels, it also means that we do not use
                // the full device resolution for arranging our views. This
                // might cause animations to make bigger position "jumps" than
                // would be necessary, but currently there is not standard
                // compliant way around it.

                return Dip::pixelAlign(requestedBounds, 1, positionRoundType,
                                       sizeRoundType);
            }

            double uiLengthToDips(const UiLength &uiLength) const override
            {
                switch (uiLength.unit) {
                case UiLength::Unit::none:
                    return 0;

                case UiLength::Unit::dip:
                    return uiLength.value;

                case UiLength::Unit::em:
                    return uiLength.value * getEmSizeDips();

                case UiLength::Unit::sem:
                    return uiLength.value * getSemSizeDips();

                default:
                    throw InvalidArgumentError(
                        "Invalid UiLength unit passed to "
                        "ViewCore::uiLengthToDips: " +
                        std::to_string((int)uiLength.unit));
                }
            }

            Margin uiMarginToDipMargin(const UiMargin &margin) const override
            {
                return Margin(
                    uiLengthToDips(margin.top), uiLengthToDips(margin.right),
                    uiLengthToDips(margin.bottom), uiLengthToDips(margin.left));
            }

            void setMargin(const UiMargin &margin) override
            {
                // nothing to do. Our parent handles this.
            }

            void invalidateSizingInfo(View::InvalidateReason reason) override
            {
                // nothing to do since we do not cache sizing info in the core.
            }

            void needLayout(View::InvalidateReason reason) override
            {
                P<View> pOuterView = getOuterViewIfStillAttached();
                if (pOuterView != nullptr) {
                    P<UiProvider> pProvider =
                        tryCast<UiProvider>(pOuterView->getUiProvider());
                    if (pProvider != nullptr)
                        pProvider->getLayoutCoordinator()->viewNeedsLayout(
                            pOuterView);
                }
            }

            void childSizingInfoInvalidated(View *pChild) override
            {
                P<View> pOuterView = getOuterViewIfStillAttached();
                if (pOuterView != nullptr) {
                    pOuterView->invalidateSizingInfo(
                        View::InvalidateReason::childSizingInfoInvalidated);
                    pOuterView->needLayout(
                        View::InvalidateReason::childSizingInfoInvalidated);
                }
            }

            void setHorizontalAlignment(
                const View::HorizontalAlignment &align) override
            {
                // do nothing. The View handles this.
            }

            void
            setVerticalAlignment(const View::VerticalAlignment &align) override
            {
                // do nothing. The View handles this.
            }

            void setPreferredSizeHint(const Size &hint) override
            {
                // nothing to do by default. Most views do not use this.
            }

            void setPreferredSizeMinimum(const Size &limit) override
            {
                // do nothing. The View handles this.
            }

            void setPreferredSizeMaximum(const Size &limit) override
            {
                // do nothing. The View handles this.
            }

            Size calcPreferredSize(
                const Size &availableSpace = Size::none()) const override
            {
                emscripten::val styleObj = _domObject["style"];

                // If we would simply look at the width/height property here, we
                // would only get the size we have last set ourselves. To get
                // the preferred size, we have to update the style to clear the
                // previous size assignment. Then the element will report its
                // preferred size. And then we restore the old style.

                emscripten::val oldWidthStyle = styleObj["width"];
                emscripten::val oldHeightStyle = styleObj["height"];
                emscripten::val oldWhitespaceStyle =
                    emscripten::val::undefined();
                bool restoreWhitespaceStyle = false;

                styleObj.set("width", std::string("auto"));
                styleObj.set("height", std::string("auto"));

                Size limit = availableSpace;

                P<const View> pView = getOuterViewIfStillAttached();
                if (pView != nullptr)
                    limit.applyMaximum(pView->preferredSizeMaximum());

                emscripten::val parentDomObj(emscripten::val::null());
                emscripten::val parentStyleObj(emscripten::val::null());
                emscripten::val oldParentWidthObj(emscripten::val::null());
                bool restoreParentWidth = false;

                bool maxWidthSet = false;
                if (std::isfinite(limit.width)) {
                    // how should we round here?
                    // If we round up then we might cause the content to not fit
                    // in the final view size. If we round down then it might
                    // seem that we could cause the content to wrap
                    // unnecessarily. However, since all view sizes and
                    // positions are rounded to full dips, the parent layout
                    // will work with the preferred size we give it. And if we
                    // round THAT up then we will get the same value in
                    // availableWidth if enough space is available. So that
                    // should be ok.

                    styleObj.set("max-width",
                                 std::to_string((int)std::floor(limit.width)) +
                                     "px");
                    maxWidthSet = true;

                    // if we have a width limit then we cannot disable
                    // auto-wrapping, otherwise we will also not wrap according
                    // to this limit. That creates another problem: if our
                    // parent is currently smaller than this limit then the
                    // auto-wrapping will wrap to make us small enough to fit
                    // into the parent.

                    // So we have to make the parent temporarily bigger to get a
                    // correct size.
                    parentDomObj = _domObject["parentElement"];
                    if (!parentDomObj.isNull()) {
                        // we need to know the parent's current clientWidth and
                        // width

                        double parentWidth =
                            parentDomObj["offsetWidth"].as<double>();
                        double parentClientWidth =
                            parentDomObj["clientWidth"].as<double>();

                        if (parentClientWidth < limit.width) {
                            // we do not fit into the parent. Enlarge it
                            // temporarily.
                            parentStyleObj = parentDomObj["style"];

                            oldParentWidthObj = parentStyleObj["width"];

                            double incBy = limit.width - parentClientWidth + 2;
                            parentStyleObj.set("width",
                                               std::to_string((int)std::ceil(
                                                   parentWidth + incBy)) +
                                                   "px");
                            restoreParentWidth = true;
                        }
                    }

                } else {
                    // our parent's size will influence how our content is
                    // wrapped if our size is set to "auto". For example, for
                    // elements containing text this text will be auto-wrapped
                    // according to the parent's width (since that constitutes
                    // the maximum width for this element, as far as the browser
                    // knows). To avoid this influence of the parent we disable
                    // wrapping during measuring.
                    oldWhitespaceStyle = styleObj["white-space"];

                    std::string newWhitespace = "nowrap";
                    if (!oldWhitespaceStyle.isUndefined()) {
                        String oldWhitespace =
                            oldWhitespaceStyle.as<std::string>();

                        if (oldWhitespace.contains("pre"))
                            newWhitespace = "pre";
                    }

                    styleObj.set("white-space", newWhitespace);

                    restoreWhitespaceStyle = true;
                }

                // none of the html controls can reduce their height. So we
                // ignore the availableHeight parameter completely.

                // we could access the offsetWidth or scrollWidth properties of
                // the object here. However, these have the downside that they
                // are always integers, even though browsers internally use
                // floating point numbers to calculate their box model. And even
                // worse: the integers are rounded down. So if you create a
                // control with the integer width you can get additional word
                // wraps in the text content and other bad effects. To avoid
                // that we call the getBoundingClientRect function. It returns
                // the rect with floating point values, allowing us to round UP
                // when we convert to integers.

                emscripten::val rectObj =
                    _domObject.call<emscripten::val>("getBoundingClientRect");

                double width = rectObj["width"].as<double>();
                double height = rectObj["height"].as<double>();

                /*double offsetWidth = _domObject["offsetWidth"].as<double>();
                double offsetHeight = _domObject["offsetHeight"].as<double>();
                double scrollWidth = _domObject["scrollWidth"].as<double>();
                double scrollHeight = _domObject["offsetHeight"].as<double>();
                std::cerr << width << "x" << height << " offset: " <<
                offsetWidth << "x" << offsetHeight << "scroll " << scrollWidth
                << "x" << scrollHeight << std::endl;*/

                styleObj.set("width", oldWidthStyle);
                styleObj.set("height", oldHeightStyle);

                if (maxWidthSet) {
                    // clear the max width setting again
                    styleObj.set("max-width", "initial");
                }

                if (restoreWhitespaceStyle) {
                    if (oldWhitespaceStyle.isUndefined())
                        styleObj.set("white-space", "initial");
                    else
                        styleObj.set("white-space", oldWhitespaceStyle);
                }

                if (restoreParentWidth) {
                    if (oldParentWidthObj.isUndefined())
                        parentStyleObj.set("width", "auto");
                    else
                        parentStyleObj.set("width", oldParentWidthObj);
                }

                // no scaling necessary. Web browsers already use DIPs.
                // However, we want to report full DIPs only, since we use that
                // as our basic atomic internal unit (because we have no way to
                // find out how big the screen's physical pixels actually are).

                width = std::ceil(width);
                height = std::ceil(height);

                Size prefSize(width, height);

                if (pView != nullptr) {
                    prefSize.applyMinimum(pView->preferredSizeMinimum());
                    prefSize.applyMaximum(pView->preferredSizeMaximum());
                }

                return prefSize;
            }

            void layout() override
            {
                // do nothing by default. Most views do not have subviews.
            }

            bool tryChangeParentView(View *pNewParent) override
            {
                _addToParent(pNewParent);

                return true;
            }

            /** Replaces special characters with the corresponding HTML entities
                (e.g. < becomes &lt;)*/
            static String htmlEscape(const String &text)
            {
                String escaped = text;
                escaped.findAndReplace("&", "&amp;");
                escaped.findAndReplace("\'", "&apos;");
                escaped.findAndReplace("\"", "&quot;");
                escaped.findAndReplace(">", "&gt;");
                escaped.findAndReplace("<", "&lt;");

                return escaped;
            }

            /** Converts a string to html format. Special characters
                are escaped like with htmlEscape.
                Additionally, linebreaks are translated to <br> tags.*/
            static String textToHtmlContent(const String &text)
            {
                String html = htmlEscape(text);

                // replace line breaks with <br> tags
                html.findAndReplace("\r\n", "<br>");
                html.findAndReplace("\n", "<br>");

                return html;
            }

          protected:
            double getEmSizeDips() const
            {
                if (_emDipsIfInitialized == -1) {
                    emscripten::val windowVal =
                        emscripten::val::global("window");

                    emscripten::val computedStyleObj =
                        windowVal.call<emscripten::val>("getComputedStyle",
                                                        _domObject);

                    std::string fontSizeString =
                        computedStyleObj["font-size"].as<std::string>();

                    // if the string ends with "px" then cut it off
                    if (fontSizeString.length() >= 2 &&
                        fontSizeString.substr(fontSizeString.length() - 2, 2) ==
                            "px")
                        fontSizeString = fontSizeString.substr(
                            0, fontSizeString.length() - 2);

                    for (auto c : fontSizeString) {
                        if (!isdigit(c) && c != '.')
                            throw std::runtime_error(
                                "Unsupported computed style value for "
                                "font-size: " +
                                fontSizeString);
                    }

                    std::stringstream s(fontSizeString);
                    double size;
                    s >> size;

                    // the computed font size we get from the DOM is in CSS
                    // pixels (i.e. DIPs). So no conversion necessary.

                    _emDipsIfInitialized = size;
                }

                return _emDipsIfInitialized;
            }

            double getSemSizeDips() const
            {
                if (_semDipsIfInitialized == -1)
                    _semDipsIfInitialized = UiProvider::get().getSemSizeDips();

                return _semDipsIfInitialized;
            }

            /** Used internally - do not call manually.*/
            void _addToParent(View *pParent)
            {
                if (pParent == nullptr) {
                    emscripten::val docVal =
                        emscripten::val::global("document");
                    docVal
                        .call<emscripten::val>("getElementsByTagName",
                                               std::string("body"))[0]
                        .call<void>("appendChild", _domObject);
                } else {
                    P<ViewCore> pParentCore =
                        cast<ViewCore>(pParent->getViewCore());
                    if (pParentCore == nullptr)
                        throw ProgrammingError(
                            "Internal error: parent of bdn::webems::ViewCore "
                            "does not have a core.");

                    pParentCore->_addChildElement(_domObject);
                }
            }

            /** Used internally - do not call manually.*/
            virtual void _addChildElement(emscripten::val childDomObject)
            {
                emscripten::val docVal = emscripten::val::global("document");

                // HDU: why getElementById()? _domObject should do?
                docVal
                    .call<emscripten::val>("getElementById",
                                           getHtmlElementId().asUtf8())
                    .call<void>("appendChild", childDomObject);
            }

            /** This is called when it is requested to set the "default"
             * padding.*/
            virtual void setDefaultPadding()
            {
                // we should use "default" padding. There is "padding=initial",
                // which, according to the standard, should mean "use default
                // padding". However, our tests have shown that padding=initial
                // with buttons sets the padding to 0, while not specifying a
                // padding at all causes the padding to be nonzero. We tested
                // this with Safari and Chrome. So, padding=initial seems to be
                // out as an option. We need to actually remove the entry. Note
                // that apparently the padding cannot be fully removed from the
                // DOM. Even when we assign an empty style object and then
                // access its padding, we will still get an empty object (as
                // opposed to "undefined"). This seems to be a special case for
                // the DOM. So instead we assign an empty string. That seems to
                // do what we want.
                _domObject["style"].set("padding", "");
            }

            /** This is called when a padding is set that is not the implicit
             * default.*/
            virtual void setNonDefaultPadding(const Margin &pad)
            {
                String paddingString;

                // we specify the padding in full DIPs. That seems like a good
                // idea, because we do not know how good the browser is an if it
                // will automatically align the content viewport on physical
                // pixel boundaries if we specify anything weird here.

                paddingString =
                    std::to_string((int)std::ceil(pad.top)) + "px" + " " +
                    std::to_string((int)std::ceil(pad.right)) + "px" + " " +
                    std::to_string((int)std::ceil(pad.bottom)) + "px" + " " +
                    std::to_string((int)std::ceil(pad.left)) + "px";

                _domObject["style"].set("padding", paddingString.asUtf8());
            }

            String _nextElementId()
            {
                return "bdn_view_" + std::to_string(IdGen::newID());
            }

            WeakP<View> _outerViewWeak;
            String _elementId;

            emscripten::val _domObject;

            mutable double _emDipsIfInitialized = -1;
            mutable double _semDipsIfInitialized = -1;
        };
    }
}

#endif
