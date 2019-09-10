#include "UIDemoPage.h"

#include <bdn/foundation.h>
#include <bdn/ui.h>
#include <bdn/ui/lottie/View.h>
#include <bdn/ui/yoga.h>

namespace bdn
{
    static constexpr const char testText[] = "The user interface (UI), in the industrial design field of "
                                             "human–computer interaction, is the space where interactions "
                                             "between humans and machines occur. The goal of this interaction "
                                             "is to allow effective operation and control of the machine from "
                                             "the human end, whilst the machine simultaneously feeds back "
                                             "information that aids the operators' decision-making process. "
                                             "Examples of this broad concept of user interfaces include the "
                                             "interactive aspects of computer operating systems, hand tools, "
                                             "heavy machinery operator controls, and process controls. The "
                                             "design considerations applicable when creating user interfaces "
                                             "are related to or involve such disciplines as ergonomics and "
                                             "psychology.\n\n"
                                             "Generally, the goal of user interface design is to produce a user "
                                             "interface which makes it easy, efficient, and enjoyable "
                                             "(user-friendly) to operate a machine in the way which produces "
                                             "the desired result. This generally means that the operator needs "
                                             "to provide minimal input to achieve the desired output, and also "
                                             "that the machine minimizes undesired outputs to the human.\n\n"
                                             "User interfaces are composed of one or more layers including a "
                                             "human-machine interface (HMI) interfaces machines with physical "
                                             "input hardware such a keyboards, mice, game pads and output "
                                             "hardware such as computer monitors, speakers, and printers. A "
                                             "device that implements a HMI is called a human interface device "
                                             "(HID). Other terms for human-machine interfaces are man–machine "
                                             "interface (MMI) and when the machine in question is a computer "
                                             "human–computer interface. Additional UI layers may interact with "
                                             "one or more human sense, including: tactile UI (touch), visual UI "
                                             "(sight), auditory UI (sound), olfactory UI (smell), equilibrial "
                                             "UI (balance), and gustatory UI (taste).\n\n"
                                             "Composite user interfaces (CUI) are UIs that interact with two or "
                                             "more senses. The most common CUI is a graphical user interface "
                                             "(GUI), which is composed of a tactile UI and a visual UI capable "
                                             "of displaying graphics. When sound is added to a GUI it becomes a "
                                             "multimedia user interface (MUI). There are three broad categories "
                                             "of CUI: standard, virtual and augmented. Standard composite user "
                                             "interfaces use standard human interface devices like keyboards, "
                                             "mice, and computer monitors. When the CUI blocks out the real "
                                             "world to create a virtual reality, the CUI is virtual and uses a "
                                             "virtual reality interface. When the CUI does not block out the "
                                             "real world and creates augmented reality, the CUI is augmented "
                                             "and uses an augmented reality interface. When a UI interacts with "
                                             "all human senses, it is called a qualia interface, named after "
                                             "the theory of qualia. CUI may also be classified by how many "
                                             "senses they interact with as either an X-sense virtual reality "
                                             "interface or X-sense augmented reality interface, where X is the "
                                             "number of senses interfaced with. For example, a Smell-O-Vision "
                                             "is a 3-sense (3S) Standard CUI with visual display, sound and "
                                             "smells; when virtual reality interfaces interface with smells and "
                                             "touch it is said to be a 4-sense (4S) virtual reality interface; "
                                             "and when augmented reality interfaces interface with smells and "
                                             "touch it is said to be a 4-sense (4S) augmented reality "
                                             "interface.\n\nThis text uses material from the Wikipedia article "
                                             "https://en.wikipedia.org/wiki/User_interface";

    class DemoListItemView : public ContainerView
    {
      public:
        virtual const std::type_info &typeInfoForCoreCreation() const { return typeid(ContainerView); }

        std::shared_ptr<Label> label;
    };

    class DemoDataSource : public ListViewDataSource
    {
      public:
        std::vector<std::string> _data;

      public:
        DemoDataSource() { _data = {"List entry 1", "List entry 2"}; }

        size_t numberOfRows(const std::shared_ptr<ListView> &) override { return _data.size(); }

