#pragma once

#include <bdn/ui.h>
#include <bdn/yogalayout.h>

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

        row->setLayoutStylesheet(FlexStylesheet{.margin.bottom = 5,
                                                .flexDirection = FlexStylesheet::Direction::Row,
                                                .flexShrink = 0.0,
                                                .alignItems = FlexStylesheet::Align::Center,
                                                .justifyContent = FlexStylesheet::Justify::SpaceBetween,
                                                .flexWrap = FlexStylesheet::Wrap::Wrap});

        auto textView = std::make_shared<TextView>();
        textView->text = title;
        row->addChildView(textView);
        row->addChildView(ctrl);
        return row;
    }

    auto createUiDemoPage()
    {
        auto container = std::make_shared<ContainerView>();

        container->setLayoutStylesheet(FlexStylesheet{.flexDirection = FlexStylesheet::Direction::Column,
                                                      .flexGrow = 1.0f,
                                                      .flexShrink = 1.0f,
                                                      .alignItems = FlexStylesheet::Align::Stretch,
                                                      .padding.all = 20,
                                                      .margin.all = 2});

        auto switchView = std::make_shared<Switch>();
        switchView->label = "I'm a switch!";
        container->addChildView(makeRow("Switch", switchView));

        auto checkBox = std::make_shared<Checkbox>();
        checkBox->label = "I'm a Checkbox!";

        container->addChildView(makeRow("Checkbox", checkBox));

        auto btn = std::make_shared<Button>();
        btn->label = "Button";
        container->addChildView(makeRow("Button", btn));

        auto bigBtn = std::make_shared<Button>();

        bigBtn->setLayoutStylesheet(FlexStylesheet{.minimumSize.height = 50});

        bigBtn->label = "Big Button";
        container->addChildView(makeRow("Big Button", bigBtn));

        auto textFieldCtrl = std::make_shared<TextField>();
        textFieldCtrl->text = "Some text";
        textFieldCtrl->setLayoutStylesheet(FlexStylesheet{.minimumSize.width = 250});
        container->addChildView(makeRow("Text Field", textFieldCtrl));

        ////////////////////////////////////////////////////////////////////////

        auto header = std::make_shared<TextView>();
        header->text = "Scrolling multiline text";
        header->setLayoutStylesheet(FlexStylesheet{.margin.bottom = 5, .flexShrink = 0});

        container->addChildView(header);

        auto textScrollView = std::make_shared<ScrollView>();
        textScrollView->setLayoutStylesheet(
            FlexStylesheet{.minimumSize.height = 180, .flexGrow = 1, .margin.bottom = 5});

        auto scrollContainer = std::make_shared<ContainerView>();
        scrollContainer->setLayoutStylesheet(FlexStylesheet{.flexGrow = 1.0f, .flexShrink = 0.0f, .padding.all = 20});

        auto scrolledTextView = std::make_shared<TextView>();
        scrolledTextView->text = testText;

        scrolledTextView->setLayoutStylesheet(FlexStylesheet{.flexGrow = 1.0f, .flexShrink = 0.0f});

        scrollContainer->addChildView(scrolledTextView);
        textScrollView->setContentView(scrollContainer);

        container->addChildView(textScrollView);

        ////////////////////////////////////////////////////////////////////////

        auto fakeList = std::make_shared<ContainerView>();

        String demoEntries[] = {"List Item 1", "List Item 2"};

        for (auto entry : demoEntries) {
            auto newEntry = std::make_shared<TextView>();
            newEntry->text = entry;
            fakeList->addChildView(newEntry);
        }

        auto buttonRow = std::make_shared<ContainerView>();

        buttonRow->setLayoutStylesheet(FlexStylesheet{.flexDirection = FlexStylesheet::Direction::Row,
                                                      .flexShrink = 0.0,
                                                      .alignItems = FlexStylesheet::Align::Center,
                                                      .justifyContent = FlexStylesheet::Justify::FlexStart});

        auto addButton = std::make_shared<Button>();
        addButton->label = "Add";

        addButton->onClick() += [fakeList, textFieldCtrl](auto) {
            auto newEntry = std::make_shared<TextView>();
            newEntry->text = textFieldCtrl->text.get().empty() ? "New Item" : textFieldCtrl->text.get();
            fakeList->addChildView(newEntry);
            // applyLayout(newEntry, fakeList->getLayout());
        };

        auto removeButton = std::make_shared<Button>();
        removeButton->label = "Remove";
        removeButton->onClick() += [fakeList](auto) {
            std::list<std::shared_ptr<bdn::View>> children = fakeList->getChildViews();
            if (children.size() > 0) {
                fakeList->removeChildView(children.back());
            }
        };

        auto clearButton = std::make_shared<Button>();
        clearButton->label = "Clear";
        clearButton->onClick() += [fakeList](auto) { fakeList->removeAllChildViews(); };

        buttonRow->addChildView(addButton);
        buttonRow->addChildView(removeButton);
        buttonRow->addChildView(clearButton);

        auto listHeader = std::make_shared<ContainerView>();
        listHeader->setLayoutStylesheet(FlexStylesheet{.margin.bottom = 5,
                                                       .flexDirection = FlexStylesheet::Direction::Row,
                                                       .flexShrink = 0.0,
                                                       .alignItems = FlexStylesheet::Align::Center,
                                                       .justifyContent = FlexStylesheet::Justify::SpaceBetween});

        auto listHeaderLabel = std::make_shared<TextView>();
        listHeaderLabel->text = "Mutable list";

        listHeader->addChildView(listHeaderLabel);
        listHeader->addChildView(buttonRow);

        container->addChildView(listHeader);
        container->addChildView(fakeList);

        return container;
    }
}
