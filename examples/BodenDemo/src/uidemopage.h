#pragma once

#include <bdn/Json.h>
#include <bdn/LottieView.h>
#include <bdn/StopWatch.h>
#include <bdn/Timer.h>
#include <bdn/log.h>
#include <bdn/ui.h>
#include <bdn/yogalayout.h>

using namespace std::string_literals;
using namespace std::chrono_literals;

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

    std::shared_ptr<View> makeRow(String title, std::shared_ptr<View> ctrl)
    {
        auto row = std::make_shared<ContainerView>();

        row->stylesheet = FlexJsonStringify({
            "direction" : "Row",
            "margin" : {"bottom" : 5},
            "flexShrink" : 0.0,
            "alignItems" : "Center",
            "justifyContent" : "SpaceBetween",
            "flexWrap" : "Wrap"
        });

        auto textView = std::make_shared<Label>();
        textView->text = title;
        row->addChildView(textView);
        row->addChildView(ctrl);
        return row;
    }

    auto _styler = std::make_shared<Styler>();

    class WindowMatcher
    {
      public:
        WindowMatcher(std::shared_ptr<Window> window, std::shared_ptr<Styler> styler) : _window(window), _styler(styler)
        {
            windowGeometrySub = _window->geometry.onChange().subscribe([=](auto) { update(); });
            update();
        }

        ~WindowMatcher() { _window->geometry.onChange().unsubscribe(windowGeometrySub); }

        void update()
        {
            // TODO: Allow matcher to change, not rebuild ?
            _styler->setCondition("width", std::make_shared<Styler::equals_condition>(_window->geometry->width));
            _styler->setCondition("height", std::make_shared<Styler::equals_condition>(_window->geometry->height));
            _styler->setCondition("max-width", std::make_shared<Styler::less_condition>(_window->geometry->width));
            _styler->setCondition("max-height", std::make_shared<Styler::less_condition>(_window->geometry->height));
            _styler->setCondition("min-width",
                                  std::make_shared<Styler::greater_equal_condition>(_window->geometry->width));
            _styler->setCondition("min-height",
                                  std::make_shared<Styler::greater_equal_condition>(_window->geometry->height));
        }

        Property<Rect>::backing_t::notifier_t::Subscription windowGeometrySub;
        std::shared_ptr<Window> _window;
        std::shared_ptr<Styler> _styler;
    };

    class DemoDataSource : public ListViewDataSource
    {
      public:
        std::vector<String> _data;

      public:
        DemoDataSource() { _data = {"List entry 1", "List entry 2"}; }

        size_t numberOfRows() override { return _data.size(); }

        std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) override
        {
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(reusableView);

            if (!label) {
                label = std::make_shared<Label>();
            }

            label->text = _data[rowIndex];

            return label;
        }

        float heightForRowIndex(size_t rowIndex) override { return 15.0f; }
    };

    std::shared_ptr<WindowMatcher> _windowMatcher;

    auto createUiDemoPage(std::shared_ptr<Window> window)
    {
        _windowMatcher = std::make_shared<WindowMatcher>(window, _styler);

        _styler->setCondition("os", std::make_shared<Styler::equals_condition>(BDN_TARGET));
        auto container = std::make_shared<ContainerView>();

        container->stylesheet =
            FlexJsonStringify({"direction" : "Column", "flexGrow" : 1.0, "flexShrink" : 1.0, "alignItems" : "Stretch"});

        auto screenOrientationCtrl = std::make_shared<Label>();

        screenOrientationCtrl->text = Window::orientationToString(window->currentOrientation);
        window->currentOrientation.onChange() += [screenOrientationCtrl](auto &property) {
            screenOrientationCtrl->text = Window::orientationToString(property.get());
        };

        container->addChildView(makeRow("Orientation", screenOrientationCtrl));

        auto switchView = std::make_shared<Switch>();
        switchView->label = "I'm a switch!";
        container->addChildView(makeRow("Switch", switchView));

        auto slider = std::make_shared<Slider>();
        container->addChildView(makeRow("Slider", slider));
        slider->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100}});

        auto checkBox = std::make_shared<Checkbox>();
        checkBox->label = "I'm a Checkbox!";

        container->addChildView(makeRow("Checkbox", checkBox));

        auto btn = std::make_shared<Button>();
        btn->label = "Button";
        container->addChildView(makeRow("Button", btn));

        auto bigBtn = std::make_shared<Button>();
        bigBtn->stylesheet = FlexJsonStringify({"minimumSize" : {"height" : 50}});
        bigBtn->label = "Big Button";
        container->addChildView(makeRow("Big Button", bigBtn));

        auto image = std::make_shared<ImageView>();

        image->url = "image://main/images/image.png";
        container->addChildView(makeRow("Image", image));

        image->stylesheet = FlexJsonStringify({"maximumSize" : {"height" : 50}});

        auto lottieView = std::make_shared<LottieView>();
        lottieView->stylesheet = FlexJsonStringify({"size" : {"width" : 50, "height" : 50}});
        lottieView->url = "resource://main/images/animation.json";
        container->addChildView(makeRow("Lottie", lottieView));

        lottieView->running = true;
        lottieView->loop = true;

        auto textFieldCtrl = std::make_shared<TextField>();
        textFieldCtrl->text = "Some text";
        textFieldCtrl->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 250}});
        container->addChildView(makeRow("Text Field", textFieldCtrl));
        ////////////////////////////////////////////////////////////////////////

        auto header = std::make_shared<Label>();
        header->text = "Scrolling multiline text";
        header->stylesheet = FlexJsonStringify({"margin" : {"bottom" : 5}, "flexShrink" : 0.0});

        container->addChildView(header);

        auto textScrollView = std::make_shared<ScrollView>();
        textScrollView->stylesheet = FlexJsonStringify({"flexGrow" : 1, "flexShrink" : 1, "margin" : {"bottom" : 5}});

        auto scrolledTextView = std::make_shared<Label>();
        scrolledTextView->text = testText;

        _styler->setStyleSheet(scrolledTextView, JsonStringify([
                                   {"flex" : {"flexGrow" : 1, "flexShrink" : 0, "margin" : {"all" : 0}}},
                                   {"if" : {"os" : "mac"}, "flex" : {"margin" : {"all" : 5}}}
                               ]));

        textScrollView->contentView = scrolledTextView;

        container->addChildView(textScrollView);

        ////////////////////////////////////////////////////////////////////////

        auto list = std::make_shared<ListView>();
        auto dataSource = std::make_shared<DemoDataSource>();

        list->dataSource = dataSource;
        list->reloadData();
        list->stylesheet = FlexJsonStringify({"flexGrow" : 1.0, "minimumSize" : {"height" : 75}});

        auto buttonRow = std::make_shared<ContainerView>();

        buttonRow->stylesheet = FlexJsonStringify(
            {"direction" : "Row", "flexShrink" : 0.0, "alignItems" : "Center", "justifyContent" : "FlexStart"});

        auto addButton = std::make_shared<Button>();
        addButton->label = "Add";

        addButton->onClick() += [dataSource, list, textFieldCtrl](auto) {
            String text = textFieldCtrl->text.get().empty() ? "New Item" : textFieldCtrl->text.get();
            dataSource->_data.push_back(text);
            list->reloadData();
        };

        auto removeButton = std::make_shared<Button>();
        removeButton->label = "Remove";
        removeButton->onClick() += [list, dataSource](auto) {
            if (!dataSource->_data.empty()) {
                dataSource->_data.pop_back();
                list->reloadData();
            }
        };

        auto clearButton = std::make_shared<Button>();
        clearButton->label = "Clear";
        clearButton->onClick() += [list, dataSource](auto) {
            dataSource->_data.clear();
            list->reloadData();
        };

        buttonRow->addChildView(addButton);
        buttonRow->addChildView(removeButton);
        buttonRow->addChildView(clearButton);

        auto btnStyle = JsonStringify([ {"if" : {"os" : "ios"}, "flex" : {"padding" : {"all" : 5}}} ]);

        _styler->setStyleSheet(addButton, btnStyle);
        _styler->setStyleSheet(removeButton, btnStyle);
        _styler->setStyleSheet(clearButton, btnStyle);

        auto listHeader = std::make_shared<ContainerView>();
        listHeader->stylesheet = FlexJsonStringify({
            "margin" : {"bottom" : 5},
            "direction" : "Row",
            "flexShrink" : 0,
            "alignItems" : "Center",
            "justifyContent" : "SpaceBetween"
        });

        auto listHeaderLabel = std::make_shared<Label>();
        listHeaderLabel->text = "Mutable list";

        listHeader->addChildView(listHeaderLabel);
        listHeader->addChildView(buttonRow);

        container->addChildView(listHeader);
        container->addChildView(list);

        return container;
    }
}