        std::shared_ptr<View> viewForRowIndex(const std::shared_ptr<ListView> &, size_t rowIndex,
                                              std::shared_ptr<View> reusableView) override
        {
            std::shared_ptr<DemoListItemView> itemView = std::dynamic_pointer_cast<DemoListItemView>(reusableView);

            if (!itemView) {
                itemView = std::make_shared<DemoListItemView>();
                itemView->label = std::make_shared<Label>();
                itemView->addChildView(itemView->label);
            }

            itemView->stylesheet = FlexJsonStringify({
                "direction" : "Row",
                "flexGrow" : 1.0,
                "alignItems" : "Center",
                "justifyContent" : "FlexStart",
                "margin" : {"left" : 10, "right" : 10}
            });
            itemView->label->text = _data[rowIndex];

            return itemView;
        }

        float heightForRowIndex(const std::shared_ptr<ListView> &, size_t rowIndex) override { return 40.0f; }
    };

    UIDemoPage::UIDemoPage(bdn::NeedsInit needsInit, std::shared_ptr<Window> window)
        : CoreLess<ContainerView>(needsInit, window->viewCoreFactory()), _styler(std::make_shared<Styler>()),
          _windowMatcher(std::make_shared<WindowMatcher>(window, _styler))
    {}

    void UIDemoPage::init()
    {
        _styler->setCondition("os", std::make_shared<Styler::equals_condition>(BDN_TARGET));

        stylesheet =
            FlexJsonStringify({"direction" : "Column", "flexGrow" : 1.0, "flexShrink" : 1.0, "alignItems" : "Stretch"});

        auto topWhitespaceContainer = std::make_shared<ContainerView>();
        topWhitespaceContainer->stylesheet =
            FlexJsonStringify({"direction" : "Row", "flexGrow" : 0.38, "minimumSize" : {"height" : 40}});
        addChildView(topWhitespaceContainer);

        auto switchView = std::make_shared<Switch>();
        switchView->on = true;
        addChildView(makeRow("Switch", switchView, 0.));

        auto slider = std::make_shared<Slider>();
        addChildView(makeRow("Slider", slider));
        slider->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100}});
        slider->value = 0.5;

        auto checkbox = std::make_shared<Checkbox>();
        checkbox->state = TriState::On;
        addChildView(makeRow("Checkbox", checkbox));

        auto btn = std::make_shared<Button>();
        btn->label = "Button";
        addChildView(makeRow("Button", btn, 5, 5, 0.62, true));

        auto imgBtn = std::make_shared<Button>();
        imgBtn->imageURL = "image://main/images/white-small-icon.png";
        addChildView(makeRow("Image Button", imgBtn, 5, 5, 0.62, false));

        auto image = std::make_shared<ImageView>();

        image->url = "image://main/images/image.png";
        addChildView(makeRow("Image", image));

        image->stylesheet = FlexJsonStringify({"maximumSize" : {"height" : 30}});

        auto lottieView = std::make_shared<lottie::View>();
        lottieView->stylesheet = FlexJsonStringify({"size" : {"width" : 40, "height" : 40}});
        lottieView->url = "resource://main/images/animation.json";
        addChildView(makeRow("Animation", lottieView));

        lottieView->running = true;
        lottieView->loop = true;

        auto textField = std::make_shared<TextField>();
        textField->placeholder = "Enter text here";
        textField->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 150}});
        textField->autocorrectionType = AutocorrectionType::Yes;
        textField->returnKeyType = ReturnKeyType::Done;
        addChildView(makeRow("Text Field", textField, 5., 20., 0.62, true));

        auto textScrollView = std::make_shared<ScrollView>();
        textScrollView->stylesheet = FlexJsonStringify({"flexGrow" : 1, "flexShrink" : 1, "margin" : {"all" : 20}});

        auto scrolledTextView = std::make_shared<Label>();
        scrolledTextView->text = testText;
        scrolledTextView->textOverflow = TextOverflow::ClipWord;
        scrolledTextView->wrap = true;

        _styler->setStyleSheet(scrolledTextView, JsonStringify([
                                   {"flex" : {"flexGrow" : 1, "flexShrink" : 0, "margin" : {"all" : 0}}},
                                   {"if" : {"os" : "mac"}, "flex" : {"margin" : {"all" : 5}}}
                               ]));
        textScrollView->contentView = scrolledTextView;

        addChildView(textScrollView);

        auto bottomWhitespaceContainer = std::make_shared<ContainerView>();
        bottomWhitespaceContainer->stylesheet =
            FlexJsonStringify({"direction" : "Row", "flexGrow" : 0.62, "minimumSize" : {"height" : 40}});
        addChildView(bottomWhitespaceContainer);
    }
}
